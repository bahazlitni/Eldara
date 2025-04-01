#pragma once
#include "Object.h"
#include <memory>
#include "../utils/Types.h"
#include "utils/Geometry.h"

class App;
class WorldPoint: public Object {
private:
    SharedPoint point;

public:
    WorldPoint(float x, float y): point(std::make_shared<QPointF>(QPointF(x, y))) {}
    WorldPoint(QPointF p): point(std::make_shared<QPointF>(p)) {}
    WorldPoint(const SharedPoint &p): point(p) {}

    bool inside(const QRectF &box, [[maybe_unused]] const float zoom) override { return PinR(p(), box); }
    bool visible(const QRectF &viewport, [[maybe_unused]] const float zoom) override { return PinR(p(), viewport); }

    // Implicit conversion to QPointF
    operator QPointF() const { return *point; }
    operator SharedPoint() const { return point; }

    // Getters
    float x() const { return point->x(); }
    float y() const { return point->y(); }
    QPointF p() const { return QPointF(x(), y()); }
    SharedPoint share() const { return point; }

    // Setters
    void setX(float x) { point->setX(x); }
    void setY(float y) { point->setY(y); }
    void setShared(const SharedPoint &p){ point = p; }

    // Transformations
    void translateX(float dx) { setX(x() + dx); }
    void translateY(float dy) { setY(y() + dy); }
    void translate(float dx, float dy) { translateX(dx); translateY(dy); }
    void translate(const QPointF& delta) { translate(delta.x(), delta.y()); }


    // UI
    QString dataString(const QString &key) const override {
        if(key == "x") return QString::number(x());
        if(key == "y") return QString::number(y());
        return Object::dataString(key);
    }

    void setData(const QString &key, const QString &value) override {
        if(key == "x") setX(value.toFloat());
        else if(key == "y") setY(value.toFloat());
        else Object::setData(key, value);
    }
};
