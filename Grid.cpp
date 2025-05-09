#include "Grid.h"
#include "Scene.h"
#include "objects/Alias.h"
#include "objects/Resistor.h"

#include "tools/MouseTool.h"
#include "utils/Geometry.h"


#include <QMutex>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

Grid::Grid(Scene *scene): scene(scene) {
    reset();
}

float Grid::snap(float x) const {
    const int TILE_SIZE = scene->tileSize();
    if(x < 0){
        const float r = std::fmod(-x, TILE_SIZE);
        return x + ((2*r > TILE_SIZE ? -TILE_SIZE : 0) + r);
    }
    const float r = std::fmod(x, TILE_SIZE);
    return x + ((2*r > TILE_SIZE ? TILE_SIZE : 0) - r);
}

void Grid::setZoom(const QPointF &p, const int d){
    const float previousZoom = zoom();
    zoomIndex = std::clamp(zoomIndex + d, 0, MAJ_ZOOM_INDEX);
    setTopLeft(topLeft() + p * (1.0f/previousZoom-1.0f/zoom()));
    setWidth(scene->width()/zoom());
    setHeight(scene->height()/zoom());
    updateGridTile();
    updateVisibility();
}

void Grid::reset(){
    zoomIndex = DEFAULT_ZOOM_INDEX;
    setZoom(QPointF(0.0f,0.0f),0);
    moveTo(QPointF(0.0f, 0.0f));
}

void Grid::updateGridTile() {
    // Compute the ideal step in screen pixels.
    float step = scene->tileSize() * zoom();
    // Quantize the step size to an integer for the pixmap.
    int stepInt = std::max(1, static_cast<int>(std::ceil(step)));

    // Create a pixmap with the quantized size.
    QPixmap newTile(stepInt, stepInt);
    newTile.fill(Qt::transparent);

    QPainter tilePainter(&newTile);
    tilePainter.setRenderHint(QPainter::Antialiasing, false);
    setupPainterMode(GRID_STROKE, tilePainter);

    // Draw the grid lines along the left and top edges.
    tilePainter.drawLine(0, 0, 0, stepInt);
    tilePainter.drawLine(0, 0, stepInt, 0);
    tilePainter.end();

    gridTile = newTile;
}

void Grid::drawLabelText(const QString &label, const int height){
    if(label.isEmpty()) return;

    painter.save();
    painter.translate(midp - N*(height + 12));

    // Rotate
    if (ANGLE > DEG90)  painter.rotate(ANGLE/DEG1 - 180);
    else if (ANGLE < -DEG90) painter.rotate(ANGLE/DEG1 + 180);
    else painter.rotate(ANGLE/DEG1);

    // Draw the text
    QFontMetrics metrics(painter.font());
    const int textWidth = metrics.horizontalAdvance(label);
    painter.drawText(QPoint(-textWidth/2, 0), label);
    painter.restore();
}

// Setup
void Grid::setupPainterMode(PainterMode type, QPainter &painter){
    QPen pen;
    switch(type){
    case SELECTION_BOX:
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setPen(1);
        painter.setPen(Palette::SELECT);
        painter.setBrush(QColor(
            Palette::SELECT.red(),
            Palette::SELECT.green(),
            Palette::SELECT.blue(),
            25
        ));
        break;
    case INDICATOR_STROKE:
        painter.setRenderHint(QPainter::Antialiasing, false);
        pen.setColor(Palette::INDICATOR_STROKE);
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        break;
    case SELECTED_OBJECT_STROKE:
        painter.setRenderHint(QPainter::Antialiasing, true);
        pen.setColor(Palette::RUBBER_BAND_FILL);
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        break;
    case GRID_STROKE:
        painter.setRenderHint(QPainter::Antialiasing, false);
        pen.setColor(scene->gridStrokeColor());
        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        break;
    case MERGE_INDICATOR:
        painter.setRenderHint(QPainter::Antialiasing, true);
        pen.setColor(Palette::MERGE_INDICATOR_STROKE);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        break;
    default:
        break;
    }
}


// NODE
void Grid::drawAliasLabel(const QPointF &center, const QString &label, const QColor &color){
    QFontMetrics fm(painter.font());
    const int textWidth = fm.horizontalAdvance(label);
    const int textHeight = fm.height();
    painter.setPen(color);
    painter.drawText(
        center.x() - textWidth/2,
        center.y() - textHeight/2,
        textWidth, textHeight,
        Qt::AlignCenter, label
    );
}

void Grid::drawAlias(
    const QPointF &center,
    const QBrush &brush,
    const int radius,
    const bool gnd,
    const QString &label,
    const QPen &pen
) {
    painter.setBrush(brush);
    painter.setPen(pen);
    painter.drawEllipse(center.x()-radius, center.y()-radius, radius*2, radius*2);

    if(gnd){
        if(pen.style() == Qt::NoPen){
            QPen p(brush.color());
            p.setWidth(1);
            painter.setPen(p);
        }
        else
            painter.setPen(pen);

        painter.setBrush(Qt::NoBrush);

        int y = center.y() + (int) radius;

        QPainterPath path;
        path.moveTo(center.x(), y);

        y += GROUND_ANTENNA_H;
        path.lineTo(center.x(), y);

        path.moveTo(center.x() - GROUND_WIDTH_1 / 2, y);
        path.lineTo(center.x() + GROUND_WIDTH_1 / 2, y);

        y += GROUND_ROW_GAP;
        path.moveTo(center.x() - GROUND_WIDTH_2 / 2, y);
        path.lineTo(center.x() + GROUND_WIDTH_2 / 2, y);

        y += GROUND_ROW_GAP;
        path.moveTo(center.x() - GROUND_WIDTH_3 / 2, y);
        path.lineTo(center.x() + GROUND_WIDTH_3 / 2, y);

        painter.drawPath(path);
    }

    if (label.isEmpty()) return;
    drawAliasLabel(center, label, isDarkColor(brush.color())? Qt::white : Qt::black);
}

// Resistor
void Grid::drawResistor(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label){
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    setupDipoleDrawingGeometry(A, B);
    const QPointF H(N*HALF_RESISTOR_HEIGHT);

    const QPointF STEP(T*RESISTOR_WIDTH/RESISTOR_OSCILLATIONS);

    QPointF P(midp - T*HALF_RESISTOR_WIDTH);
    int sign = 1;

    QPainterPath path;

    // Begin line
    path.moveTo(A);
    path.lineTo(P);

    // First Oscilation
    P += STEP + sign*H;
    path.lineTo(P);
    sign = -sign;

    // Mid Oscillations
    for(int i = 1; i<RESISTOR_OSCILLATIONS-1; i++){
        P += STEP + 2*sign*H;
        path.lineTo(P);
        sign = -sign;
    }

    // Last Oscillation
    P += STEP + sign*H;
    path.lineTo(P);

    // Finish line
    path.lineTo(B);

    painter.drawPath(path);

    drawLabelText(label, HALF_RESISTOR_HEIGHT);
}

// Capacitor
void Grid::drawCapacitor(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label){
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    setupDipoleDrawingGeometry(A, B);
    const QPointF H(N*HALF_CAPACITOR_HEIGHT);

    QPointF P0 = midp - T*HALF_CAPACITOR_WIDTH;
    QPointF P1 = midp + T*HALF_CAPACITOR_WIDTH;

    QPainterPath path;
    path.moveTo(A);
    path.lineTo(P0);
    path.moveTo(P0 + H);
    path.lineTo(P0 - H);
    path.moveTo(P1 + H);
    path.lineTo(P1 - H);
    path.moveTo(P1);
    path.lineTo(B);
    painter.drawPath(path);

    drawLabelText(label, HALF_CAPACITOR_HEIGHT);
}

// Inductor
void Grid::drawInductor(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label){
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    setupDipoleDrawingGeometry(A, B);

    constexpr float K = 0.5522847498;
    const QPointF H(N*HALF_INDUCTOR_HEIGHT);
    const QPointF STEP(T*INDUCTOR_WIDTH/((float) INDUCTOR_OSCILLATIONS));

    QPointF S = midp - T*HALF_INDUCTOR_WIDTH;
    QPointF E = S + STEP + H;
    QPointF dC1 = (STEP/2.0 - H*4.0)*K;
    QPointF dC2 = (STEP/2.0 + H*4.0)*K;

    QPainterPath path;

    // Begin line
    path.moveTo(A);
    path.lineTo(S);

    // First Oscillation
    path.cubicTo(S + dC1, E - dC2, E);

    // Mid Oscillations
    S += H;
    dC1 -= H*K;
    dC2 += H*K;
    for(int i = 1; i < INDUCTOR_OSCILLATIONS-1; i++){
        S += STEP;
        E += STEP;
        path.cubicTo(S + dC1, E - dC2, E);
    }
    S -= H;
    E -= H;
    dC1 += H*2.0*K;
    dC2 -= H*2.0*K;

    // Last Oscillation
    S += STEP;
    E += STEP;
    path.cubicTo(S + dC1, E - dC2, E);

    // Finish line
    path.lineTo(B);

    painter.drawPath(path);

    drawLabelText(label, HALF_INDUCTOR_HEIGHT);
};

// Battery
void Grid::drawBattery(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label){
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    setupDipoleDrawingGeometry(A, B);

    QPointF P0 = midp - T*HALF_BATTERY_WIDTH;
    QPointF P1 = midp + T*HALF_BATTERY_WIDTH;

    painter.drawLine(A, P0);
    painter.drawLine(P1, B);
    painter.drawLine(P0 - N*HALF_BATTERY_NEGATIVE_HEIGHT, P0 + N*HALF_BATTERY_NEGATIVE_HEIGHT);
    painter.drawLine(P1 - N*HALF_BATTERY_POSITIVE_HEIGHT, P1 + N*HALF_BATTERY_POSITIVE_HEIGHT);

    drawLabelText(label, HALF_BATTERY_POSITIVE_HEIGHT);
}

// DCV
void Grid::drawDCV(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label){
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    setupDipoleDrawingGeometry(A, B);

    const QPointF v1 = -QPointF(std::cos(ANGLE-DEFAULT_ARROW_ANGLE), std::sin(ANGLE-DEFAULT_ARROW_ANGLE));
    const QPointF v2 = -QPointF(std::cos(ANGLE+DEFAULT_ARROW_ANGLE), std::sin(ANGLE+DEFAULT_ARROW_ANGLE));

    const QPointF P0 = midp - DEFAULT_CIRCLE_RADIUS*T;
    const QPointF P1 = midp - 0.6*DEFAULT_CIRCLE_RADIUS*T;
    const QPointF P2 = midp + 0.6*DEFAULT_CIRCLE_RADIUS*T;
    const QPointF P3 = P2 + DCV_ARROW_H*v1;
    const QPointF P4 = P2 + DCV_ARROW_H*v2;
    const QPointF P5 = midp + DEFAULT_CIRCLE_RADIUS*T;

    QPainterPath path;
    path.moveTo(A);
    path.lineTo(P0);
    path.moveTo(P1);
    path.lineTo(P2);
    path.moveTo(P3);
    path.lineTo(P2);
    path.lineTo(P4);
    path.moveTo(P5);
    path.lineTo(B);
    painter.drawPath(path);

    painter.drawEllipse(midp, DEFAULT_CIRCLE_RADIUS, DEFAULT_CIRCLE_RADIUS);

    drawLabelText(label, DEFAULT_CIRCLE_RADIUS);
}

// DCI
void Grid::drawDCI(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label){
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    setupDipoleDrawingGeometry(A, B);

    const QPointF P0 = midp - DEFAULT_CIRCLE_RADIUS*T;
    const QPointF P1 = midp - DEFAULT_CIRCLE_RADIUS*N;
    const QPointF P2 = midp + DEFAULT_CIRCLE_RADIUS*N;
    const QPointF P3 = midp + DEFAULT_CIRCLE_RADIUS*T;

    QPainterPath path;
    path.moveTo(A);
    path.lineTo(P0);
    path.moveTo(P1);
    path.lineTo(P2);
    path.moveTo(P3);
    path.lineTo(B);
    painter.drawPath(path);

    painter.drawEllipse(midp, DEFAULT_CIRCLE_RADIUS, DEFAULT_CIRCLE_RADIUS);
    drawLabelText(label, DEFAULT_CIRCLE_RADIUS);
}

void Grid::drawDipole(const ObjectType type, const QPointF &A, const QPointF &B, const QPen &pen, const QString &label){
    switch (type) {
    case ObjectType::Resistor: return drawResistor(A, B, pen, label);
    case ObjectType::Capacitor: return drawCapacitor(A, B, pen, label);
    case ObjectType::Inductor: return drawInductor(A, B, pen, label);
    case ObjectType::Battery: return drawBattery(A, B, pen, label);
    case ObjectType::DCV: return drawDCV(A, B, pen, label);
    case ObjectType::DCI: return drawDCI(A, B, pen, label);
    default: return;
    }
}

void Grid::drawObject(const SharedObject &obj, const QPen &pen, const QBrush &brush){
    if(const auto &a = dynamic_pointer_cast<Alias>(obj))
        drawAlias(
            toScreen(a->p()), brush,
            a->visualRadius(), a->gnd(),
            a->showLabel() ? a->label(scene->displayRawValues()) : "",
            pen
        );
    else if(const auto &d = dynamic_pointer_cast<Dipole>(obj))
        drawDipole(
            d->type(),
            toScreen(d->p1()),
            toScreen(d->p2()),
            pen, d->showLabel()? d->label(scene->displayRawValues()) : ""
        );
}

// Update the grid size and then update the grid cache.
void Grid::updateSize(){
    setSize(scene->size()/zoom());
    updateGridTile();
    updateVisibility();
}

void Grid::updateVisibility(const SharedAlias &alias){
    if(alias->visible(*this, zoom())) addVisible(alias);
    else removeVisible(alias);
}
void Grid::updateVisibility(const SharedDipole &dipole){
    if(dipole->visible(*this, zoom())) addVisible(dipole);
    else removeVisible(dipole);
}

void Grid::addVisible(const SharedObject &obj){
    switch(obj->category()){
    case ObjectCategory::Node:
        addVisible(std::static_pointer_cast<Alias>(obj));
        break;
    case ObjectCategory::Dipole:
        addVisible(std::static_pointer_cast<Dipole>(obj));
        break;
    default:
        break;
    }
}
void Grid::removeVisible(const SharedObject &obj){
    switch(obj->category()){
    case ObjectCategory::Node:
        removeVisible(std::static_pointer_cast<Alias>(obj));
        break;
    case ObjectCategory::Dipole:
        removeVisible(std::static_pointer_cast<Dipole>(obj));
        break;
    default:
        break;
    }
}
void Grid::updateVisibility(const SharedObject &obj){
    switch(obj->category()){
    case ObjectCategory::Node:
        updateVisibility(std::static_pointer_cast<Alias>(obj));
        break;
    case ObjectCategory::Dipole:
        updateVisibility(std::static_pointer_cast<Dipole>(obj));
        break;
    default:
        break;
    }
}

void Grid::updateVisibility() {
    // Create temporary containers for thread-safe collection
    QSet<SharedAlias> threadSafeVisibleAliases;
    QSet<SharedDipole> threadSafeVisibleDipoles;
    QMutex aliasMutex, dipoleMutex;

    const bool NOT_dirtyVisibleCheckFlagInitial = !dirtyVisibleCheckFlagInitial;

    // Prepare for parallel processing
    const QHash<int, SharedAlias>& aliases = scene->aliases;
    const QList<int> aliasKeys = aliases.keys(); // Get keys for indexing
    const int totalAliases = aliasKeys.size();

    threadSafeVisibleAliases.reserve(visibleAliases.size());
    threadSafeVisibleDipoles.reserve(visibleDipoles.size());

    const int idealThreadCount = QThread::idealThreadCount();
    const int chunkSize = qMax(1, totalAliases / idealThreadCount);

    const int assumptionThreadSizeForAliases = visibleAliases.size()/idealThreadCount;
    const int assumptionThreadSizeForDipoles = visibleDipoles.size()/idealThreadCount;

    // Create a list of futures for parallel execution
    QList<QFuture<void>> futures;

    for (int startIdx = 0; startIdx < totalAliases; startIdx += chunkSize) {
        int endIdx = qMin(startIdx + chunkSize, totalAliases);

        // Process each chunk in a separate thread
        futures.append(QtConcurrent::run([&, startIdx, endIdx, NOT_dirtyVisibleCheckFlagInitial]() {
            QSet<SharedAlias> localVisibleAliases;
            QSet<SharedDipole> localVisibleDipoles;
            localVisibleAliases.reserve(assumptionThreadSizeForAliases);
            localVisibleDipoles.reserve(assumptionThreadSizeForDipoles);

            for (int i = startIdx; i < endIdx; i++) {
                const int aliasKey = aliasKeys[i];
                const auto& alias = aliases[aliasKey];

                if (alias->visible(*this, zoom())) {
                    localVisibleAliases.insert(alias);
                    for (const auto& dipole : alias->connections()) {
                        if (dipole->dirtyVisibleCheckFlag == dirtyVisibleCheckFlagInitial) {
                            localVisibleDipoles.insert(dipole);
                        }
                        dipole->dirtyVisibleCheckFlag = NOT_dirtyVisibleCheckFlagInitial;
                    }
                } else {
                    for (const auto& dipole : alias->connections()) {
                        if (dipole->dirtyVisibleCheckFlag == dirtyVisibleCheckFlagInitial &&
                            dipole->visible(*this, zoom())) {
                            localVisibleDipoles.insert(dipole);
                        }
                        dipole->dirtyVisibleCheckFlag = NOT_dirtyVisibleCheckFlagInitial;
                    }
                }
            }

            if (!localVisibleAliases.isEmpty()) {
                QMutexLocker aliasLocker(&aliasMutex);
                threadSafeVisibleAliases.unite(localVisibleAliases);
            }

            if (!localVisibleDipoles.isEmpty()) {
                QMutexLocker dipoleLocker(&dipoleMutex);
                threadSafeVisibleDipoles.unite(localVisibleDipoles);
            }
        }));
    }

    for (auto& future : futures) future.waitForFinished();

    visibleAliases.clear();
    visibleDipoles.clear();

    visibleAliases = std::move(threadSafeVisibleAliases);
    visibleDipoles = std::move(threadSafeVisibleDipoles);

    dirtyVisibleCheckFlagInitial = NOT_dirtyVisibleCheckFlagInitial;
}

// Render the grid and the rest of the scene.
void Grid::render([[maybe_unused]] QPaintEvent *event){
    QRectF viewportRect = toScreen(*this);

    painter.begin(scene);
    painter.setClipRect(viewportRect);
    painter.fillRect(viewportRect, scene->backgroundColor());

    if (scene->showGrid() && zoom() > 1.5f && !gridTile.isNull()) {
        setupPainterMode(GRID_STROKE, painter);
        painter.drawTiledPixmap(viewportRect, gridTile, QPointF(x() - snap(x()), y() - snap(y()))*zoom());
    }

    // Draw objects.
    for (const auto &dipole : visibleDipoles){
        if(scene->mouse->willDraw(dipole)) continue;
        drawDipole(
            dipole->type(),
            toScreen(dipole->p1()),
            toScreen(dipole->p2()),
            dipole->pen(),
            dipole->showLabel() ? dipole->label(scene->displayRawValues()) : ""
        );
    }

    for (const auto &alias : visibleAliases){
        if (scene->mouse->willDraw(alias)) continue;
        drawAlias(
            toScreen(alias->p()), alias->brush(),
            alias->visualRadius(), alias->gnd(),
            alias->showLabel() ? alias->label(true) : "",
            alias->pen()
        );
    }

    if(scene->simulator.isRunning())
        scene->simulator.draw(&painter);

    scene->mouse->draw(&painter);

    painter.end();
}
