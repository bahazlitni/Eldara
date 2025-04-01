#pragma once
#include "Dipole.h"
#include <QLineF>
#include "utils/Types.h"

class UnitDipole: public Dipole {
protected:
    SharedDouble _value;
    bool _showLabel;

public:
    UnitDipole(
        const SharedAlias &A,
        const SharedAlias &B,
        const QColor &color,
        const SharedDouble &value,
        const bool showLabel
    ):
        Dipole(A, B, color),
        _showLabel(showLabel)
    {
        setValue(value);
    }

    void setShowLabel(const bool b) { _showLabel = b; }
    void setValue(const SharedDouble &value){
        _value = value? value : std::make_shared<double>(0.0);
    }

    bool showLabel() const { return _showLabel; }
    double value() const { return *_value; }

    SharedDouble share() const { return _value; }
    void unshare(){
        _value = std::make_shared<double>(*_value);
    }

    QString dataString(const QString &key) const override {
        if(key == "value") return QString::number(value());
        else if(key == "show-label") return showLabel()? "1" : "0";
        return Dipole::dataString(key);
    };
    void setData(const QString &key, const QString &value) override {
        if(key == "value") setValue(std::make_shared<double>(value.toDouble()));
        else if(key == "show-label") setShowLabel((bool) value.toInt());
        else Dipole::setData(key, value);
    }
};
