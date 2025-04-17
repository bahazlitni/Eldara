#pragma once
#include "Dipole.h"
#include "utils/Globals.h"

class Alias;
class Resistor: public Dipole {
protected:
    double _R;
    QString _nameR;

public:
    Resistor(const uint64_t id,
        const SharedAlias &A,
        const SharedAlias &B,
        const QPen &pen,
        const bool showLabel,
        const double R
    ): Dipole(id, A, B, pen, showLabel), _R(R) {}

    inline double paramValue(const Param param) const override {
        return param == Param::R ? _R : 0.0;
    }
    inline QVector<Param> varnameParams(const QString &name) const override {
        return name == _nameR ? QVector{Param::R} : QVector<Param>();
    }
    inline QString paramVarname(const Param param) const override {
        return param == Param::R ? _nameR : "";
    }
    inline void setParamValue(const Param param, const double v) override {
        if(param == Param::R) setR(v);
    }
    inline void setParamVarname(const Param param, const QString &name) override {
        if(param == Param::R) _nameR = name;
    }
    inline void setR(const double v) {
        _R = v;
    }
    inline double R() const {
        return _R;
    }

    int boxWidth() const override { return RESISTOR_WIDTH; }
    int boxHeight() const override { return RESISTOR_HEIGHT; }
    int halfBoxWidth() const override { return HALF_RESISTOR_WIDTH; }
    int halfBoxHeight() const override { return HALF_RESISTOR_HEIGHT; }

    ObjectType type() const override { return ObjectType::Resistor; }

    inline QString label(const bool raw) const override {
        return _nameR.isEmpty() ? formatDouble(_R, 2, OHM, raw) : _nameR;
    }

    SharedDipole clone(const uint64_t id, const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<Resistor>(id, A, B, _pen, _showLabel, _R);
    }
};


