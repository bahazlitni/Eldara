#pragma once
#include "WorldPoint.h"
#include "utils/Geometry.h"

#pragma once
#include "WorldPoint.h"
#include "utils/Geometry.h"
#include <memory>

class BCControlPoint;
class BCPoint: public WorldPoint {
private:
    std::shared_ptr<BCControlPoint> _before;
    std::shared_ptr<BCControlPoint> _after;
    std::shared_ptr<BCPoint> _next = nullptr;
    std::weak_ptr<BCPoint> _prev;

public:
    BCPoint(float x, float y,
            const std::shared_ptr<BCControlPoint> &before,
            const std::shared_ptr<BCControlPoint> &after):
        WorldPoint(x, y), _before(before), _after(after) {}
    BCPoint(const QPointF &p,
            const std::shared_ptr<BCControlPoint> &before,
            const std::shared_ptr<BCControlPoint> &after):
        WorldPoint(p), _before(before), _after(after) {}
    BCPoint(const SharedPoint &p,
            const std::shared_ptr<BCControlPoint> &before,
            const std::shared_ptr<BCControlPoint> &after):
        WorldPoint(p), _before(before), _after(after) {}

    ~BCPoint() = default;

    ObjectType type() const override { return ObjectType::BCP; }
    ObjectCategory category() const override { return ObjectCategory::Edit; }

    std::shared_ptr<BCControlPoint> before() const { return _before; }
    std::shared_ptr<BCControlPoint> after() const { return _after; }
    std::shared_ptr<BCPoint> next() const { return _next; }
    std::shared_ptr<BCPoint> prev() const { return _prev.lock(); }

    void setBefore(const std::shared_ptr<BCControlPoint> &before){ _before = before; }
    void setAfter(const std::shared_ptr<BCControlPoint> &after){ _after = after; }
    void setNext(const std::shared_ptr<BCPoint> &next){ _next = next; }
    void setPrev(const std::shared_ptr<BCPoint> &prev){ _prev = prev; }

    bool hover(const QPointF &p, [[maybe_unused]] const float zoom) override {
        return PinC(p, this->p(), HALF_BC_EDIT_CONTROL_POINT_WIDTH/zoom);
    }
};
