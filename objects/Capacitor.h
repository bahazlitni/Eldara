#pragma once
#include "Dipole.h"
#include "utils/Globals.h"

#include <QString>
#include <QVector>

class Capacitor: public Dipole {
protected:
    double _C, _Q0;
    QString _nameC, _nameQ0;

public:
    Capacitor(const uint64_t id,
        const SharedAlias &A,
        const SharedAlias &B,
        const QPen &pen,
        const bool showLabel,
        const double C,
        const double Q0 = 0.0
    ): Dipole(id, A, B, pen, showLabel), _C(C), _Q0(Q0) {}

    inline double paramValue(const Param param) const override {
        return param == Param::C ? _C : param == Param::Q0 ? _Q0 : 0.0;
    }
    inline QVector<Param> varnameParams(const QString &name) const override {
        return name == _nameC ? QVector{Param::C}
            : name == _nameQ0 ? QVector{Param::Q0}
            : QVector<Param>();
    }
    inline QString paramVarname(const Param param) const override {
        return param == Param::C ? _nameC : param == Param::Q0 ? _nameQ0 : "";
    }
    inline void setParamValue(const Param param, const double v) override {
        if(param == Param::C) setC(v);
        if(param == Param::Q0) setQ0(v);
    }
    inline void setParamVarname(const Param param, const QString &name) override {
        if(param == Param::C) _nameC = name;
        if(param == Param::Q0) _nameQ0 = name;
    }
    inline void setC(const double v) {
        _C = v;
    }
    inline double C() const {
        return _C;
    }
    inline void setQ0(const double v) {
        _Q0 = v;
    }
    inline double Q0() const {
        return _Q0;
    }

    int boxWidth() const override { return CAPACITOR_WIDTH; }
    int boxHeight() const override { return CAPACITOR_HEIGHT; }
    int halfBoxWidth() const override { return HALF_CAPACITOR_WIDTH; }
    int halfBoxHeight() const override { return HALF_CAPACITOR_HEIGHT; }

    inline ObjectType type() const override { return ObjectType::Capacitor; }
    inline DCRole dcRole() const override { return DCRole::R; }

    inline QString label(const bool raw) const override {
        return _nameC.isEmpty() ? formatDouble(_C, 3, FARAD, raw) : _nameC;
    }

    SharedDipole clone(const uint64_t id, const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<Capacitor>(id, A, B, _pen, _showLabel, _C, _Q0);
    }
};
