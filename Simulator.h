#pragma once

#include "utils/Globals.h"
#include <QObject>
#include <atomic>
#include <chrono>

class Scene;
class QPainter;
class QThread;

class Simulator : public QObject {
    Q_OBJECT

public:
    explicit Simulator(Scene *scene);
    ~Simulator() override;

    void run();
    void stop();
    void reset();

    bool isPaused()   const { return m_paused.load(); }
    bool isRunning()  const { return m_running.load(); }

    void draw(QPainter *painter);

    float fps() const { return m_timestep > 0.0f ? 1.0f / m_timestep.load() : 0.0f; }
    void setFps(float newFps) {
        if (newFps <= 0.0f) {
            m_paused.store(true);
        } else {
            m_timestep.store(1.0f / newFps);
            m_paused.store(false);
        }
    }

    std::chrono::steady_clock::duration elapsed_time() const {
        return std::chrono::nanoseconds(m_elapsed_ns.load());
    }

signals:
    void simulationStarted();
    void simulationEnded();

private slots:
    void runLoop();

private:
    Scene *scene;
    QThread* workerThread;

    std::atomic<bool>        m_running{false};
    std::atomic<float>       m_timestep{1.0f/60.0f};
    std::atomic<bool>        m_paused{false};

    using clock     = std::chrono::steady_clock;
    using timepoint = clock::time_point;

    timepoint               m_startTime;
    std::atomic<int64_t>    m_elapsed_ns{0};

    void stepOnce();
};
