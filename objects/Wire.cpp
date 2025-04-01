#include "Wire.h"
#include "utils/Geometry.h"
#include "BCPoint.h"
#include "BCPath.h"
#include "BCControlPoint.h"
#include "WorldPoint.h"
#include "Alias.h"
#include "Grid.h"

bool Wire::hover(const QPointF &p, const float zoom) {
    const float lw = STROKE_HOVER_MAGNET/zoom;
    for (auto cur = _path.first(); cur;) {
        auto next = cur->next();
        if (!next) break;
        auto ctrlAfter = cur->after();
        if (!ctrlAfter) break;
        auto ctrlBefore = next->before();
        if (!ctrlBefore) break;
        if (PinBC(p, cur->p(), ctrlAfter->p(), ctrlBefore->p(), next->p(), lw))
            return true;
        cur = next;
    }
    return false;
}

bool Wire::inside(const QRectF &box, [[maybe_unused]] const float zoom) {
    for (auto cur = _path.first(); cur;) {
        auto next = cur->next();
        if (!next) break;
        auto ctrlAfter = cur->after();
        if (!ctrlAfter) break;
        auto ctrlBefore = next->before();
        if (!ctrlBefore) break;
        if (BCinR(cur->p(), ctrlAfter->p(), ctrlBefore->p(), next->p(), box))
            return true;
        cur = next;
    }
    return false;
}

void Wire::setA(const SharedAlias &A){ _A = A; if(_path.first()) _path.first()->setShared(A->share()); }
void Wire::setB(const SharedAlias &B){ _B = B; if(_path.last()) _path.last()->setShared(B->share()); }



SharedDipole Wire::clone(const SharedAlias &A, const SharedAlias &B){
    return std::make_shared<Wire>(A, B, color(), _path.clone());
}




