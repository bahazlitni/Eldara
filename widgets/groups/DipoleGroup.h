#pragma once
#include <QWidget>
#include "ObjectGroup.h"
#include "utils/Globals.h"

class App;
class CoordinateInput;
class IDLabel;
class AliasGroup;
class DoubleParameterInput;

class DipoleGroup: public ObjectGroup {
    Q_OBJECT

protected:
    const ObjectType _type;

    IDLabel *ID;
    QVector<DoubleParameterInput*> DoubleInputs;

    void addParameter(const Param param, const QString &label);

public:
    DipoleGroup(MainPanel *mainPanel, const ObjectType type, QWidget *parent = nullptr);
    ObjectCategory category() const override { return _DIPOLE; }
    ObjectType type() const override { return _type; }

    void updateData() override;


    bool isMixedParam(const Param param) const override;
    double paramValue(const Param param) const override;
    QString paramVarname(const Param param) const override;

    void setParamValue(const Param param, const double value) override;
    void confirmParamValue(const Param param, const double value) override;

    void setParamVarname(const Param param, const QString &varname) override;
    void confirmParamVarname(const Param param, const QString &varname) override;

public slots:
    void onAddVariables(
        const QVector<QString> &names,
        const QVector<QVariant> &values,
        const QVector<VariableType> &types
    );
    void onChangeVariables(
        const QVector<QString> &names,
        const QVector<QVariant> &newValues
    );
    void onRemoveVariables(
        const QVector<QString> &names
    );
};
