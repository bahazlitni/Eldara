#pragma once
#include "Object.h"
#include <memory>
#include "../utils/Globals.h"
#include "utils/Geometry.h"

class Scene;
class WorldPoint: public Object {
private:
    SharedPoint point;

public:
    WorldPoint(float x, float y): point(std::make_shared<QPointF>(QPointF(x, y))) {}
    WorldPoint(QPointF p): point(std::make_shared<QPointF>(p)) {}
    WorldPoint(const SharedPoint &p): point(p) {}

    WorldPoint(float x, float y, const QBrush &brush, const QPen &pen): Object(brush, pen), point(std::make_shared<QPointF>(QPointF(x, y))) {}
    WorldPoint(QPointF p, const QBrush &brush, const QPen &pen): Object(brush, pen), point(std::make_shared<QPointF>(p)) {}
    WorldPoint(const SharedPoint &p, const QBrush &brush, const QPen &pen): Object(brush, pen), point(p) {}

    WorldPoint(float x, float y, const QBrush &brush): Object(brush), point(std::make_shared<QPointF>(QPointF(x, y))) {}
    WorldPoint(QPointF p, const QBrush &brush): Object(brush), point(std::make_shared<QPointF>(p)) {}
    WorldPoint(const SharedPoint &p, const QBrush &brush): Object(brush), point(p) {}

    WorldPoint(float x, float y, const QPen &pen): Object(pen), point(std::make_shared<QPointF>(QPointF(x, y))) {}
    WorldPoint(QPointF p, const QPen &pen): Object(pen), point(std::make_shared<QPointF>(p)) {}
    WorldPoint(const SharedPoint &p, const QPen &pen): Object(pen), point(p) {}

    bool inside(const QRectF &box, [[maybe_unused]] const float zoom) override { return PinR(p(), box); }
    bool visible(const QRectF &viewport, [[maybe_unused]] const float zoom) override { return PinR(p(), viewport); }

    // Implicit conversion to QPointF
    operator QPointF() const { return *point; }
    operator SharedPoint() const { return point; }

    // Getters
    inline float x() const { return point->x(); }
    inline float y() const { return point->y(); }
    inline QPointF p() const { return QPointF(x(), y()); }
    inline SharedPoint share() const { return point; }

    // Setters
    inline void setX(float x) { point->setX(x); }
    inline void setY(float y) { point->setY(y); }
    inline void setP(const QPointF &p) { setX(p.x()); setY(p.y()); }
    inline void setShared(const SharedPoint &p){ point = p; }

    // Transformations
    inline void translateX(float dx) { setX(x() + dx); }
    inline void translateY(float dy) { setY(y() + dy); }
    inline void translate(float dx, float dy) { translateX(dx); translateY(dy); }
    inline void translate(const QPointF& delta) { translate(delta.x(), delta.y()); }


    // UI
    QVariant getAttr(const Attr attr) const override {
        switch(attr){
        case Attr::X: return x();
        case Attr::Y: return y();
        case Attr::P: return p();
        default: return Object::getAttr(attr);
        }
    }
    void setAttr(const Attr attr, const QVariant &v) override {
        switch(attr){
        case Attr::X: setX(v.toFloat()); return;
        case Attr::Y: setY(v.toFloat()); return;
        case Attr::P: setP(v.toPointF()); return;
        default: Object::setAttr(attr, v); return;
        }
    }
};
