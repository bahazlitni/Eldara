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
bool Dipole::visible(const QRectF &viewport, [[maybe_unused]] const float zoom){ return LinR(line(), viewport); }
bool Dipole::inside(const QRectF &box, [[maybe_unused]] const float zoom){ return LinR(line(), box); }

int Dipole::x1() const { return _A.expired() ? 0 : A()->x(); }
int Dipole::x2() const { return _B.expired() ? 0 : B()->x(); }
int Dipole::y1() const { return _A.expired() ? 0 : A()->y(); }
int Dipole::y2() const { return _B.expired() ? 0 : B()->y(); }
QPointF Dipole::p2() const { return _B.expired() ? QPointF() : *B(); }
QPointF Dipole::p1() const { return _A.expired() ? QPointF() : *A(); }


// UI
QString Dipole::dataString(const QString &key) const {
    if(key == "id") return _A.expired() || _B.expired() ? "Null" : A()->dataString("id") + ":" + B()->dataString("id");
    if(key == "a") return _A.expired() ? "Null" : A()->dataString("id");
    if(key == "b") return _B.expired() ? "Null" : B()->dataString("id");
    if(key == "x1") return QString::number(x1());
    if(key == "y1") return QString::number(y1());
    if(key == "x2") return QString::number(x2());
    if(key == "y2") return QString::number(y2());
    return Object::dataString(key);
}

void Dipole::setData(const QString &key, const QString &value){
    if(key == "x1"){
        if(!_A.expired()) A()->setX(value.toFloat());
    }
    else if(key == "y1"){
        if(!_A.expired()) A()->setY(value.toFloat());
    }
    else if(key == "x2"){
        if(!_B.expired()) B()->setX(value.toFloat());
    }
    else if(key == "y2"){
        if(!_B.expired()) B()->setY(value.toFloat());
    }
    else Object::setData(key, value);
}
