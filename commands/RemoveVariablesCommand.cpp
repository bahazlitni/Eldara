#include "RemoveVariablesCommand.h"
#include "App.h"
#include "VariablesManager.h"

inline QVector<QVariant> RemoveVariablesCommand::valuesOf(
    const QVector<QString> &names,
    const QVector<VariableType> types
){
    QVector<QVariant> values;
    for(int i = 0; i < names.size(); ++i)
        values.append(app->varManager.varvalue(names[i], types[i]));
    return values;
}

inline QHash<QString, QVector<SharedUnitDipole>> RemoveVariablesCommand::cacheData(
    const QVector<QString> &names
){
    QHash<QString, QVector<SharedUnitDipole>> cache;
    for(int i = 0; i < names.size(); ++i)
        cache.insert(names[i], app->varManager.unitDipoles(names[i]));
    return cache;
}

RemoveVariablesCommand::RemoveVariablesCommand(
    App *app, const QVector<QString> &names, const QVector<VariableType> types
):  Command(app),
    _names(names),
    _values(valuesOf(names, types)),
    _types(types),
    _cache(cacheData(names))
{}

void RemoveVariablesCommand::execute(){
    Command::execute();
    app->varManager.removeVariables(_names, _types);
}

void RemoveVariablesCommand::undo(){
    Command::undo();
    app->varManager.addVariables(_names, _values, _types, _cache);

}
