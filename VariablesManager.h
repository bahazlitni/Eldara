#pragma once
#include "utils/Types.h"
#include <QObject>



class App;
class VariablesManager: public QObject {
    Q_OBJECT

private:
    App *app;

    DoubleVarTable doubleTable;
    StringVarTable stringTable;
    IntVarTable intTable;

    void addVariable();
    void removeVariable();
    void changeValue();

    SharedDouble doubleKey(const QString &name) const;
    SharedInt intKey(const QString &name) const;
    SharedString stringKey(const QString &name) const;

public:
    static QString displayDouble(const double value, const VariableType type){
        if(type == VAR_DOUBLE) return QString::number(value, 'd', 2);
        const int order = magnitude(value);
        return QString::number(value/std::pow(10.0, order), 'd', 2) + (
            + " "
            + getOrderSymbol(order)
            + getUnitSymbol(getUnitOfVariableType(type))
        );
    }

    explicit VariablesManager(App *app): app(app) {}

    void detachDoubleFromObjects(const QString &name);
    void detachIntFromObjects(const QString &name);
    void detachStringFromObjects(const QString &name);

    void attachDoubleToObjects(const QString &name, const QSet<SharedObject> &selection);
    void attachIntToObjects(const QString &name, const QSet<SharedObject> &selection);
    void attachStringToObjects(const QString &name, const QSet<SharedObject> &selection);

    QVariant varvalue(const QString &name) const;
    QVariant varvalue(const QString &name, const VariableType type) const;
    QString varname(const SharedDouble &var) const;
    QString varname(const SharedInt &var) const;
    QString varname(const SharedString &var) const;
    VariableType vartype(const SharedDouble &var) const;
    VariableType vartype(const SharedInt &var) const;
    VariableType vartype(const SharedString &var) const;
    VarMetadata varmetadata(const SharedDouble &var) const;
    VarMetadata varmetadata(const SharedInt &var) const;
    VarMetadata varmetadata(const SharedString &var) const;

    QVector<SharedUnitDipole> unitDipoles(const QString &name) const;

    QStringList varnames() const;
    QStringList varnames(const VariableType type) const;


    void changeVariables(
        const QVector<QString> &names,
        const QVector<QVariant> newValues,
        const QVector<VariableType> types
    );
    void removeVariables(
        const QVector<QString> &names,
        const QVector<VariableType> types
    );
    void addVariables(
        const QVector<QString> &names,
        const QVector<QVariant> values,
        const QVector<VariableType> types
    );
    void addVariables(
        const QVector<QString> &names,
        const QVector<QVariant> values,
        const QVector<VariableType> types,
        const QHash<QString, QVector<SharedUnitDipole>> &cache
    );

signals:
    void variablesAdded(
        const QVector<QString> &names,
        const QVector<QVariant> values,
        const QVector<VariableType> types
    );
    void variablesChanged(
        const QVector<QString> &names,
        const QVector<QVariant> newValues,
        const QVector<VariableType> &types
    );
    void variablesRemoved(
        const QVector<QString> &names,
        const QVector<VariableType> &types
    );
};
