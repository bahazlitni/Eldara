#include "Dipole.h"
#include "Alias.h"
#include "utils/Geometry.h"

bool Dipole::hover(const QPointF &p, const float zoom){
    return (
        PinL(p, line(), STROKE_HOVER_MAGNET/zoom) ||
        PinRotatedR(p, center(),
            halfBoxWidth()/zoom,
            halfBoxHeight()/zoom,
            angle()
        )
    );
}
bool Dipole::visible(const QRectF &viewport, [[maybe_unused]] const float zoom){
    return LinR(line(), viewport);
}
bool Dipole::inside(const QRectF &box, [[maybe_unused]] const float zoom){
    return LinR(line(), box);
}

float Dipole::x1() const {
    if(auto A = _A.lock()) return A->x();
    return 0.0f;
}
float Dipole::x2() const {
    if(auto B = _B.lock()) return B->x();
    return 0.0f;
}
float Dipole::y1() const {
    if(auto A = _A.lock()) return A->y();
    return 0.0f;
}
float Dipole::y2() const {
    if(auto B = _B.lock()) return B->y();
    return 0.0f;
}

void Dipole::setX1(const float v) {
    if(auto A = _A.lock()) A->setX(v);
}
void Dipole::setX2(const float v) {
    if(auto B = _B.lock()) B->setX(v);
}
void Dipole::setY1(const float v) {
    if(auto A = _A.lock()) A->setY(v);
}
void Dipole::setY2(const float v) {
    if(auto B = _B.lock()) B->setY(v);
}

QVariant Dipole::getAttr(const Attr attr) const {
    switch (attr) {
    case Attr::ID: return id();
    case Attr::X1: return x1();
    case Attr::X2: return x2();
    case Attr::Y1: return y1();
    case Attr::Y2: return y2();
    case Attr::Length: return length();
    case Attr::Angle: return angle();
    case Attr::Width: return width();
    case Attr::Height: return height();
    case Attr::Size: return size();
    case Attr::Rect: return rect();
    default: return Object::getAttr(attr);
    }
}

void Dipole::setAttr(const Attr attr, const QVariant &v){
    switch (attr) {
    case Attr::X1: setX1(v.toFloat()); return;
    case Attr::X2: setX2(v.toFloat()); return;
    case Attr::Y1: setY1(v.toFloat()); return;
    case Attr::Y2: setY2(v.toFloat()); return;
    default: Object::setAttr(attr, v); return;
    }
}
