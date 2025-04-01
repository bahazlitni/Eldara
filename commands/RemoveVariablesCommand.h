#pragma once
#include "Command.h"
#include "utils/Types.h"

#include <QSet>


#include <QHash>


class App;
class RemoveVariablesCommand: public Command {
public:
    RemoveVariablesCommand(App *app, const QVector<QString> &names, const QVector<VariableType> types);

    void execute() override;
    void undo() override;

private:
    inline QVector<QVariant> valuesOf(const QVector<QString> &names, const QVector<VariableType> types);
    inline QHash<QString, QVector<SharedUnitDipole>> cacheData(const QVector<QString> &names);

    const QVector<QString> _names;
    const QVector<QVariant> _values;
    const QVector<VariableType> _types;
    const QHash<QString, QVector<SharedUnitDipole>> _cache;
};
