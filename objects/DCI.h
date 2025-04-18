#pragma once
#include "Dipole.h"
#include "utils/Globals.h"

#include <QString>
#include <QVector>

class DCI: public Dipole {
protected:
    double _I;
    QString _nameI;

public:
    DCI(const uint64_t id,
        const SharedAlias &A,
        const SharedAlias &B,
        const QPen &pen,
        const bool showLabel,
        const double I
    ): Dipole(id, A, B, pen, showLabel), _I(I) {}

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

    inline ObjectType type() const override { return ObjectType::DCI; }
    inline DCRole dcRole() const override { return DCRole::I; }

    inline QString label(const bool raw) const override {
        return _nameI.isEmpty() ? formatDouble(_I, 3, AMP, raw) : _nameI;
    }

    bool hover(const QPointF &p, const float zoom) override {
        return (
            PinL(p, line(), STROKE_HOVER_MAGNET/zoom) ||
            PinC(p, center(), halfBoxWidth())
        );
    }

    SharedDipole clone(const uint64_t id, const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<DCI>(id, A, B, _pen, _showLabel, _I);
    }
};
