#include "Simulator.h"
#include "Scene.h"

#include "objects/Alias.h"
#include "objects/Resistor.h"
#include "objects/DCV.h"
#include "objects/DCI.h"

#include <QSet>
#include <QList>
#include <QQueue>
#include <QHash>
#include <QThread>
#include <algorithm>
#include "eigen/Eigen"
#include <chrono>
#include <thread>

Simulator::Simulator(Scene *scene)
    : QObject(nullptr)
    , scene(scene)
    , workerThread(new QThread(this))
{
    setFps(60.0f);

    // Move this object into the worker thread
    this->moveToThread(workerThread);

    // When the thread starts, call runLoop()
    connect(workerThread, &QThread::started,
            this,         &Simulator::runLoop);
    // When simulationEnded() is emitted, quit the thread
    connect(this, &Simulator::simulationEnded,
            workerThread, &QThread::quit);
}

Simulator::~Simulator() {
    stop();
    workerThread->wait();
}

void Simulator::reset(){
    if(m_running){
        stop();
        workerThread->wait();
    }
}

void Simulator::run() {
    if (m_running.load()) return;
    workerThread->start();
    workerThread->setPriority(QThread::HighPriority);
}

void Simulator::stop() {
    m_running.store(false);
}

void Simulator::runLoop() {
    m_running.store(true);

    m_elapsed_ns.store(0);
    m_startTime = clock::now();
    emit simulationStarted();

    auto last_tick = clock::now();
    double dt = m_timestep.load();
    auto next_frame = last_tick + std::chrono::duration<double>(dt);

    while (m_running.load()) {
        if (!m_paused.load()) {
            stepOnce();

            auto now_tp = clock::now();
            auto diff_ns = std::chrono::duration_cast<std::chrono::nanoseconds>
                (now_tp - last_tick).count();
            m_elapsed_ns.fetch_add(diff_ns, std::memory_order_relaxed);
            last_tick = now_tp;

            next_frame += std::chrono::duration<double>(dt);
            std::this_thread::sleep_until(next_frame);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            last_tick = clock::now();
            next_frame = last_tick + std::chrono::duration<double>(dt);
        }
    }

    emit simulationEnded();
}


void Simulator::stepOnce() {
    const auto &aliases = scene->aliases;
    int approx = aliases.size();

    // node‑address → alias pointers
    QHash<int64_t, QVector<SharedAlias>> nodeToAliasMap;
    nodeToAliasMap.reserve(approx);

    struct Component { SharedDipole dp; double val; DCRole role; };
    QVector<Component> components;
    components.reserve(approx*2);

    // raw adjacency
    QHash<int64_t, QVector<int64_t>> adj;
    adj.reserve(approx);

    QSet<SharedDipole> visited;
    visited.reserve(approx);

    // === collect nodes & components ===
    for (auto &aliasPtr : aliases) {
        int64_t a_addr = aliasPtr->address();
        nodeToAliasMap[a_addr].append(aliasPtr);

        for (auto &dp : aliasPtr->connections()) {
            if (visited.contains(dp)) continue;
            visited.insert(dp);

            int64_t A = dp->A()->address(), B = dp->B()->address();
            switch (dp->type()) {
            case ObjectType::Capacitor:
                continue;
            case ObjectType::Inductor:
                components.append({dp, 0.0, DCRole::V});
                break;
            case ObjectType::Resistor: {
                double R = std::static_pointer_cast<Resistor>(dp)->R();
                if (R == 0.0) components.append({dp, 0.0, DCRole::V});
                else          components.append({dp, R,   DCRole::R});
                break;
            }
            case ObjectType::DCV:
            case ObjectType::Battery: {
                double V = std::static_pointer_cast<DCV>(dp)->V();
                components.append({dp, V, DCRole::V});
                break;
            }
            case ObjectType::DCI: {
                double I = std::static_pointer_cast<DCI>(dp)->I();
                components.append({dp, I, DCRole::I});
                break;
            }
            default:
                break;
            }

            adj[A].append(B);
            adj[B].append(A);
        }
    }

    // === find connected components of raw node addresses ===
    QList<QSet<int64_t>> ccList;
    QSet<int64_t> seen;
    QVector<int64_t> queue;
    for (auto it = adj.constBegin(); it != adj.constEnd(); ++it) {
        int64_t start = it.key();
        if (seen.contains(start)) continue;
        seen.insert(start);

        QSet<int64_t> comp { start };
        queue.clear(); queue.append(start);
        for (int qi = 0; qi < queue.size(); ++qi) {
            for (auto v : adj[queue[qi]]) {
                if (!seen.contains(v)) {
                    seen.insert(v);
                    comp.insert(v);
                    queue.append(v);
                }
            }
        }
        ccList.append(comp);
    }
    // isolated nodes
    for (auto it = nodeToAliasMap.constBegin(); it != nodeToAliasMap.constEnd(); ++it) {
        if (!seen.contains(it.key()))
            ccList.append(QSet<int64_t>{ it.key() });
    }

    // === solve each sub‑circuit via MNA ===
    for (auto &nodes : ccList) {
        if (nodes.isEmpty()) continue;

        // pick ground as the smallest address
        int64_t reference = *std::min_element(nodes.begin(), nodes.end());

        // filter components touching this subgraph
        QVector<const Component*> sub;
        sub.reserve(components.size());
        for (auto &c : components) {
            int64_t A = c.dp->A()->address(), B = c.dp->B()->address();
            if (nodes.contains(A) || nodes.contains(B))
                sub.append(&c);
        }

        // list of non‑ref nodes
        QVector<int64_t> nodeList = nodes.values();
        nodeList.removeAll(reference);
        std::sort(nodeList.begin(), nodeList.end());
        int N = nodeList.size();

        // split by role
        QVector<const Component*> Rs, Vs, Is;
        for (auto c : sub) {
            if (c->role == DCRole::R) Rs.append(c);
            else if (c->role == DCRole::V) Vs.append(c);
            else if (c->role == DCRole::I) Is.append(c);
        }
        int M = Vs.size(), dim = N + M;
        if (dim == 0) continue;

        // map node→index
        QHash<int64_t,int> idxMap;
        for (int i = 0; i < N; ++i) idxMap[nodeList[i]] = i;

        Eigen::MatrixXd A = Eigen::MatrixXd::Zero(dim,dim);
        Eigen::VectorXd b = Eigen::VectorXd::Zero(dim);

        // stamp resistors
        for (auto c : Rs) {
            double g = 1.0/c->val;
            int64_t a = c->dp->A()->address(), b_ = c->dp->B()->address();
            int i = (a==reference?-1:idxMap.value(a,-1));
            int j = (b_==reference?-1:idxMap.value(b_,-1));
            if (i>=0) {
                A(i,i) += g;
                if (j>=0) {
                    A(i,j) -= g;
                    A(j,i) -= g;
                    A(j,j) += g;
                }
            } else if (j>=0) {
                A(j,j) += g;
            }
        }

        // stamp current sources (KCL)
        for (auto c : Is) {
            double I = c->val;
            int64_t a = c->dp->A()->address(), b_ = c->dp->B()->address();
            int i = (a==reference?-1:idxMap.value(a,-1));
            int j = (b_==reference?-1:idxMap.value(b_,-1));
            if (i>=0) b(i) -= I;
            if (j>=0) b(j) += I;
        }

        // *** NEW: stamp voltage sources with A->B positive convention ***
        for (int k = 0; k < M; ++k) {
            auto c = Vs[k];
            double V = c->val;
            int64_t a = c->dp->A()->address(), b_ = c->dp->B()->address();
            int i = (a==reference?-1:idxMap.value(a,-1));
            int j = (b_==reference?-1:idxMap.value(b_,-1));
            int row = N + k;

            // KCL: +I at A, -I at B  (I flows A→B)
            if (i>=0) A(i,row) = +1;
            if (j>=0) A(j,row) = -1;

            // KVL: V_B - V_A = V
            if (i>=0) A(row,i) = -1;
            if (j>=0) A(row,j) = +1;

            b(row) = V;
        }

        // solve
        Eigen::FullPivLU<Eigen::MatrixXd> lu(A);
        if (!lu.isInvertible()) continue;
        Eigen::VectorXd x = lu.solve(b);

        // write back node voltages
        for (auto &alias : nodeToAliasMap[reference])
            alias->setRefV(0.0);
        for (int i = 0; i < N; ++i) {
            double vi = x(i);
            for (auto &alias : nodeToAliasMap[nodeList[i]])
                alias->setRefV(vi);
        }

        // write back currents
        for (int k = 0; k < M; ++k)
            Vs[k]->dp->setRefCurrent(x(N+k));
        for (auto c : Rs) {
            double va = c->dp->A()->refV(), vb = c->dp->B()->refV();
            c->dp->setRefCurrent((va - vb)/c->val);
        }

        scene->update();
    }
}


void Simulator::draw(QPainter *painter){
    if (!painter->isActive()) return;
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::cyan);

    // how far along (in seconds) since start, excluding pauses
    double t = std::chrono::duration<double>(elapsed_time()).count();

    constexpr float STEP_LENGTH = 10.0f;  // pixels between successive points

    for (const auto &dipole : scene->grid.visibleDipoles) {
        const float I = dipole->refCurrent();

        if(I){
            QPointF A = dipole->p1();
            QPointF B = dipole->p2();

            QVector2D v(B - A);
            float length = v.length();
            if (length <= 0.0f) continue;
            QVector2D dir = v.normalized();

            int n = static_cast<int>(length / STEP_LENGTH);
            float speed = STEP_LENGTH * std::abs(I) * 10.0f;
            float phase = std::fmod(t * speed, STEP_LENGTH);

            // starting base
            QVector2D base;
            if (I >= 0) {
                base = QVector2D(A) + dir * phase;
            } else {
                base = QVector2D(B) - dir * phase;
                dir = -dir;
            }

            for (int i = 0; i < n; ++i) {
                QVector2D p = base + dir * (i * STEP_LENGTH);
                painter->drawEllipse(
                    scene->grid.toScreenX(p.x()) - 2.0f,
                    scene->grid.toScreenY(p.y()) - 2.0f,
                    4.0f, 4.0f
                );
            }
        }

        scene->grid.drawAliasLabel(
            scene->grid.toScreen(dipole->center()) + QPointF(0, -40),
            formatDouble(dipole->refCurrent(), 6, AMP, scene->displayRawValues()),
            Qt::cyan
        );
    }

    // voltages on non‐GND aliases
    for (const auto &alias : scene->grid.visibleAliases) {
        if (alias->gnd()) continue;
        QPointF P = scene->grid.toScreen(*alias) + QPointF(0, -25);
        scene->grid.drawAliasLabel(
            P,
            formatDouble(alias->refV(), 6, VOLT, scene->displayRawValues()),
            Qt::yellow
            );
    }
}

