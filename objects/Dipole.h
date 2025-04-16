#pragma once
#include "Object.h"
#include <QLineF>
#include "utils/Globals.h"
#include "utils/Geometry.h"

class Dipole: public Object {
protected:
    WeakAlias _A, _B;
    bool _showLabel;

public:
    Dipole(const SharedAlias &A, const SharedAlias &B, const QBrush &brush, const QPen &pen, const bool showLabel):
        Object(brush, pen), _showLabel(showLabel) {
        setA(A);
        setB(B);
    }
    Dipole(const SharedAlias &A, const SharedAlias &B, const QBrush &brush, const bool showLabel):
        Object(brush), _showLabel(showLabel) {
        setA(A);
        setB(B);
    }
    Dipole(const SharedAlias &A, const SharedAlias &B, const QPen &pen, const bool showLabel):
        Object(pen), _showLabel(showLabel) {
        setA(A);
        setB(B);
    }
    Dipole(const SharedAlias &A, const SharedAlias &B, const bool showLabel):
        Object(), _showLabel(showLabel) {
        setA(A);
        setB(B);
    }

    ObjectCategory category() const override { return ObjectCategory::Dipole; }

    inline QPair<int, int> paramMagnitudeRange(const Param param) {
        switch (param) {
        case Param::R:
        case Param::R0:
            return qMakePair(-3, 9);
        case Param::C:
        case Param::C0:
        case Param::L:
        case Param::L0:
            return qMakePair(-12, 3);
        case Param::V:
        case Param::V0:
        case Param::I:
        case Param::I0:
        case Param::Q:
        case Param::Q0:
            return qMakePair(-9, 3);

        default:
            return qMakePair(0, 0);
        }
    }

    inline void setShowLabel(const bool b) override { _showLabel = b; }
    inline bool showLabel() const override { return _showLabel; }

    inline virtual double paramValue([[maybe_unused]] const Param param) const { return 0.0; }
    inline virtual QVector<Param> varnameParams([[maybe_unused]] const QString &name) const {
        return QVector<Param>();
    }
    inline virtual QString paramVarname([[maybe_unused]] const Param param) const { return ""; }
    inline virtual void setParamValue(
        [[maybe_unused]] const Param param,
        [[maybe_unused]] const double v
    ){}
    inline virtual void setParamVarname(
        [[maybe_unused]] const Param param,
        [[maybe_unused]] const QString &name
    ){}


    QVariant getAttr(const Attr attr) const override;
    void setAttr(const Attr attr, const QVariant &v) override;


    // Geometry
    virtual int halfBoxWidth() const { return HALF_DIPOLE_BOX_WIDTH; }
    virtual int halfBoxHeight() const { return HALF_DIPOLE_BOX_HEIGHT; }
    virtual int boxWidth() const { return DIPOLE_BOX_WIDTH; }
    virtual int boxHeight() const { return DIPOLE_BOX_HEIGHT; }

    bool visible(const QRectF &viewport, [[maybe_unused]] const float zoom) override;
    bool hover(const QPointF &p, [[maybe_unused]] const float zoom) override;
    bool inside(const QRectF &box, [[maybe_unused]] const float zoom) override;

    float x1() const;
    float x2() const;
    float y1() const;
    float y2() const;

    void setX1(const float v);
    void setX2(const float v);
    void setY1(const float v);
    void setY2(const float v);

    inline float dx() const { return x2() - x1(); };
    inline float dy() const { return y2() - y1(); };
    QPointF p1() const;
    QPointF p2() const;
    inline QPointF center() const{ return (p1() + p2())/2.0; }
    inline QLineF line() const { return QLineF(p1(), p2()); }
    inline float length() const {
        const float dx = this->dx(), dy = this->dy();
        return std::sqrt(dx*dx + dy*dy);
    }
    inline float angle() const { return std::atan2(dy(), dx()); }
    inline float width() const { return qAbs(dx()); }
    inline float height() const { return qAbs(dy()); }
    inline QSizeF size() const { return QSizeF(width(), height()); }
    inline QRectF rect() const {
        return QRectF(qMin(x1(), x2()), qMin(y1(), y2()), width(), height());
    }

    inline void setA(const SharedAlias &A){ _A = A; }
    inline void setB(const SharedAlias &B){ _B = B; }
    inline SharedAlias A() const { return _A.lock(); }
    inline SharedAlias B() const { return _B.lock(); }
    inline SharedAlias other(const SharedAlias &a) const {
        return a == A() ? B() : a == B() ? A() : nullptr;
    }

    // Visual
    inline bool connectedTo(const SharedAlias &alias){
        return (!_A.expired() && A() == alias) || (!_B.expired() && B() == alias);
    }

    bool dirtyVisibleCheckFlag = false;

    virtual SharedDipole clone(const SharedAlias &A, const SharedAlias &B) = 0;

};
