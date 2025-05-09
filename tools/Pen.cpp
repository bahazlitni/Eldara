#include "Pen.h"
#include "Scene.h"
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

#include "commands/SplitDipoleCommand.h"
#include "commands/InsertObjectsCommand.h"
#include "Grid.h"
#include "tools/Grabber.h"
#include "tools/Selector.h"


Pen::Pen(Scene *scene): MouseTool(scene),
    penCursor(QCursor(QPixmap(":assets/cursor/pen.png"), 0, 0)),
    constructingCursor(QCursor(QPixmap(":assets/cursor/pen_constructing.png"), 0, 0)),
    constructingPlusCursor(QCursor(QPixmap(":assets/cursor/pen_constructing_plus.png"), 0, 0))
{}

QVariant Pen::getAttr(const Attr attr) const {
    switch(attr){
    case Attr::Radius : return radius();
    case Attr::FillColor : return fillColor();
    case Attr::StrokeColor : return strokeColor();
    case Attr::StrokeWidth : return strokeWidth();
    case Attr::AliasOutline : return aliasOutline();
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
    case Attr::AliasOutline : setAliasOutline(v.toUInt()); return;
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
    if(makingAlias()) setState(PEN);
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
    const int64_t address = previous && makingAlias() && alt() ? previous->address() : scene->address(willMakeGround());
    return std::make_shared<Alias>(
        scene->id(),
        address,
        p.x(), p.y(),
        radius(),
        brush(),
        aliasPen(),
        showLabel()
    );
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
        case ObjectCategory::Dipole: mode = allowSplitting()? Mode::AliasSplit : Mode::Prohibited; return;
        case ObjectCategory::Node: mode = Mode::SwitchPrevious; return;
        case ObjectCategory::Void: mode = Mode::AliasOnly; return;
        default: mode = Mode::Prohibited; return;
        }
    case CONSTRUCTING: {
        if(_hoverCategory == ObjectCategory::Node && previous == hoveredAlias()){
            mode = Mode::SwitchPrevious;
            return;
        }
        SharedDipole lsceneingHoveredDipole;
        const QLineF constructionLine = QLineF(previous->p(), t());

        if(_hoverCategory == ObjectCategory::Dipole && hoveredDipole()->connectedTo(previous)){
            mode = allowSplitting()? Mode::OverSplit : Mode::Prohibited;
            return;
        }

        for(const auto &dipole : scene->grid.visibleDipoles){
            if(T_LlapL(constructionLine, dipole->line()) != QLineF::UnboundedIntersection) continue;
            if(_hoverCategory == ObjectCategory::Dipole && hoveredDipole() == dipole){
                lsceneingHoveredDipole = dipole;
                continue;
            }
            mode = _hoverCategory == ObjectCategory::Node ? Mode::SwitchPrevious : Mode::Prohibited;
            return;
        }

        if(lsceneingHoveredDipole){
            mode = allowSplitting() && lsceneingHoveredDipole->connectedTo(previous) ? Mode::OverSplit : Mode::Prohibited;
            return;
        }

        switch(_hoverCategory){
        case ObjectCategory::Dipole: mode = allowSplitting()? Mode::NormalSplit : Mode::Prohibited; return;
        case ObjectCategory::Node: mode = Mode::DipoleOnly; return;
        case ObjectCategory::Void: mode = Mode::AliasAndDipole; return;
        default: mode = Mode::Prohibited; return;
        }
        break;
    }
    default:
        mode = Mode::Prohibited;
        break;
    }
}

void Pen::construct() {
    switch (mode) {
    case Mode::Prohibited:
        return;

    case Mode::AliasOnly: {
        auto a = MakeAlias(t());
        previous = a;

        scene->execute(
            new InsertObjectsCommand(scene, Selection{a})
        );
        break;
    }

    case Mode::AliasAndDipole: {
        auto a = MakeAlias(t());
        auto d = MakeDipole(previous, a);
        previous = a;

        scene->execute(
            new InsertObjectsCommand(scene, Selection{a, d})
        );
        break;
    }

    case Mode::DipoleOnly: {
        auto targetAlias = std::static_pointer_cast<Alias>(hoveredObject());
        auto d = MakeDipole(previous, targetAlias);
        previous = targetAlias;

        scene->execute(
            new InsertObjectsCommand(scene, Selection{d})
        );
        break;
    }

    case Mode::AliasSplit: {
        auto splitter  = MakeAlias(t());
        auto splitted  = hoveredDipole();
        auto resultant = MakeDipole(splitter, splitted->B(), splitted->type());
        previous = splitter;

        auto *macro = new QUndoCommand(tr("Alias Split"));

        new InsertObjectsCommand(
            scene, Selection{splitter, resultant}, macro
        );
        new SplitDipoleCommand(
            scene, splitted, resultant, splitter, macro
        );

        scene->execute(macro);
        break;
    }

    case Mode::NormalSplit: {
        auto splitter  = MakeAlias(t());
        auto splitted  = hoveredDipole();
        auto resultant = MakeDipole(splitter, splitted->B(), splitted->type());
        auto normal    = MakeDipole(previous, splitter);
        previous = splitter;

        auto *macro = new QUndoCommand(tr("Normal Split"));

        new InsertObjectsCommand(
            scene, Selection{splitter, resultant, normal}, macro
        );
        new SplitDipoleCommand(
            scene, splitted, resultant, splitter, macro
        );

        scene->execute(macro);
        break;
    }

    case Mode::OverSplit: {
        auto splitter  = MakeAlias(t());
        auto splitted  = hoveredDipole();
        auto resultant = MakeDipole(previous, splitter);
        previous = splitter;

        auto *macro = new QUndoCommand(tr("Over Split"));

        new InsertObjectsCommand(
            scene, Selection{splitter, resultant}, macro
        );
        new SplitDipoleCommand(
            scene, splitted, resultant, splitter, macro
        );

        scene->execute(macro);
        break;
    }

    case Mode::SwitchPrevious:
        previous = std::static_pointer_cast<Alias>(hoveredObject());
        if (allowOnClickColoring())
            previous->setBrush(brush());
        break;
    }
}


void Pen::downL(){
    analyze();
    construct();
    switch(_state){
    case PEN:
        if(!makingAlias() && previous)
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
    case Qt::Key_G:
        setType(ObjectType::Ground);
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
    scene->grid.drawAlias(scene->grid.toScreen(t()), b, radius(), willMakeGround());
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
    case Mode::NormalSplit:
    case Mode::AliasSplit:
        A = dipole->p1();
        B = t();
        break;
    case Mode::OverSplit: {
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
    case Mode::NormalSplit:
    case Mode::AliasSplit:
        A = t();
        B = dipole->p2();
        break;
    case Mode::OverSplit:
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
    case Mode::AliasOnly:
        drawAliasPreview(fillColor());
        drawIndicators(painter);
        break;
    case Mode::DipoleOnly:
        drawDipolePreview(strokeColor());
        drawHoveredAlias(Palette::HOVER);
        break;
    case Mode::AliasAndDipole:
        drawDipolePreview(strokeColor());
        drawAliasPreview(fillColor());
        drawIndicators(painter);
        break;
    case Mode::AliasSplit:
        drawSplittedPreview(Palette::HOVER);
        drawResultantPreview(Palette::HOVER);
        drawAliasPreview(fillColor());
        drawIndicators(painter);
        break;
    case Mode::NormalSplit:
        drawSplittedPreview(Palette::HOVER);
        drawResultantPreview(Palette::HOVER);
        drawDipolePreview(strokeColor());
        drawAliasPreview(fillColor());
        drawIndicators(painter);
        break;
    case Mode::OverSplit:
        drawSplittedPreview(Palette::HOVER);
        drawDipolePreview(strokeColor());
        drawAliasPreview(fillColor());
        drawIndicators(painter);
        break;
    case Mode::SwitchPrevious: {
        drawHoveredAlias(Palette::HOVER);
        break;
    }
    case Mode::Prohibited:
        drawAliasPreview(Palette::CONSTRUCTION_PROHIBITED);
        if(_state == CONSTRUCTING)
            drawDipolePreview(Palette::CONSTRUCTION_PROHIBITED);
        if(_hoverCategory == ObjectCategory::Dipole)
            drawHoveredDipole(Palette::CONSTRUCTION_PROHIBITED);
        break;
    }
}
