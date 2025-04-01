#pragma once
#include "WorldPoint.h"
#include "utils/Geometry.h"

class BCControlPoint: public WorldPoint {
public:
    BCControlPoint(float x, float y): WorldPoint(x, y) {}
    BCControlPoint(const QPointF &p): WorldPoint(p) {}
    BCControlPoint(const SharedPoint &p): WorldPoint(p) {}

    ObjectCategory category() const override { return _EDIT; }
    ObjectType type() const override { return BC_CONTROL_POINT; }

    bool hover(const QPointF &p, [[maybe_unused]] const float zoom) override {
        const float D = BC_EDIT_CONTROL_POINT_DIAGONAL_WIDTH/zoom;
        return PinDia(p, this->p(), D, D);
    }

};
