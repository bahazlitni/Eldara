#include "Pen.h"
#include "Scene.h"
#include "objects/Object.h"
#include "objects/Alias.h"
#include "objects/BCControlPoint.h"
#include "objects/BCPath.h"
#include "objects/BCPoint.h"
#include "objects/Battery.h"
#include "objects/Capacitor.h"
#include "objects/DCI.h"
#include "objects/DCV.h"
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


Pen::Pen(Scene *scene): MouseTool(scene),
    penCursor(QCursor(QPixmap(":assets/cursor/pen.png"), 0, 0)),
    constructingCursor(QCursor(QPixmap(":assets/cursor/pen_constructing.png"), 0, 0)),
    constructingPlusCursor(QCursor(QPixmap(":assets/cursor/pen_constructing_plus.png"), 0, 0))
{
    setFillColor(QColor(255,255,255));
    setStrokeColor(QColor(222,222,222));
    setStrokeWidth(2);
    setRadius(10);
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
    if(obj && obj->category() == ObjectCategory::Node) {
        const auto &alias = static_pointer_cast<Alias>(obj);
        if(alias == previous) end();
    }
}


void Pen::setType(ObjectType type){
    if(type == this->type) return;
    this->type = type;
    if(type == ObjectType::Alias) setState(PEN);
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
    const uint64_t address = previous && type == ObjectType::Alias && alt() ? previous->address() : scene->address();
    return std::make_shared<Alias>(scene->id(), address, p.x(), p.y(), radius(), brush(), showLabel());
}
SharedDipole Pen::MakeDipole(const SharedAlias &A, const SharedAlias &B, ObjectType type){
    switch(type){
    case ObjectType::Resistor:
        return std::make_shared<Resistor>(scene->id(), A, B, pen(), showLabel(), defaultResistance());

    case ObjectType::Capacitor:
        return std::make_shared<Capacitor>(scene->id(), A, B, pen(), showLabel(), defaultCapacitance(), 0.0);

    case ObjectType::Inductor:
        return std::make_shared<Inductor>(scene->id(), A, B, pen(), showLabel(), defaultInductance(), 0.0);

    case ObjectType::Battery:
        return std::make_shared<Battery>(scene->id(), A, B, pen(), showLabel(), defaultBatteryVoltage());

    case ObjectType::DCV:
        return std::make_shared<DCV>(scene->id(), A, B, pen(), showLabel(), defaultDCVoltage());

    case ObjectType::DCI:
        return std::make_shared<DCI>(scene->id(), A, B, pen(), showLabel(), defaultIntensity());

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
        case ObjectCategory::Dipole: mode = allowSplitting()? ALIAS_SPLIT : PROHIBITED; return;
        case ObjectCategory::Node: mode = SWITCH_PREVIOUS; return;
        case ObjectCategory::Void: mode = ALIAS_ONLY; return;
        default: mode = PROHIBITED; return;
        }
    case CONSTRUCTING: {
        if(_hoverCategory == ObjectCategory::Node && previous == hoveredAlias()){
            mode = SWITCH_PREVIOUS;
            return;
        }
        SharedDipole lsceneingHoveredDipole;
        const QLineF constructionLine = QLineF(previous->p(), t());

        if(_hoverCategory == ObjectCategory::Dipole && hoveredDipole()->connectedTo(previous)){
            mode = allowSplitting()? OVER_SPLIT : PROHIBITED;
            return;
        }

        for(const auto &dipole : scene->grid.visibleDipoles){
            if(T_LlapL(constructionLine, dipole->line()) != QLineF::UnboundedIntersection) continue;
            if(_hoverCategory == ObjectCategory::Dipole && hoveredDipole() == dipole){
                lsceneingHoveredDipole = dipole;
                continue;
            }
            mode = _hoverCategory == ObjectCategory::Node ? SWITCH_PREVIOUS : PROHIBITED;
            return;
        }

        if(lsceneingHoveredDipole){
            mode = allowSplitting() && lsceneingHoveredDipole->connectedTo(previous) ? OVER_SPLIT : PROHIBITED;
            return;
        }

        switch(_hoverCategory){
        case ObjectCategory::Dipole: mode = allowSplitting()? NORMAL_SPLIT : PROHIBITED; return;
        case ObjectCategory::Node: mode = DIPOLE_ONLY; return;
        case ObjectCategory::Void: mode = ALIAS_AND_DIPOLE; return;
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
        scene->execute(std::make_unique<InsertObjectsCommand>(scene, Selection({previous})));
        break;
    case ALIAS_AND_DIPOLE: {
        const auto &newAlias = MakeAlias(t());
        scene->execute(std::make_unique<InsertObjectsCommand>(scene, Selection({newAlias, MakeDipole(previous, newAlias)})));
        previous = newAlias;
        break;
    }
    case DIPOLE_ONLY: {
        const auto &hoveredAlias = static_pointer_cast<Alias>(hoveredObject());
        scene->execute(
            std::make_unique<InsertObjectsCommand>(
                scene, Selection({MakeDipole(previous, hoveredAlias)})
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
            std::make_unique<InsertObjectsCommand>(scene, Selection({splitter, resultant}))
        );
        cmd->addCommand(
            std::make_unique<SplitDipoleCommand>(scene, splitted, resultant, splitter)
        );
        scene->execute(std::move(cmd));
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
            std::make_unique<InsertObjectsCommand>(scene, Selection({splitter, resultant, normal}))
        );
        cmd->addCommand(
            std::make_unique<SplitDipoleCommand>(scene, splitted, resultant, splitter)
        );
        scene->execute(std::move(cmd));
        previous = splitter;
        break;
    }
    case OVER_SPLIT: {
        const auto &splitter = MakeAlias(t());
        const auto &splitted = hoveredDipole();
        const auto &resultant = MakeDipole(previous, splitter);

        auto cmd = std::make_unique<ComboCommand>();
        cmd->addCommand(
            std::make_unique<InsertObjectsCommand>(scene, Selection({splitter, resultant}))
        );
        cmd->addCommand(
            std::make_unique<SplitDipoleCommand>(scene, splitted, resultant, splitter)
        );
        scene->execute(std::move(cmd));
        previous = splitter;
        break;
    }
    case SWITCH_PREVIOUS:
        previous = static_pointer_cast<Alias>(hoveredObject());
        if(allowOnClickColoring()) previous->setBrush(brush());
        break;
    }
}

void Pen::downL(){
    analyze();
    construct();
    switch(_state){
    case PEN:
        if(type != ObjectType::Alias && previous)
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
    for(const auto &alias : scene->grid.visibleAliases){
        if(alias->hover(worldp, scene->grid.zoom())){
            _hoveredObject = alias;
            _hoverCategory = ObjectCategory::Node;
            setTarget(alias->p());
            return;
        }
    }
    for(const auto &dipole : scene->grid.visibleDipoles){
        if(dipole->hover(worldp, scene->grid.zoom())){
            const QPointF p = pLA(dipole->line(), worldp);
            _hoveredObject = dipole;
            _hoverCategory = ObjectCategory::Dipole;
            const auto &A = dipole->A();
            if(A && A->hover(p, scene->grid.zoom())){
                setTarget(A->p());
                _hoveredObject = A;
                _hoverCategory = ObjectCategory::Node;
                return;
            }
            const auto &B = dipole->B();
            if(B && B->hover(p, scene->grid.zoom())){
                setTarget(B->p());
                _hoveredObject = B;
                _hoverCategory = ObjectCategory::Node;
                return;
            }
            setTarget(p);
            return;
        }
    }
    if(t() == worldp) return;
    for(const auto &alias : scene->grid.visibleAliases){
        if(alias->hover(t(), scene->grid.zoom())){
            _hoveredObject = alias;
            _hoverCategory = ObjectCategory::Node;
            setTarget(alias->p());
            return;
        }
    }
    for(const auto &dipole : scene->grid.visibleDipoles){
        if(dipole->hover(target, scene->grid.zoom())){
            const QPointF p = pLA(dipole->line(), target);
            _hoveredObject = dipole;
            _hoverCategory = ObjectCategory::Dipole;
            const auto &A = dipole->A();
            if(A && A->hover(p, scene->grid.zoom())){
                setTarget(A->p());
                _hoveredObject = A;
                _hoverCategory = ObjectCategory::Node;
                return;
            }
            const auto &B = dipole->B();
            if(B && B->hover(p, scene->grid.zoom())){
                setTarget(B->p());
                _hoveredObject = B;
                _hoverCategory = ObjectCategory::Node;
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
        if(scene->snapPosition())
            setTarget(scene->grid.snap(worldP()));
        else
            setTarget(worldP());

        updateHover();

        if(!scene->snapPosition()) break;

        for(const auto &alias : scene->grid.visibleAliases)
            points.append(alias->p());

        setTarget(indicateTarget(scene, _indicators, t(), points, scene->grid.zoom()));
        break;

    case CONSTRUCTING:
        setTarget(worldP());
        if(scene->snapPosition())
            setTarget(scene->grid.snap(t()));

        if(type != ObjectType::Wire && (_state == CONSTRUCTING || (_state == PEN && shift())))
            setTarget(pA(t(), previous->p()));

        updateHover();

        if(!scene->snapPosition()) break;

        for(const auto &alias : scene->grid.visibleAliases){
            if(alias != previous) points.append(alias->p());
        }

        if(type == ObjectType::Wire){
            auto cur = _path.first();
            while(cur){
                if(const auto &ctrlAfter = cur->after()){
                    if(ctrlAfter->visible(scene->grid, scene->grid.zoom()))
                        points.append(ctrlAfter->p());
                }
                if(const auto &ctrlBefore = cur->before()){
                    if(ctrlBefore->visible(scene->grid, scene->grid.zoom()))
                        points.append(ctrlBefore->p());
                }
                if(cur->visible(scene->grid, scene->grid.zoom()))
                    points.append(cur->p());
                cur = cur->next();
            }
        }
        setTarget(indicateTarget(scene, _indicators, t(), points, scene->grid.zoom()));
        break;

    case INITIAL_CONTROLLING:
    case CONTROLLING: {
        setTarget(worldP());
        const auto &last = _path.last();

        if(shift() && last)
            setTarget(pA(t(), last->p()));


        if(!scene->snapPosition()) break;

        for(const auto &alias : scene->grid.visibleAliases)
            points.append(alias->p());

        auto cur = _path.first();
        while(cur){
            if(cur != last){
                if(const auto &ctrlAfter = cur->after()){
                    if(ctrlAfter->visible(scene->grid, scene->grid.zoom()))
                        points.append(ctrlAfter->p());
                }
                if(const auto &ctrlBefore = cur->before()){
                    if(ctrlBefore->visible(scene->grid, scene->grid.zoom()))
                        points.append(ctrlBefore->p());
                }
            }
            if(cur->visible(scene->grid, scene->grid.zoom()))
                points.append(cur->p());
            cur = cur->next();
        }

        setTarget(indicateTarget(scene, _indicators, t(), points, scene->grid.zoom()));
        setTarget(2*last->p() - indicateTarget(scene, _indicators, 2*last->p() - t(), points, scene->grid.zoom()));

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
        scene->setCursor(penCursor);
        break;
    case CONSTRUCTING:
        scene->setCursor(constructingCursor);
        break;
    case INITIAL_CONTROLLING:
    case CONTROLLING:
        scene->setCursor(constructingPlusCursor);
        break;
    default:
        break;
    }
}

void Pen::keyDown([[maybe_unused]] Qt::Key key){
    switch(key){
    case Qt::Key_Space:
        if(_state == PEN || _state == CONSTRUCTING)
            scene->setTempMouse(&scene->grabber, Qt::Key_Space);
        break;
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
        break;
    case Qt::Key_Escape:
        init();
        break;
    case Qt::Key_V:
        scene->setMouse(&scene->selector);
        break;
    case Qt::Key_P:
        setType(ObjectType::Alias);
        break;
    case Qt::Key_R:
        setType(ObjectType::Resistor);
        break;
    case Qt::Key_C:
        setType(ObjectType::Capacitor);
        break;
    case Qt::Key_L:
        setType(ObjectType::Inductor);
        break;
    case Qt::Key_B:
        setType(ObjectType::Battery);
        break;
    case Qt::Key_E:
        setType(ObjectType::DCV);
        break;
    case Qt::Key_I:
        setType(ObjectType::DCI);
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
    scene->grid.drawAlias(scene->grid.toScreen(t()), b, radius(), "");
}


void Pen::drawDipolePreview(const QColor &color){
    QPen p = pen();
    p.setColor(color);
    scene->grid.drawDipole(type, scene->grid.toScreen(previous->p()), scene->grid.toScreen(t()), color);
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
    scene->grid.drawDipole(dipole->type(), scene->grid.toScreen(A), scene->grid.toScreen(B), p);
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
    scene->grid.drawDipole(dipole->type(), scene->grid.toScreen(A), scene->grid.toScreen(B), p);
}

void Pen::drawHoveredAlias(const QColor &color){
    QBrush b = brush();
    if(allowOnClickColoring())
        b.setColor(fillColor());
    else
        b.setColor(color);

    QPen pen;
    pen.setStyle(Qt::PenStyle::DashLine);
    pen.setColor(Palette::HOVER);
    pen.setWidth(2);
    scene->grid.drawObject(hoveredAlias(), pen, b);
}
void Pen::drawHoveredDipole(const QColor &color){
    QPen p = pen();
    p.setColor(color);
    scene->grid.drawObject(hoveredDipole(), p, Qt::NoBrush);
}

void Pen::draw(QPainter *painter){
    switch(mode){
    case ALIAS_ONLY:
        drawAliasPreview(fillColor());
        drawIndicators(painter);
        break;
    case DIPOLE_ONLY:
        drawDipolePreview(strokeColor());
        drawHoveredAlias(Palette::HOVER);
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
        drawHoveredAlias(Palette::HOVER);
        break;
    }
    case PROHIBITED:
        drawAliasPreview(Palette::CONSTRUCTION_PROHIBITED);
        if(_state == CONSTRUCTING) drawDipolePreview(Palette::CONSTRUCTION_PROHIBITED);
        if(_hoverCategory == ObjectCategory::Dipole) drawHoveredDipole(Palette::CONSTRUCTION_PROHIBITED);
        break;
    }
}
