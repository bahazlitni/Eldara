#pragma once
#include "Dipole.h"
#include "utils/Globals.h"

class Node;
class DC_VoltageGenerator: public Dipole {
protected:
    double _V;
    QString _nameV;

public:
    DC_VoltageGenerator(
        const SharedAlias &A,
        const SharedAlias &B,
        const QColor &color,
        const bool showLabel,
        const double V
    ): Dipole(A, B, QPen(color), showLabel), _V(V) {}

    DC_VoltageGenerator(
        const SharedAlias &A,
        const SharedAlias &B,
        const QPen &pen,
        const bool showLabel,
        const double V
    ): Dipole(A, B, pen, showLabel), _V(V) {}

    inline double paramValue(const Param param) const override {
        return param == Param::V ? _V : 0.0;
    }
    inline QVector<Param> varnameParams(const QString &name) const override {
        return name == _nameV ? QVector{Param::V} : QVector<Param>();
    }
    inline QString paramVarname(const Param param) const override {
        return param == Param::V ? _nameV : "";
    }
    inline void setParamValue(const Param param, const double v) override {
        if(param == Param::V) setV(v);
    }
    inline void setParamVarname(const Param param, const QString &name) override {
        if(param == Param::V) _nameV = name;
    }
    inline void setV(const double v) {
        _V = v;
    }
    inline double V() const {
        return _V;
    }

    int boxWidth() const override { return DEFAULT_CIRCLE_DIAMETER; }
    int boxHeight() const override { return boxWidth(); }
    int halfBoxWidth() const override { return DEFAULT_CIRCLE_RADIUS; }
    int halfBoxHeight() const override { return halfBoxWidth(); }

    ObjectType type() const override { return DC_VOLTAGE_GENERATOR; }

    inline QString label() const override {
        return _nameV.isEmpty() ? formatDouble(_V, 2, VOLT) : _nameV;
    }

    bool hover(const QPointF &p, const float zoom) override {
        return (
            PinL(p, line(), STROKE_HOVER_MAGNET/zoom) ||
            PinC(p, center(), halfBoxWidth())
        );
    }

    SharedDipole clone(const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<DC_VoltageGenerator>(A, B, _pen, _showLabel, _V);
    }
};
