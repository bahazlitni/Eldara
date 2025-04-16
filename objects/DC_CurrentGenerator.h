#pragma once
#include "Dipole.h"
#include "utils/Globals.h"

class Node;
class DC_CurrentGenerator: public Dipole {
protected:
    double _I;
    QString _nameI;

public:
    DC_CurrentGenerator(
        const SharedAlias &A,
        const SharedAlias &B,
        const QColor &color,
        const bool showLabel,
        const double I
    ): Dipole(A, B, QPen(color), showLabel), _I(I) {}

    DC_CurrentGenerator(
        const SharedAlias &A,
        const SharedAlias &B,
        const QPen &pen,
        const bool showLabel,
        const double I
    ): Dipole(A, B, pen, showLabel), _I(I) {}

    inline double paramValue(const Param param) const override {
        return param == Param::I ? _I : 0.0;
    }
    inline QVector<Param> varnameParams(const QString &name) const override {
        return name == _nameI ? QVector{Param::I} : QVector<Param>();
    }
    inline QString paramVarname(const Param param) const override {
        return param == Param::I ? _nameI : "";
    }
    inline void setParamValue(const Param param, const double v) override {
        if(param == Param::I) setI(v);
    }
    inline void setParamVarname(const Param param, const QString &name) override {
        if(param == Param::I) _nameI = name;
    }
    inline void setI(const double v) {
        _I = v;
    }
    inline double I() const {
        return _I;
    }

    int boxWidth() const override { return DEFAULT_CIRCLE_DIAMETER; }
    int boxHeight() const override { return boxWidth(); }
    int halfBoxWidth() const override { return DEFAULT_CIRCLE_RADIUS; }
    int halfBoxHeight() const override { return halfBoxWidth(); }

    ObjectType type() const override { return DC_CURRENT_GENERATOR; }

    inline QString label() const override {
        return _nameI.isEmpty() ? formatDouble(_I, 3, AMP) : _nameI;
    }

    bool hover(const QPointF &p, const float zoom) override {
        return (
            PinL(p, line(), STROKE_HOVER_MAGNET/zoom) ||
            PinC(p, center(), halfBoxWidth())
        );
    }

    SharedDipole clone(const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<DC_CurrentGenerator>(A, B, _pen, _showLabel, _I);
    }
};
