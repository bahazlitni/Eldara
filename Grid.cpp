#include "Grid.h"
#include "App.h"
#include "objects/Alias.h"
#include "objects/Wire.h"
#include "objects/Resistor.h"
#include "objects/Capacitor.h"
#include "objects/Inductor.h"
#include "objects/Battery.h"
#include "objects/DC_VoltageGenerator.h"
#include "objects/DC_CurrentGenerator.h"

#include "tools/MouseTool.h"
#include "utils/Geometry.h"
#include "utils/Functions.h"

#include "objects/BCPath.h"
#include "objects/BCPoint.h"
#include "objects/BCControlPoint.h"

#include <QGraphicsPixmapItem>

#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QMutex>
#include <memory>
#include <QString>
#include <QRegularExpression>


Grid::Grid(App *app): app(app) {
    setX(0.0f);
    setY(0.0f);
    setWidth(app->width());
    setHeight(app->height());
    zoomIndex = DEFAULT_ZOOM_INDEX;
}

void Grid::setZoom(const QPointF &p, const int d){
    const float previousZoom = zoom();
    zoomIndex = std::clamp(zoomIndex + d, 0, MAJ_ZOOM_INDEX);
    setTopLeft(topLeft() + p * (1.0f/previousZoom-1.0f/zoom()));
    setWidth(app->width()/zoom());
    setHeight(app->height()/zoom());
    updateGridTile();
    updateVisibility();
}

void Grid::updateGridTile() {
    // Compute the ideal step in screen pixels.
    float step = tileSize() * zoom();
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

void Grid::drawLabelText(const QString &LABEL, const int HEIGHT){
    painter.save();
    painter.translate(midp - N*(HEIGHT + 12));

    // Rotate
    if (ANGLE > DEG90)  painter.rotate(ANGLE/DEG1 - 180);
    else if (ANGLE < -DEG90) painter.rotate(ANGLE/DEG1 + 180);
    else painter.rotate(ANGLE/DEG1);

    // Draw the text
    QFontMetrics metrics(painter.font());
    const int textWidth = metrics.horizontalAdvance(LABEL);
    painter.drawText(QPoint(-textWidth/2, 0), LABEL);
    painter.restore();
}

// Setup
void Grid::setupPainterMode(PainterMode type, QPainter &painter){
    QPen pen;
    switch(type){
    case SELECTION_BOX:
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setPen(1);
        painter.setPen(WORLD_OBJECT_SELECTION_COLOR);
        painter.setBrush(QColor(
            WORLD_OBJECT_SELECTION_COLOR.red(),
            WORLD_OBJECT_SELECTION_COLOR.green(),
            WORLD_OBJECT_SELECTION_COLOR.blue(),
            25
        ));
        break;
    case INDICATOR_STROKE:
        painter.setRenderHint(QPainter::Antialiasing, false);
        pen.setColor(WORLD_INDICATOR_COLOR);
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        break;
    case SELECTED_OBJECT_STROKE:
        painter.setRenderHint(QPainter::Antialiasing, true);
        pen.setColor(WORLD_SELECTION_BOX_OUTLINE_COLOR);
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        break;
    case GRID_STROKE:
        painter.setRenderHint(QPainter::Antialiasing, false);
        pen.setColor(GRID_STROKE_COLOR);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        break;
    case MERGE_INDICATOR:
        painter.setRenderHint(QPainter::Antialiasing, true);
        pen.setColor(MERGE_INDICATOR_STROKE_COLOR);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        break;
    default:
        break;
    }
}

void Grid::setupDipolePainter(const QColor &color, QPainter &painter){
    QPen pen;
    painter.setRenderHint(QPainter::Antialiasing, true);
    pen.setWidth(STROKE_WIDTH);
    pen.setColor(color);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
}
void Grid::setupDipoleDashedPainter(const QColor &color, QPainter &painter){
    QPen pen;
    painter.setRenderHint(QPainter::Antialiasing, true);
    pen.setWidth(STROKE_WIDTH);
    pen.setColor(color);
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
}

void Grid::setupNodePainter(const QColor &color, QPainter &painter){
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
};



// WIRE
void Grid::drawBCControlPoint(const QPointF &p, const QColor &strokeColor){
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(strokeColor);
    QColor brushColor = strokeColor;
    brushColor.setAlpha(40);

    QPainterPath path;
    path.moveTo(p.x() - BC_EDIT_CONTROL_POINT_DIAGONAL_WIDTH, p.y());
    path.lineTo(p.x(), p.y() - BC_EDIT_CONTROL_POINT_DIAGONAL_WIDTH);
    path.lineTo(p.x() + BC_EDIT_CONTROL_POINT_DIAGONAL_WIDTH, p.y());
    path.lineTo(p.x(), p.y() + BC_EDIT_CONTROL_POINT_DIAGONAL_WIDTH);
    path.closeSubpath();

    painter.setBrush(brushColor);
    painter.drawPath(path);
}

void Grid::drawBCPoint(const QPointF &p, const QColor &strokeColor){
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(strokeColor);
    QColor brushColor = strokeColor;
    brushColor.setAlpha(80);
    painter.setBrush(brushColor);
    painter.drawEllipse(
        p.x() - HALF_BC_EDIT_CONTROL_POINT_WIDTH,
        p.y() - HALF_BC_EDIT_CONTROL_POINT_WIDTH,
        BC_EDIT_CONTROL_POINT_WIDTH,
        BC_EDIT_CONTROL_POINT_WIDTH
    );
}

void Grid::drawBCControlLine(const QPointF &point, const QPointF &controlPoint, const QColor &strokeColor){
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(strokeColor);
    painter.setBrush(Qt::NoBrush);
    painter.drawLine(point.x(), point.y(), controlPoint.x(), controlPoint.y());
}


QPainterPath Grid::constructPath(const BCPath &bcp){
    QPainterPath path;
    auto cur = bcp.first();
    path.moveTo(toScreen(cur->p()));
    while (true) {
        auto next = cur->next();
        if (!next)
            break;

        auto ctrlAfter = cur->after();
        auto ctrlBefore = next->before();
        if (!ctrlAfter || !ctrlBefore)
            break;

        const QPointF p1 = toScreen(ctrlAfter->p());
        const QPointF p2 = toScreen(ctrlBefore->p());
        const QPointF p3 = toScreen(next->p());

        path.cubicTo(p1, p2, p3);
        cur = next;
    }
    return path;
}

void Grid::drawBCPath(const BCPath &bcp, const QColor &color) {
    if (bcp.isEmpty()) return;
    setupDipolePainter(color);
    painter.drawPath(constructPath(bcp));
}
void Grid::drawBCPath(const BCPath &bcp, const QColor &color, const QPointF &preview){
    if (bcp.isEmpty()) return;
    QPainterPath path = constructPath(bcp);
    if(bcp.last() && bcp.last()->after())
        path.cubicTo(toScreen(bcp.last()->after()->p()), toScreen(preview), toScreen(preview));

    setupDipolePainter(color);
    painter.drawPath(path);
};

void Grid::drawBCPathEdits(const BCPath &bcp) {
    if (bcp.isEmpty())
        return;

    auto cur = bcp.first();
    while (cur) {
        auto next = cur->next();
        const QPointF p0 = toScreen(cur->p());
        if (auto ctrlBefore = cur->before()) {
            const QPointF pbef = toScreen(ctrlBefore->p());
            drawBCControlLine(p0, pbef, BC_CONTROL_POINT_COLOR);
            drawBCControlPoint(pbef, BC_CONTROL_POINT_COLOR);
        }
        if (auto ctrlAfter = cur->after()) {
            const QPointF paft = toScreen(ctrlAfter->p());
            drawBCControlLine(p0, paft, BC_CONTROL_POINT_COLOR);
            drawBCControlPoint(paft, BC_CONTROL_POINT_COLOR);
        }
        drawBCPoint(p0, BC_POINT_COLOR);

        cur = cur->next();
    }
}



void Grid::drawNode(const QPointF &center, const QColor &color, int radius) {
    setupNodePainter(color);
    painter.drawEllipse(PtoR(center, radius));
}


// GND
void Grid::drawGround(const QPointF &center, const QColor &color, int radius){
    drawNode(center, color, radius);

    painter.setPen(1);
    painter.setPen(color);
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

// Resistor
void Grid::drawResistor(const QPointF &A, const QPointF &B, const QColor &color){
    setupDipolePainter(color);
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
}






// Capacitor
void Grid::drawCapacitor(const QPointF &A, const QPointF &B, const QColor &color){
    setupDipolePainter(color);
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
}

// Inductor
void Grid::drawInductor(const QPointF &A, const QPointF &B, const QColor &color){
    setupDipolePainter(color);
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
};


// Battery
void Grid::drawBattery(const QPointF &A, const QPointF &B, const QColor &color){
    setupDipolePainter(color);
    setupDipoleDrawingGeometry(A, B);

    QPointF P0 = midp - T*HALF_BATTERY_WIDTH;
    QPointF P1 = midp + T*HALF_BATTERY_WIDTH;

    painter.drawLine(A, P0);
    painter.drawLine(P1, B);
    painter.drawLine(P0 - N*HALF_BATTERY_NEGATIVE_HEIGHT, P0 + N*HALF_BATTERY_NEGATIVE_HEIGHT);
    painter.drawLine(P1 - N*HALF_BATTERY_POSITIVE_HEIGHT, P1 + N*HALF_BATTERY_POSITIVE_HEIGHT);
}


// DC_VoltageGenerator
void Grid::drawDC_VoltageGenerator(const QPointF &A, const QPointF &B, const QColor &color){
    setupDipolePainter(color);
    setupDipoleDrawingGeometry(A, B);

    const QPointF v1 = -QPointF(std::cos(ANGLE-DEFAULT_ARROW_ANGLE), std::sin(ANGLE-DEFAULT_ARROW_ANGLE));
    const QPointF v2 = -QPointF(std::cos(ANGLE+DEFAULT_ARROW_ANGLE), std::sin(ANGLE+DEFAULT_ARROW_ANGLE));

    const QPointF P0 = midp - DEFAULT_CIRCLE_RADIUS*T;
    const QPointF P1 = midp - 0.6*DEFAULT_CIRCLE_RADIUS*T;
    const QPointF P2 = midp + 0.6*DEFAULT_CIRCLE_RADIUS*T;
    const QPointF P3 = P2 + DC_VOLTAGE_GENERATOR_ARROW_H*v1;
    const QPointF P4 = P2 + DC_VOLTAGE_GENERATOR_ARROW_H*v2;
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
}



// DC_CurrentGenerator
void Grid::drawDC_CurrentGenerator(const QPointF &A, const QPointF &B, const QColor &color){
    setupDipolePainter(color);
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
}



void Grid::drawNode(const QPointF &center, const QColor &color, int radius, const QString &label) {
    drawNode(center, color, radius);
    if (label.isEmpty()) return;
    QFontMetrics fm(painter.font());
    const int textWidth = fm.horizontalAdvance(label);
    const int textHeight = fm.height();
    painter.setPen(isDarkColor(color) ? Qt::white : Qt::black);
    painter.drawText(
        center.x() - textWidth/2,
        center.y() - textHeight/2,
        textWidth, textHeight,
        Qt::AlignCenter, label
    );
}


// GND
void Grid::drawGround(const QPointF &center, const QColor &color, int radius, const QString &label) {
    drawNode(center, color, radius, label);
}

// Resistor
void Grid::drawResistor(const QPointF &A, const QPointF &B, const QColor &color, const QString &label){
    drawResistor(A, B, color);
    drawLabelText(label, HALF_RESISTOR_HEIGHT);
}

// Capacitor
void Grid::drawCapacitor(const QPointF &A, const QPointF &B, const QColor &color,  const QString &label){
    drawCapacitor(A, B, color);
    drawLabelText(label, HALF_CAPACITOR_HEIGHT);
}

// Inductor
void Grid::drawInductor(const QPointF &A, const QPointF &B, const QColor &color,  const QString &label){
    drawInductor(A, B, color);
    drawLabelText(label, HALF_INDUCTOR_HEIGHT);
};

// Battery
void Grid::drawBattery(const QPointF &A, const QPointF &B, const QColor &color,  const QString &label){
    drawBattery(A, B, color);
    drawLabelText(label, HALF_BATTERY_POSITIVE_HEIGHT);
}

// DC_VoltageGenerator
void Grid::drawDC_VoltageGenerator(const QPointF &A, const QPointF &B, const QColor &color, const QString &label){
    drawDC_VoltageGenerator(A, B, color);
    drawLabelText(label, DEFAULT_CIRCLE_RADIUS);
}

// DC_CurrentGenerator
void Grid::drawDC_CurrentGenerator(const QPointF &A, const QPointF &B, const QColor &color, const QString &label){
    drawDC_CurrentGenerator(A, B, color);
    drawLabelText(label, DEFAULT_CIRCLE_RADIUS);
}


void Grid::drawDipole(const ObjectType type, const QPointF &A, const QPointF &B, const QColor &color, const QString &label){
    switch (type) {
    case RESISTOR: return drawResistor(A, B, color, label);
    case CAPACITOR: return drawCapacitor(A, B, color, label);
    case INDUCTOR: return drawInductor(A, B, color, label);
    case BATTERY: return drawBattery(A, B, color, label);
    case DC_VOLTAGE_GENERATOR: return drawDC_VoltageGenerator(A, B, color, label);
    case DC_CURRENT_GENERATOR: return drawDC_CurrentGenerator(A, B, color, label);
    default: return;
    }
}

void Grid::drawDipole(const ObjectType type, const QPointF &A, const QPointF &B, const QColor &color) {
    switch (type) {
    case RESISTOR: return drawResistor(A, B, color);
    case CAPACITOR: return drawCapacitor(A, B, color);
    case INDUCTOR: return drawInductor(A, B, color);
    case BATTERY: return drawBattery(A, B, color);
    case DC_VOLTAGE_GENERATOR: return drawDC_VoltageGenerator(A, B, color);
    case DC_CURRENT_GENERATOR: return drawDC_CurrentGenerator(A, B, color);
    default: return;
    }
}


void Grid::drawObject(const SharedObject &obj, const QColor &color){
    switch(obj->type()){
    case ALIAS: {
        auto alias = static_pointer_cast<Alias>(obj);
        if(alias->showLabel()) drawNode(toScreen(alias->p()), color, alias->radius(), alias->name());
        else drawNode(toScreen(alias->p()), color, alias->radius());
        break;
    }
    case WIRE:
        drawBCPath(static_pointer_cast<Wire>(obj)->path(), color);
        break;
    case RESISTOR:
    case CAPACITOR:
    case INDUCTOR:
    case BATTERY:
    case DC_VOLTAGE_GENERATOR:
    case DC_CURRENT_GENERATOR: {
        auto dipole = static_pointer_cast<UnitDipole>(obj);
        if(dipole->showLabel()){
            const QString varname = app->varManager.varname(dipole->share());
            drawDipole(
                dipole->type(),
                toScreen(dipole->A()->p()),
                toScreen(dipole->B()->p()),
                color,
                varname.isEmpty()? VariablesManager::displayDouble(dipole->value(), varTypeOf(dipole->type())) : varname
            );
        }
        else
            drawDipole(
                dipole->type(),
                toScreen(dipole->A()->p()),
                toScreen(dipole->B()->p()),
                color
            );
        break;
    }
    default: break;
    }
}

// Update the grid size and then update the grid cache.
void Grid::updateSize(){
    setSize(app->size()/zoom());
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
    case _NODE:
        addVisible(std::static_pointer_cast<Alias>(obj));
        break;
    case _DIPOLE:
        addVisible(std::static_pointer_cast<Dipole>(obj));
        break;
    default:
        break;
    }
}
void Grid::removeVisible(const SharedObject &obj){
    switch(obj->category()){
    case _NODE:
        removeVisible(std::static_pointer_cast<Alias>(obj));
        break;
    case _DIPOLE:
        removeVisible(std::static_pointer_cast<Dipole>(obj));
        break;
    default:
        break;
    }
}
void Grid::updateVisibility(const SharedObject &obj){
    switch(obj->category()){
    case _NODE:
        updateVisibility(std::static_pointer_cast<Alias>(obj));
        break;
    case _DIPOLE:
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
    const QHash<int, SharedAlias>& aliases = app->aliases;
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
    painter.begin(app);
    painter.setClipRect(toScreen(*this));

    if (zoom() > 1.5f && !gridTile.isNull()) {
        setupPainterMode(GRID_STROKE, painter);
        QRectF viewportRect = toScreen(*this);
        painter.drawTiledPixmap(viewportRect, gridTile, QPointF(x() - snap(x()), y() - snap(y()))*zoom());
    }

    // Draw objects.
    for (const auto &dipole : visibleDipoles){
        if(app->mouse->willDraw(dipole)) continue;
        drawObject(dipole, dipole->color());
    }

    for (const auto &alias : visibleAliases){
        if (app->mouse->willDraw(alias)) continue;
        drawObject(alias, alias->color());
    }

    app->mouse->draw(&painter);
    painter.end();
}
