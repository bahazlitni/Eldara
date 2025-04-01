#pragma once
#include "Object.h"
#include <QLineF>
#include "../utils/Types.h"
#include "utils/Geometry.h"

class Dipole: public Object {
public:
    Dipole(const SharedAlias &A, const SharedAlias &B, const QColor &color):
    _color(color) {
        setA(A);
        setB(B);
    }

    virtual int halfBoxWidth() const { return HALF_DIPOLE_BOX_WIDTH; }
    virtual int halfBoxHeight() const { return HALF_DIPOLE_BOX_HEIGHT; }
    virtual int boxWidth() const { return DIPOLE_BOX_WIDTH; }
    virtual int boxHeight() const { return DIPOLE_BOX_HEIGHT; }

    ObjectCategory category() const override { return _DIPOLE; }

    bool visible(const QRectF &viewport, [[maybe_unused]] const float zoom) override;
    bool hover(const QPointF &p, [[maybe_unused]] const float zoom) override;
    bool inside(const QRectF &box, [[maybe_unused]] const float zoom) override;

    int x1() const;
    int x2() const;
    int y1() const;
    int y2() const;
    int dx() const { return x2() - x1(); };
    int dy() const { return y2() - y1(); };
    QPointF p1() const;
    QPointF p2() const;
    QPointF center() const{ return (p1() + p2())/2.0; }
    QLineF line() const { return QLineF(p1(), p2()); }
    float length() const { return std::sqrt(QPointF::dotProduct(p1(), p2())); }
    float angle() const { return std::atan2(dy(), dx()); }

    WeakAlias a() const { return _A; }
    WeakAlias b() const { return _B; }
    SharedAlias A() const { return _A.lock(); }
    SharedAlias B() const { return _B.lock(); }
    SharedAlias other(const SharedAlias &a) const { return a == A() ? B() : a == B() ? A() : nullptr; }

    void setA(const SharedAlias &A){ _A = A; }
    void setB(const SharedAlias &B){ _B = B; }

    // Visual
    QColor color() const override { return _color; }
    void setColor(const QColor &color) override { _color = color; }

    bool connectedTo(const SharedAlias &alias){
        return (!_A.expired() && A() == alias) || (!_B.expired() && B() == alias);
    }

    // UI
    QString dataString(const QString &key) const override;
    void setData(const QString &key, const QString &value) override;

    bool dirtyVisibleCheckFlag = false;

    virtual SharedDipole clone(const SharedAlias &A, const SharedAlias &B) = 0;



protected:
    WeakAlias _A, _B;
    QColor _color;
};
