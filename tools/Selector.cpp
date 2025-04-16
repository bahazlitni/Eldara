#include "Selector.h"
#include "App.h"
#include <QDebug>
#include <QPen>
#include <QCursor>
#include "Grid.h"
#include "commands/MergeSelectionCommand.h"
#include "commands/RemoveObjectsCommand.h"
#include "commands/MovePointsCommand.h"
#include "commands/InsertObjectsCommand.h"

#include "objects/Alias.h"
#include "objects/Dipole.h"
#include "objects/Object.h"
#include "utils/Selection.h"

Selector::Selector(App *app) : MouseTool(app),
    selectorCursor(QCursor(QPixmap(":assets/cursor/selector.png"), 0, 0)),
    draggingCursor(QCursor(QPixmap(":assets/cursor/cross.png"))),
    plusCursor(QCursor(QPixmap(":assets/cursor/selector_plus.png"), 0, 0)),
    minusCursor(QCursor(QPixmap(":assets/cursor/selector_minus.png"), 0, 0))
{}


void Selector::end() {
    box = QRectF(0.0, 0.0, 0.0, 0.0);
    _state = SELECTOR;
    _selection.clear();
    deepSelection.clear();
    movementMap.clear();
    mergeMap.clear();
    draggingObject.reset();
    resetHover();
    toCopySelection.clear();
    emit selectionChanged();
}

void Selector::downL(){
    switch(_state){
    case SELECTOR: {
        if(_hoverCategory == _VOID) {
            setState(SELECTING);
            draggingObject.reset();
            break;
        }
        const auto &hovered = hoveredObject();
        if(!_selection.contains(hovered)) {
            _selection.clear();
            _selection.insert(hovered);
            emit selectionChanged();
        }
        draggingObject = hovered;
        break;
    }
    case SELECTING_PLUS: {
        if(_hoverCategory == _VOID) break;
        const auto &hovered = hoveredObject();
        if(_selection.contains(hovered))
            _selection.remove(hovered);
        else
            _selection.insert(hovered);
        emit selectionChanged();
        break;
    }
    default:
        break;
    }
}

void Selector::upL(){
    draggingObject.reset();
    switch(_state){
    case SELECTING:
    case DRAGGING:
        setState(SELECTOR);
        break;
    default:
        break;
    }
}

void Selector::updateMovement(){
    setTarget(app->grid.snap(worldP()));
    switch(_state){
    case SELECTOR:
    case SELECTING_PLUS:
        resetHover();
        for(const auto &alias : app->grid.visibleAliases){
            if(alias->hover(worldP(), app->grid.zoom())){
                _hoveredObject = alias;
                _hoverCategory = _NODE;
                setTarget(alias->p());
                return;
            }
        }
        for(const auto &dipole : app->grid.visibleDipoles){
            if(dipole->hover(worldP(), app->grid.zoom())){
                _hoveredObject = dipole;
                _hoverCategory = _DIPOLE;
                return;
            }
        }
        break;
    case DRAGGING: {
        _indicators.clear();
        if(ctrl()){
            setTarget(worldP());
            return;
        }

        QVector<QPointF> points;
        switch(draggingObject->category()){
        case _NODE: {
            const auto &draggingAlias = std::static_pointer_cast<Alias>(draggingObject);

            if(shift()) setTarget(pA(worldP(), movementMap[draggingAlias->share()].first));

            const QPointF dp(t() - tDown(Qt::LeftButton));

            for(const auto &alias : app->grid.visibleAliases){
                if(!_selection.contains(alias)) points.append(alias->p());
            }

            setTarget(indicateTarget(app, _indicators, movementMap[draggingAlias->share()].first + dp, points, app->grid.zoom()));
            break;
        }
        case _DIPOLE: {
            const auto &draggingDipole = std::static_pointer_cast<Dipole>(draggingObject);
            const auto &A = draggingDipole->A();
            const auto &B = draggingDipole->B();
            const QPointF A0 = movementMap[A->share()].first;
            const QPointF B0 = movementMap[B->share()].first;

            if(shift()) setTarget(pA(worldP(), (A0 + B0)/2.0));

            const QPointF dp(t() - tDown(Qt::LeftButton));

            for(const auto &alias : app->grid.visibleAliases){
                if(alias != A && alias != B && !_selection.contains(alias)) points.append(alias->p());
            }

            setTarget(t() - B0 - dp +
                indicateTarget(app, _indicators, B0 - A0 +
                indicateTarget(app, _indicators, A0 + dp, points, app->grid.zoom()), points, app->grid.zoom())
            );
            break;
        }
        break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}



void Selector::move(){
    switch(_state){
    case SELECTOR:
        if(draggingObject){
            setState(DRAGGING);
            move();
        }
        break;
    case SELECTING_PLUS:
        if(_hoverCategory == _VOID) app->setCursor(selectorCursor);
        else {
            if(_selection.contains(hoveredObject()))
                app->setCursor(minusCursor);
            else
                app->setCursor(plusCursor);
        }
        break;
    case SELECTING: {
        const QPointF down = app->grid.toWorld(pDown(Qt::LeftButton));
        const QPointF targ = worldP();
        box = QRectF(
            std::min(down.x(), targ.x()),
            std::min(down.y(), targ.y()),
            std::abs(down.x() - targ.x()),
            std::abs(down.y() - targ.y())
        );
        selectInsideBox();
        break;
    }
    case DRAGGING: {
        const QPointF dp = t() - tDown(Qt::LeftButton);
        for(auto sharedP : movementMap.keys()){
            const QPointF updatedP = movementMap[sharedP].first + dp;
            sharedP->setX(updatedP.x());
            sharedP->setY(updatedP.y());
        }
        calcMergeMap();
        emit selectionDragged();
        break;
    }
    default:
        break;
    }
}


QPointF Selector::unitDisplacement(const Qt::Key key){
    switch(key){
    case Qt::Key_Up: return QPointF(0.0, -app->grid.tileSize());
    case Qt::Key_Down: return QPointF(0.0, app->grid.tileSize());
    case Qt::Key_Left: return QPointF(-app->grid.tileSize(), 0.0);
    case Qt::Key_Right: return QPointF(app->grid.tileSize(), 0.0);
    default: return QPointF(0.0, 0.0);
    }
}

void Selector::keyDown(Qt::Key key){
    switch(key){
    case Qt::Key_Space:
        if(_state != SELECTING)
            app->setTempMouse(&app->grabber, Qt::Key_Space);
        break;
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
        setState(SELECTING_PLUS);
        break;
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        deleteSelection();
        break;
    case Qt::Key_A:
        if(ctrl()){
            if(shift()){
                for(const auto &alias : app->aliases){
                    _selection.insert(alias);
                    for(const auto &dipole : alias->connections())
                        _selection.insert(dipole);
                }
            }
            else {
                _selection.clear();
                _selection.reserve(app->grid.visibleAliases.size() + app->grid.visibleDipoles.size());
                for(const auto &alias : app->grid.visibleAliases) _selection.insert(alias);
                for(const auto &dipole : app->grid.visibleDipoles) _selection.insert(dipole);
            }
            emit selectionChanged();
        }
        break;
    case Qt::Key_Escape:
        init();
        break;
    case Qt::Key_P:
        app->setMouse(&app->pen);
        app->pen.setType(ALIAS);
        break;
    case Qt::Key_W:
        app->setMouse(&app->pen);
        app->pen.setType(WIRE);
        break;
    case Qt::Key_R:
        app->setMouse(&app->pen);
        app->pen.setType(RESISTOR);
        break;
    case Qt::Key_C:
        if(ctrl()) toCopySelection = QSet(_selection);
        else {
            app->setMouse(&app->pen);
            app->pen.setType(CAPACITOR);
        }
        break;
    case Qt::Key_V:
        if(ctrl() && !toCopySelection.isEmpty()){
            select(
                copySelection(
                    app,
                    toCopySelection,
                    QPointF(app->grid.tileSize(), app->grid.tileSize())
                )
            );
            toCopySelection = QSet(_selection);
            app->execute(std::make_unique<InsertObjectsCommand>(app, _selection));
        }
        break;
    case Qt::Key_L:
        app->setMouse(&app->pen);
        app->pen.setType(INDUCTOR);
        break;
    case Qt::Key_B:
        app->setMouse(&app->pen);
        app->pen.setType(BATTERY);
        break;
    case Qt::Key_E:
        app->setMouse(&app->pen);
        app->pen.setType(DC_VOLTAGE_GENERATOR);
        break;
    case Qt::Key_I:
        app->setMouse(&app->pen);
        app->pen.setType(DC_CURRENT_GENERATOR);
        break;
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right: {
        if(_state == DRAGGING || _state == SELECTING) break;
        if(!initKeyDrag){
            initKeyDrag = true;
            initMovementMap();
        }
        translate(unitDisplacement(key));
        break;
    }
    default:
        break;
    }
}

void Selector::keyPress(Qt::Key key){
    switch(key){
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right: {
        if(!initKeyDrag || _state == DRAGGING || _state == SELECTING) break;
        translate(unitDisplacement(key));
        break;
    }
    default:
        break;
    }
}

void Selector::keyUp(Qt::Key key){
    switch(key){
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
        setState(SELECTOR);
        break;
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
        if(!initKeyDrag || _state == DRAGGING || _state == SELECTING) break;
        if(!pressed(Qt::Key_Up) &&
            !pressed(Qt::Key_Down) &&
            !pressed(Qt::Key_Left) &&
            !pressed(Qt::Key_Right)
        ) {
            executeDrag();
            initKeyDrag = false;
        }
        break;
    default:
        break;
    }
}

void Selector::drawObject(const SharedObject &obj, const QColor &color){
    QPen pen(obj->pen());
    QBrush brush(obj->brush());
    brush.setColor(color);
    pen.setColor(color);
    app->grid.drawObject(obj, pen, brush);
}

void Selector::draw(QPainter *painter){
    if(_state != DRAGGING){
        const auto &hovered = hoveredObject();
        for(const auto &obj : _selection){
            if(obj->category() == _DIPOLE && hovered != obj)
               drawObject(obj, Palette::SELECT);
        }
        for(const auto &obj : _selection){
            if(obj->category() == _NODE && hovered != obj)
                drawObject(obj, Palette::SELECT);
        }
        if(_hoverCategory != _VOID)
            drawObject(hoveredObject(), Palette::HOVER);
    }

    switch(_state){
    case SELECTING: {
        app->grid.setupPainterMode(SELECTION_BOX);
        painter->drawRect(app->grid.toScreen(box));
        break;
    }
    case DRAGGING: {
        for(const auto &obj : _selection){
            if(obj->category() != _DIPOLE || mergeMap.contains(obj)) continue;
            drawObject(obj, Palette::SELECT);
        }
        for(const auto &obj : _selection){
            if(obj->category() != _NODE || mergeMap.contains(obj)) continue;
            drawObject(obj, Palette::SELECT);
        }

        drawIndicators(painter);
        for(const auto &merging : mergeMap.keys()){
            app->grid.setupPainterMode(MERGE_INDICATOR);
            switch(merging->category()){
            case _NODE: {
                const auto &mergingAlias = std::static_pointer_cast<Alias>(merging);
                const int r = static_cast<int>(mergingAlias->radius());
                const QRectF rect = PtoR(app->grid.toScreen(mergingAlias->p()), r);
                painter->drawEllipse(rect);
                painter->drawLine(rect.topLeft(), rect.bottomRight());
                painter->drawLine(rect.topRight(), rect.bottomLeft());
                break;
            }
            default:
                drawObject(merging, Palette::MERGE_INDICATOR_STROKE);
                break;
            }
        }
        break;
    }
    default:
        break;
    }
}

void Selector::setCursor(const ToolState state){
    switch(state){
    case SELECTOR:
    case SELECTING:
    case SELECTING_PLUS:
        app->setCursor(selectorCursor);
        break;
    case DRAGGING:
        app->setCursor(draggingCursor);
        break;
    default:
        break;
    }
}

void Selector::deleteSelection(){
    if(_selection.isEmpty()) return;
    app->execute(std::make_unique<RemoveObjectsCommand>(app, _selection));
    _selection.clear();
    emit selectionChanged();
}

int Selector::count(ObjectType type) const noexcept {
    int cnt = 0;
    for (const auto &obj : _selection) {
        if (obj->type() == type)
            cnt++;
    }
    return cnt;
}

int Selector::count(ObjectCategory cat) const noexcept {
    int cnt = 0;
    for (const auto &obj : _selection) {
        if (obj->category() == cat)
            cnt++;
    }
    return cnt;
}

SharedObject Selector::first(ObjectType type) const noexcept {
    for (const auto &obj : _selection) {
        if (obj->type() == type)
            return obj;
    }
    return SharedObject();
}

SharedObject Selector::first(ObjectCategory cat) const noexcept {
    for (const auto &obj : _selection) {
        if (obj->category() == cat)
            return obj;
    }
    return SharedObject();
}

Selection Selector::filter(ObjectType type) const noexcept {
    Selection result;
    for (const auto &obj : _selection) {
        if (obj->type() == type)
            result.insert(obj);
    }
    return result;
}

Selection Selector::filter(ObjectCategory cat) const noexcept {
    Selection result;
    for (const auto &obj : _selection) {
        if (obj->category() == cat)
            result.insert(obj);
    }
    return result;
}

void Selector::selectInsideBox() {
    _selection.clear();
    for(const auto &alias : app->grid.visibleAliases){
        if(alias->inside(box, app->grid.zoom())) _selection.insert(alias);
    }
    for(const auto &dipole : app->grid.visibleDipoles){
        if(dipole->inside(box, app->grid.zoom())) _selection.insert(dipole);
    }
}

void Selector::initMovementMap(){
    movementMap.clear();
    for(auto sharedP : GetWorldPointSelection(_selection))
        movementMap.insert(sharedP, qMakePair(*sharedP.get(), *sharedP.get()));
}
void Selector::endMovementMap(){
    for(auto sharedP : movementMap.keys()){
        if(*sharedP.get() == movementMap[sharedP].first) movementMap.remove(sharedP);
        else movementMap[sharedP].second = *sharedP.get();
    }
}


void Selector::setState(const ToolState state){
    switch(state){
    case SELECTOR:
    case SELECTING_PLUS:
        if(_state == DRAGGING) executeDrag();
        else if(_state == SELECTING) {
            selectInsideBox();
            emit selectionChanged();
        }
        draggingObject.reset();
        movementMap.clear();
        mergeMap.clear();
        box = QRectF();
        break;
    case SELECTING: {
        const QPointF p = app->grid.toWorld(pDown(Qt::LeftButton));
        box = QRectF(p.x(), p.y(), 0.0, 0.0);
        break;
    }
    case DRAGGING:
        deepSelection = GetDeepSelection(_selection);
        initMovementMap();
        break;
    default: return;
    }
    _state = state;
    updateMovement();
    MouseTool::setCursor();
}

void Selector::calcMergeMap(){
    mergeMap.clear();
    for(const auto &mergerAlias : app->grid.visibleAliases){
        if(deepSelection.contains(mergerAlias)) continue;
        for(const auto &obj : deepSelection){
            if(obj->category() != _NODE) continue;
            const auto &alias = std::static_pointer_cast<Alias>(obj);
            const float dx = mergerAlias->x() - alias->x();
            const float dy = mergerAlias->y() - alias->y();
            if(dx*dx + dy*dy > EPSILON_SQ) continue;
            mergeMap.insert(alias, mergerAlias);
        }
    }

    for(const auto &obj : deepSelection){
        if(obj->category() != _DIPOLE) continue;
        const auto &mergedDipole = std::static_pointer_cast<Dipole>(obj);
        const auto &mergedA = mergedDipole->A();
        const auto &mergedB = mergedDipole->B();
        const auto &mergingA = mergeMap.contains(mergedA)? mergeMap[mergedA] : mergedA;
        const auto &mergingB = mergeMap.contains(mergedB)? mergeMap[mergedB] : mergedB;

        if(mergedA == mergingA && mergedB == mergingB) continue;

        for(const auto &mergerDipole : app->grid.visibleDipoles){
            if(deepSelection.contains(mergerDipole)) continue;
            const auto &mergerA = mergerDipole->A();
            const auto &mergerB = mergerDipole->B();
            if(
                (mergingA == mergerA && mergingB == mergerB) ||
                (mergingA == mergerB && mergingB == mergerA)
            ) mergeMap.insert(mergedDipole, mergerDipole);
        }

    }
}

void Selector::deepRemoval(const SharedObject &obj){
    if(!obj) return;
    MouseTool::deepRemoval(obj);
    _selection.remove(obj);
    toCopySelection.remove(obj);
    deepSelection.remove(obj);

    if(obj->category() == _NODE){
        const auto &alias = std::static_pointer_cast<Alias>(obj);
        const auto &sharedP = alias->share();
        mergeMap.remove(alias);
        movementMap.remove(sharedP);
    }
    if(draggingObject == obj){
        draggingObject.reset();
        if(_state == DRAGGING) setState(SELECTOR);
    }
}

void Selector::translate(const QPointF &dp){
    if(dp.isNull()) return;
    for(auto sharedP : movementMap.keys()){
        const QPointF updatedP = app->grid.snap(*sharedP.get() + dp);
        sharedP->setX(updatedP.x());
        sharedP->setY(updatedP.y());
    }
    calcMergeMap();
    emit selectionDragged();
}

void Selector::executeDrag(){
    endMovementMap();
    calcMergeMap();

    const bool Move = !movementMap.isEmpty();
    const bool Merge = !mergeMap.isEmpty();

    if(Move && !Merge)
        app->execute(std::make_unique<MovePointsCommand>(app, movementMap));
    else if(Merge && !Move)
        app->execute(std::make_unique<MergeSelectionCommand>(app, mergeMap));
    else if(Move && Merge){
        auto cmd = std::make_unique<ComboCommand>();
        cmd->addCommand(std::make_unique<MovePointsCommand>(app, movementMap));
        cmd->addCommand(std::make_unique<MergeSelectionCommand>(app, mergeMap));
        app->execute(std::move(cmd));
    }
}
