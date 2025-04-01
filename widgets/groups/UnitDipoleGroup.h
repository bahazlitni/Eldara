#pragma once
#include <QWidget>
#include "DipoleGroup.h"
#include "utils/Types.h"

class BooleanInput;
class DoubleParameterInput;

class App;
class UnitDipoleGroup: public DipoleGroup {
    Q_OBJECT

private:
    const ObjectType _type;
    const VariableType _vartype;

    DoubleParameterInput *Value;

public:
    UnitDipoleGroup(App *app, const ObjectType type, QWidget *parent = nullptr);
    ObjectType type() const override { return _type; }
    VariableType vartype() const { return _vartype; }
    void updateData() override;

    QStringList varnames() const;

    void apply(const QString &key, const QString &value) override;
    void onEditingFinishedApply(const QString &key, const QString &value) override;

    bool isMixed(const QString &key) const override;
    QString dataString(const QString &key) const override;

public slots:
    void onAddVariables(
        const QVector<QString> &names,
        const QVector<QVariant> &values,
        const QVector<VariableType> &types
    );
    void onChangeVariables(
        const QVector<QString> &names,
        const QVector<QVariant> &newValues,
        [[maybe_unused]] const QVector<VariableType> &types
    );
    void onRemoveVariables(
        const QVector<QString> &names,
        [[maybe_unused]] const QVector<VariableType> &types
    );
};
