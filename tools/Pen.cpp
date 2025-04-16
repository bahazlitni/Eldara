#include "Pen.h"
#include "App.h"
#include "objects/Object.h"
#include "objects/Alias.h"
#include "objects/BCControlPoint.h"
#include "objects/BCPath.h"
#include "objects/BCPoint.h"
#include "objects/Battery.h"
#include "objects/Capacitor.h"
#include "objects/DC_CurrentGenerator.h"
#include "objects/DC_VoltageGenerator.h"
#include "objects/Inductor.h"
#include "objects/Resistor.h"
#include "objects/WorldPoint.h"

#include "utils/Geometry.h"

#include "commands/Command.h"
#include "commands/SplitDipoleCommand.h"
#include "commands/InsertObjectsCommand.h"
#include "commands/Timeline.h"
#include "Grid.h"
#include "tools/Grabber.h"
#include "tools/Selector.h"


Pen::Pen(App *app): MouseTool(app),
    penCursor(QCursor(QPixmap(":assets/cursor/pen.png"), 0, 0)),
    constructingCursor(QCursor(QPixmap(":assets/cursor/pen_constructing.png"), 0, 0)),
    constructingPlusCursor(QCursor(QPixmap(":assets/cursor/pen_constructing_plus.png"), 0, 0))
{
    setFillColor(QColor(255,255,255));
    setStrokeColor(QColor(222,222,222));
    setStrokeWidth(2);
    setRadius(10);

    _brush.setStyle(Qt::BrushStyle::SolidPattern);
    _pen.setStyle(Qt::PenStyle::SolidLine);
}

QVariant Pen::getAttr(const Attr attr) const {
    switch(attr){
    case Attr::Radius : return radius();
    case Attr::FillColor : return fillColor();
    case Attr::StrokeColor : return strokeColor();
    case Attr::StrokeWidth : return strokeWidth();
    case Attr::ShowLabel : return showLabel();
    default: return MouseTool::getAttr(attr);
    }
}
void Pen::setAttr(const Attr attr, const QVariant &v) {
    switch(attr){
    case Attr::Radius : setRadius(v.toUInt()); return;
    case Attr::FillColor : setFillColor(v.toString()); return;
    case Attr::StrokeColor : setStrokeColor(v.toString()); return;
    case Attr::StrokeWidth : setStrokeWidth(v.toUInt()); return;
    case Attr::ShowLabel : setShowLabel(v.toBool()); return;
    default: MouseTool::setAttr(attr, v); return;
    }
}


void Pen::deepRemoval(const SharedObject &obj) {
    MouseTool::deepRemoval(obj);
    if(obj && obj->category() == _NODE) {
        const auto &alias = static_pointer_cast<Alias>(obj);
        if(alias == previous) end();
    }
}


void Pen::setType(ObjectType type){
    if(type == this->type) return;
    this->type = type;
    if(type == ALIAS) setState(PEN);
    else if(onControl()){
        _path.clear();
        setState(CONSTRUCTING);
    }
}

void Pen::end(){
    setState(PEN);
}

void Pen::move(){
    switch(_state){
    case PEN:
    case CONSTRUCTING:
        break;
    case CONTROLLING:
    case INITIAL_CONTROLLING: {
        if(_path.isEmpty()) break;
        _path.last()->after()->setX(tx());
        _path.last()->after()->setY(ty());
        _path.last()->before()->setX(2*_path.last()->x() - tx());
        _path.last()->before()->setY(2*_path.last()->y() - ty());
        break;
    }
    default:
        break;
    }
}

SharedAlias Pen::MakeAlias(const QPointF &p){
    const QString address = previous && type == ALIAS && alt() ? previous->address() : app->address();
    return std::make_shared<Alias>(app->id(), address, p.x(), p.y(), radius(), brush(), showLabel());
}
SharedDipole Pen::MakeDipole(const SharedAlias &A, const SharedAlias &B, ObjectType type){
    switch(type){
    case RESISTOR:
        return std::make_shared<Resistor>(A, B, pen(), showLabel(), DEFAULT_RESISTANCE_VALUE);

    case CAPACITOR:
        return std::make_shared<Capacitor>(A, B, pen(), showLabel(), DEFAULT_CAPACITANCE_VALUE, 0.0);

    case INDUCTOR:
        return std::make_shared<Inductor>(A, B, pen(), showLabel(), DEFAULT_INDUCTANCE_VALUE, 0.0);

    case BATTERY:
        return std::make_shared<Battery>(A, B, pen(), showLabel(), DEFAULT_BATTERY_VALUE);

    case DC_VOLTAGE_GENERATOR:
        return std::make_shared<DC_VoltageGenerator>(A, B, pen(), showLabel(), DEFAULT_DC_VOLTAGE_GENERATOR_VALUE);

    case DC_CURRENT_GENERATOR:
        return std::make_shared<DC_CurrentGenerator>(A, B, pen(), showLabel(), DEFAULT_DC_CURRENT_GENERATOR_VALUE);

    default: return nullptr;
    }
}


void Pen::stackBCPoint(const QPointF &p){
    auto before = std::make_shared<BCControlPoint>(p);
    auto after = std::make_shared<BCControlPoint>(p);
    auto bcpoint = std::make_shared<BCPoint>(p, before, after);
    _path.stack(bcpoint);
}
void Pen::stackBCPoint(const SharedPoint &p){
    auto before = std::make_shared<BCControlPoint>(*p);
    auto after = std::make_shared<BCControlPoint>(*p);
    auto bcpoint = std::make_shared<BCPoint>(p, before, after);
    _path.stack(bcpoint);
}


SharedDipole Pen::hoveredDipole() { return static_pointer_cast<Dipole>(hoveredObject()); }
SharedAlias Pen::hoveredAlias() { return static_pointer_cast<Alias>(hoveredObject()); }

void Pen::analyze(){
    switch(_state){
    case PEN:
        switch(_hoverCategory){
        case _DIPOLE: mode = ALIAS_SPLIT; return;
        case _NODE: mode = SWITCH_PREVIOUS; return;
        case _VOID: mode = ALIAS_ONLY; return;
        default: mode = PROHIBITED; return;
        }
    case CONSTRUCTING: {
        if(_hoverCategory == _NODE && previous == hoveredAlias()){
            mode = SWITCH_PREVIOUS;
            return;
        }
        SharedDipole lappingHoveredDipole;
        const QLineF constructionLine = QLineF(previous->p(), t());

        if(_hoverCategory == _DIPOLE && hoveredDipole()->connectedTo(previous)){
            mode = OVER_SPLIT;
            return;
        }

        for(const auto &dipole : app->grid.visibleDipoles){
            if(T_LlapL(constructionLine, dipole->line()) != QLineF::UnboundedIntersection) continue;
            if(_hoverCategory == _DIPOLE && hoveredDipole() == dipole){
                lappingHoveredDipole = dipole;
                continue;
            }
            mode = _hoverCategory == _NODE ? SWITCH_PREVIOUS : PROHIBITED;
            return;
        }

        if(lappingHoveredDipole){
            mode = lappingHoveredDipole->connectedTo(previous) ? OVER_SPLIT : PROHIBITED;
            return;
        }

        switch(_hoverCategory){
        case _DIPOLE: mode = NORMAL_SPLIT; return;
        case _NODE: mode = DIPOLE_ONLY; return;
        case _VOID: mode = ALIAS_AND_DIPOLE; return;
        default: mode = PROHIBITED; return;
        }
        break;
    }
    default:
        mode = PROHIBITED;
        break;
    }
}

void Pen::construct(){
    switch (mode) {
    case PROHIBITED: return;
    case ALIAS_ONLY:
        previous = MakeAlias(t());
        app->execute(std::make_unique<InsertObjectsCommand>(app, Selection({previous})));
        break;
    case ALIAS_AND_DIPOLE: {
        const auto &newAlias = MakeAlias(t());
        app->execute(std::make_unique<InsertObjectsCommand>(app, Selection({newAlias, MakeDipole(previous, newAlias)})));
        previous = newAlias;
        break;
    }
    case DIPOLE_ONLY: {
        const auto &hoveredAlias = static_pointer_cast<Alias>(hoveredObject());
        app->execute(
            std::make_unique<InsertObjectsCommand>(
                app, Selection({MakeDipole(previous, hoveredAlias)})
            )
        );
        previous = hoveredAlias;
        break;
    }
    case ALIAS_SPLIT: {
        const auto &splitter = MakeAlias(t());
        const auto &splitted = hoveredDipole();
        const auto &resultant = MakeDipole(splitter, splitted->B(), splitted->type());

        auto cmd = std::make_unique<ComboCommand>();
        cmd->addCommand(
            std::make_unique<InsertObjectsCommand>(app, Selection({splitter, resultant}))
        );
        cmd->addCommand(
            std::make_unique<SplitDipoleCommand>(app, splitted, resultant, splitter)
        );
        app->execute(std::move(cmd));
        previous = splitter;
        break;
    }
    case NORMAL_SPLIT: {
        const auto &splitter = MakeAlias(t());
        const auto &splitted = hoveredDipole();
        const auto &resultant = MakeDipole(splitter, splitted->B(), splitted->type());
        const auto &normal = MakeDipole(previous, splitter);

        auto cmd = std::make_unique<ComboCommand>();
        cmd->addCommand(
            std::make_unique<InsertObjectsCommand>(app, Selection({splitter, resultant, normal}))
        );
        cmd->addCommand(
            std::make_unique<SplitDipoleCommand>(app, splitted, resultant, splitter)
        );
        app->execute(std::move(cmd));
        previous = splitter;
        break;
    }
    case OVER_SPLIT: {
        const auto &splitter = MakeAlias(t());
        const auto &splitted = hoveredDipole();
        const auto &resultant = MakeDipole(previous, splitter);

        auto cmd = std::make_unique<ComboCommand>();
        cmd->addCommand(
            std::make_unique<InsertObjectsCommand>(app, Selection({splitter, resultant}))
        );
        cmd->addCommand(
            std::make_unique<SplitDipoleCommand>(app, splitted, resultant, splitter)
        );
        app->execute(std::move(cmd));
        previous = splitter;
        break;
    }
    case SWITCH_PREVIOUS:
        previous = static_pointer_cast<Alias>(hoveredObject());
        break;
    }
}

void Pen::downL(){
    analyze();
    construct();
    switch(_state){
    case PEN:
        if(type != ALIAS && previous)
            setState(CONSTRUCTING);
        break;
    default:
        break;
    }
}

void Pen::downR(){}

void Pen::upL(){}

void Pen::upR(){}

void Pen::updateHover(){
    const QPointF worldp = worldP();
    const QPointF target = t();

    resetHover();
    for(const auto &alias : app->grid.visibleAliases){
        if(alias->hover(worldp, app->grid.zoom())){
            _hoveredObject = alias;
            _hoverCategory = _NODE;
            setTarget(alias->p());
            return;
        }
    }
    for(const auto &dipole : app->grid.visibleDipoles){
        if(dipole->hover(worldp, app->grid.zoom())){
            const QPointF p = pLA(dipole->line(), worldp);
            _hoveredObject = dipole;
            _hoverCategory = _DIPOLE;
            const auto &A = dipole->A();
            if(A && A->hover(p, app->grid.zoom())){
                setTarget(A->p());
                _hoveredObject = A;
                _hoverCategory = _NODE;
                return;
            }
            const auto &B = dipole->B();
            if(B && B->hover(p, app->grid.zoom())){
                setTarget(B->p());
                _hoveredObject = B;
                _hoverCategory = _NODE;
                return;
            }
            setTarget(p);
            return;
        }
    }
    if(t() == worldp) return;
    for(const auto &alias : app->grid.visibleAliases){
        if(alias->hover(t(), app->grid.zoom())){
            _hoveredObject = alias;
            _hoverCategory = _NODE;
            setTarget(alias->p());
            return;
        }
    }
    for(const auto &dipole : app->grid.visibleDipoles){
        if(dipole->hover(target, app->grid.zoom())){
            const QPointF p = pLA(dipole->line(), target);
            _hoveredObject = dipole;
            _hoverCategory = _DIPOLE;
            const auto &A = dipole->A();
            if(A && A->hover(p, app->grid.zoom())){
                setTarget(A->p());
                _hoveredObject = A;
                _hoverCategory = _NODE;
                return;
            }
            const auto &B = dipole->B();
            if(B && B->hover(p, app->grid.zoom())){
                setTarget(B->p());
                _hoveredObject = B;
                _hoverCategory = _NODE;
                return;
            }
            setTarget(p);
            return;
        }
    }
}

void Pen::updateMovement(){
    QVector<QPointF> points;
    _indicators.clear();
    switch(_state){
    case PEN:
        setTarget(app->grid.snap(worldP()));
        updateHover();

        for(const auto &alias : app->grid.visibleAliases)
            points.append(alias->p());

        setTarget(indicateTarget(app, _indicators, t(), points, app->grid.zoom()));
        break;

    case CONSTRUCTING:
        setTarget(app->grid.snap(worldP()));
        if(type != WIRE && (_state == CONSTRUCTING || (_state == PEN && shift())))
            setTarget(pA(t(), previous->p()));

        updateHover();

        for(const auto &alias : app->grid.visibleAliases){
            if(alias != previous) points.append(alias->p());
        }

        if(type == WIRE){
            auto cur = _path.first();
            while(cur){
                if(const auto &ctrlAfter = cur->after()){
                    if(ctrlAfter->visible(app->grid, app->grid.zoom()))
                        points.append(ctrlAfter->p());
                }
                if(const auto &ctrlBefore = cur->before()){
                    if(ctrlBefore->visible(app->grid, app->grid.zoom()))
                        points.append(ctrlBefore->p());
                }
                if(cur->visible(app->grid, app->grid.zoom()))
                    points.append(cur->p());
                cur = cur->next();
            }
        }
        setTarget(indicateTarget(app, _indicators, t(), points, app->grid.zoom()));
        break;

    case INITIAL_CONTROLLING:
    case CONTROLLING: {
        setTarget(worldP());
        const auto &last = _path.last();

        if(shift() && last)
            setTarget(pA(t(), last->p()));

        for(const auto &alias : app->grid.visibleAliases)
            points.append(alias->p());

        auto cur = _path.first();
        while(cur){
            if(cur != last){
                if(const auto &ctrlAfter = cur->after()){
                    if(ctrlAfter->visible(app->grid, app->grid.zoom()))
                        points.append(ctrlAfter->p());
                }
                if(const auto &ctrlBefore = cur->before()){
                    if(ctrlBefore->visible(app->grid, app->grid.zoom()))
                        points.append(ctrlBefore->p());
                }
            }
            if(cur->visible(app->grid, app->grid.zoom()))
                points.append(cur->p());
            cur = cur->next();
        }

        setTarget(indicateTarget(app, _indicators, t(), points, app->grid.zoom()));
        setTarget(2*last->p() - indicateTarget(app, _indicators, 2*last->p() - t(), points, app->grid.zoom()));

        break;
    }
    default:
        break;
    }

    analyze();
}

void Pen::setState(const ToolState state){
    if(state == PEN){
        _path.clear();
        previous.reset();
    }
    else if(state == CONSTRUCTING && !previous) return;
    _state = state;
    updateMovement();
    MouseTool::setCursor();
}

void Pen::setCursor(const ToolState state){
    switch(state){
    case PEN:
        app->setCursor(penCursor);
        break;
    case CONSTRUCTING:
        app->setCursor(constructingCursor);
        break;
    case INITIAL_CONTROLLING:
    case CONTROLLING:
        app->setCursor(constructingPlusCursor);
        break;
    default:
        break;
    }
}

void Pen::keyDown([[maybe_unused]] Qt::Key key){
    switch(key){
    case Qt::Key_Space:
        if(_state == PEN || _state == CONSTRUCTING)
            app->setTempMouse(&app->grabber, Qt::Key_Space);
        break;
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
        break;
    case Qt::Key_Escape:
        init();
        break;
    case Qt::Key_V:
        app->setMouse(&app->selector);
        break;
    case Qt::Key_P:
        setType(ALIAS);
        break;
    case Qt::Key_R:
        setType(RESISTOR);
        break;
    case Qt::Key_C:
        setType(CAPACITOR);
        break;
    case Qt::Key_L:
        setType(INDUCTOR);
        break;
    case Qt::Key_B:
        setType(BATTERY);
        break;
    case Qt::Key_E:
        setType(DC_VOLTAGE_GENERATOR);
        break;
    case Qt::Key_I:
        setType(DC_CURRENT_GENERATOR);
        break;
    default:
        break;
    }
}

void Pen::keyPress([[maybe_unused]] Qt::Key key){}

void Pen::keyUp([[maybe_unused]] Qt::Key key){}

void Pen::drawAliasPreview(const QColor &color){
    QBrush b = brush();
    b.setColor(color);
    app->grid.drawAlias(app->grid.toScreen(t()), b, radius(), "");
}


void Pen::drawDipolePreview(const QColor &color){
    QPen p = pen();
    p.setColor(color);
    app->grid.drawDipole(type, app->grid.toScreen(previous->p()), app->grid.toScreen(t()), color);
}

void Pen::drawSplittedPreview(const QColor &color){
    const auto &dipole = static_pointer_cast<Dipole>(hoveredObject());
    QPointF A, B;

    switch(mode){
    case NORMAL_SPLIT:
    case ALIAS_SPLIT:
        A = dipole->p1();
        B = t();
        break;
    case OVER_SPLIT: {
        QPointF resultantB_p = t();
        QPointF splittedA_p = dipole->p1();
        QPointF splittedB_p = dipole->p2();

        if(previous == dipole->A()){
            A = resultantB_p;
            B = splittedB_p;
        }
        else {
            A = splittedA_p;
            B = resultantB_p;
        }
        break;
    }
    default:
        return;
    }

    QPen p = pen();
    p.setColor(color);
    app->grid.drawDipole(dipole->type(), app->grid.toScreen(A), app->grid.toScreen(B), p);
}

void Pen::drawResultantPreview(const QColor &color){
    const auto &dipole = static_pointer_cast<Dipole>(hoveredObject());
    QPointF A, B;

    switch(mode){
    case NORMAL_SPLIT:
    case ALIAS_SPLIT:
        A = t();
        B = dipole->p2();
        break;
    case OVER_SPLIT:
        A = previous->p();
        B = t();
        break;
    default:
        return;
    }

    QPen p = pen();
    p.setColor(color);
    app->grid.drawDipole(dipole->type(), app->grid.toScreen(A), app->grid.toScreen(B), p);
}

void Pen::drawHoveredAlias(const QColor &color){
    QBrush b = brush();
    b.setColor(color);
    app->grid.drawObject(hoveredAlias(), Qt::NoPen, b);
}
void Pen::drawHoveredDipole(const QColor &color){
    QPen p = pen();
    p.setColor(color);
    app->grid.drawObject(hoveredDipole(), p, Qt::NoBrush);
}

void Pen::draw(QPainter *painter){
    switch(mode){
    case ALIAS_ONLY:
        drawAliasPreview(fillColor());
        drawIndicators(painter);
        break;
    case DIPOLE_ONLY:
        drawDipolePreview(strokeColor());
        drawHoveredAlias();
        break;
    case ALIAS_AND_DIPOLE:
        drawDipolePreview(strokeColor());
        drawAliasPreview(fillColor());
        drawIndicators(painter);
        break;
    case ALIAS_SPLIT:
        drawSplittedPreview(Palette::HOVER);
        drawResultantPreview(Palette::HOVER);
        drawAliasPreview(fillColor());
        drawIndicators(painter);
        break;
    case NORMAL_SPLIT:
        drawSplittedPreview(Palette::HOVER);
        drawResultantPreview(Palette::HOVER);
        drawDipolePreview(strokeColor());
        drawAliasPreview(fillColor());
        drawIndicators(painter);
        break;
    case OVER_SPLIT:
        drawSplittedPreview(Palette::HOVER);
        drawDipolePreview(strokeColor());
        drawAliasPreview(fillColor());
        drawIndicators(painter);
        break;
    case SWITCH_PREVIOUS: {
        drawHoveredAlias();
        break;
    }
    case PROHIBITED:
        drawAliasPreview(Palette::CONSTRUCTION_PROHIBITED);
        if(_state == CONSTRUCTING) drawDipolePreview(Palette::CONSTRUCTION_PROHIBITED);
        if(_hoverCategory == _DIPOLE) drawHoveredDipole(Palette::CONSTRUCTION_PROHIBITED);
        break;
    }
}
