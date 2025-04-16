#pragma once
#include "utils/Globals.h"

const int DBL_CLICK_GAP_MS = 500;
const int CLICK_GAP_MS = 300;

enum ToolState {
    SELECTOR,
    SELECTING,
    DRAGGING,
    SELECTING_PLUS,
    PEN,
    CONSTRUCTING,
    CONTROLLING,
    INITIAL_CONTROLLING,
    BC_EDIT,
    BC_EDIT_CONTROLLING_POINT,
    BC_EDIT_CONTROLLING_CONTROL_POINT,
    GRABBER,
    GRABBING,
    GRABBER_TMP,
    GRABBING_TMP
};

class Scene;
class MouseTool: public QObject {
    Q_OBJECT

public:
    MouseTool(Scene *scene): scene(scene) {}


    void init(){
        resetHover();
        _indicators.clear();
        setState(initState());
    }

    virtual ToolState initState() const = 0;
    virtual void end(){}
    virtual void move(){}
    virtual void downL(){}
    virtual void downM(){}
    virtual void downR(){}
    virtual void upL(){}
    virtual void upM(){}
    virtual void upR(){}
    virtual void doubleL(){}
    virtual void doubleM(){}
    virtual void doubleR(){}
    virtual void updateMovement(){}
    virtual void keyDown([[maybe_unused]]  Qt::Key key){}
    virtual void keyPress([[maybe_unused]] Qt::Key key){}
    virtual void keyUp([[maybe_unused]] Qt::Key key){}
    virtual void draw([[maybe_unused]] QPainter *painter){}
    virtual void setCursor([[maybe_unused]]  const ToolState state){}

    virtual QVariant getAttr([[maybe_unused]] const Attr attr) const { return ""; }
    virtual void setAttr([[maybe_unused]] const Attr attr, [[maybe_unused]] const QVariant &v){}

    void setCursor(){ setCursor(_state); }
    void drawIndicators(QPainter *painter);

    ToolState state() const { return _state; }

    float x() const;
    float y() const;
    float dx() const;
    float dy() const;
    QPointF p() const;
    QPointF dp() const;

    float tx() const { return _t.x(); }
    float ty() const { return _t.y(); }
    float dtx() const { return _dt.x(); }
    float dty() const { return _dt.y(); }
    QPointF t() const { return _t; }
    QPointF dt() const { return _dt; }

    QPointF worldP() const;
    QPointF clientT() const;


    QPointF pDown(const Qt::MouseButton btn) const;
    QPointF pUp(const Qt::MouseButton btn) const;
    QPointF tDown(const Qt::MouseButton btn) const {
        int i = 0;
        switch(btn){
        case Qt::MiddleButton: i = 1; break;
        case Qt::RightButton: i = 2; break;
        default: break;
        }
        return _record[0][i];
    }
    QPointF tUp(const Qt::MouseButton btn) const {
        int i = 0;
        switch(btn){
        case Qt::MiddleButton: i = 1; break;
        case Qt::RightButton: i = 2; break;
        default: break;
        }
        return _record[1][i];
    }
    void set_tDown(const Qt::MouseButton btn, const QPointF &p){
        int i = 0;
        switch(btn){
        case Qt::MiddleButton: i = 1; break;
        case Qt::RightButton: i = 2; break;
        default: break;
        }
        _record[0][i] = p;
    }
    void set_tUp(const Qt::MouseButton btn, const QPointF &p){
        int i = 0;
        switch(btn){
        case Qt::MiddleButton: i = 1; break;
        case Qt::RightButton: i = 2; break;
        default: break;
        }
        _record[1][i] = p;
    }
    void setDt(const QPointF &dt){
        _dt = dt;
    }

    bool pressed(Qt::Key key) const;
    bool shift() const;
    bool ctrl() const;
    bool alt() const;

    void setTarget(const QPointF &p){ _t.setX(p.x()); _t.setY(p.y()); }

    bool isHovered(const SharedObject &obj) const;
    virtual bool willDraw([[maybe_unused]] const SharedObject &obj) const { return false; };

    SharedObject hoveredObject() const { return _hoveredObject.lock(); };
    ObjectCategory hoverCategory() const { return _hoverCategory; }
    void resetHover(){
        _hoveredObject.reset();
        _hoverCategory = ObjectCategory::Void;
    }

    virtual void deepRemoval(const SharedObject &obj){
        if(obj == hoveredObject()) resetHover();
    }

protected:
    virtual void setState(const ToolState state) = 0;
    QPointF _record[2][3];
    QPointF _t, _dt;

    Scene *scene;
    ToolState _state;
    WeakObject _hoveredObject;
    ObjectCategory _hoverCategory = ObjectCategory::Void;
    QPainterPath _indicators;
};
