#pragma once
#include "Dipole.h"
#include "utils/Globals.h"

class Inductor: public Dipole {
protected:
    double _L, _I0;
    QString _nameL, _nameI0;

public:
    Inductor(const uint64_t id,
        const SharedAlias &A,
        const SharedAlias &B,
        const QPen &pen,
        const bool showLabel,
        const double L,
        const double I0 = 0.0
    ): Dipole(id, A, B, pen, showLabel), _L(L), _I0(I0) {}

    inline double paramValue(const Param param) const override {
        return param == Param::L ? _L : param == Param::I0 ? _I0 : 0.0;
    }
    inline QVector<Param> varnameParams(const QString &name) const override {
        // L and I0 have different types, therefore it's prohibited to get them at same time.
        return name == _nameL ? QVector{Param::L}
            : name == _nameI0 ? QVector{Param::I0}
            : QVector<Param>();
    }
    inline QString paramVarname(const Param param) const override {
        return param == Param::L ? _nameL : param == Param::I0 ? _nameI0 : "";
    }
    inline void setParamValue(const Param param, const double v) override {
        if(param == Param::L) setL(v);
        if(param == Param::I0) setI0(v);
    }
    inline void setParamVarname(const Param param, const QString &name) override {
        if(param == Param::L) _nameL = name;
        if(param == Param::I0) _nameI0 = name;
    }
    inline void setL(const double v) {
        _L = v;
    }
    inline double L() const {
        return _L;
    }
    inline void setI0(const double v) {
        _I0 = v;
    }
    inline double I0() const {
        return _I0;
    }

    int boxWidth() const override { return INDUCTOR_WIDTH; }
    int boxHeight() const override { return INDUCTOR_HEIGHT; }
    int halfBoxWidth() const override { return HALF_INDUCTOR_WIDTH; }
    int halfBoxHeight() const override { return HALF_INDUCTOR_HEIGHT; }

    ObjectType type() const override { return ObjectType::Inductor; }

    inline QString label(const bool raw) const override {
        return _nameL.isEmpty() ? formatDouble(_L, 2, HENRY, raw) : _nameL;
    }

    SharedDipole clone(const uint64_t id, const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<Inductor>(id, A, B, _pen, _showLabel, _L, _I0);
    }
};
