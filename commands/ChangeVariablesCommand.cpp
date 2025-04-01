#include "ChangeVariablesCommand.h"
#include "App.h"
#include "VariablesManager.h"

inline QVector<QVariant> valuesOf(App *app, const QVector<QString> &names, const QVector<VariableType> types){
    QVector<QVariant> values;
    for(int i = 0; i < names.size(); ++i)
        values.append(app->varManager.varvalue(names[i], types[i]));
    return values;
}

ChangeVariablesCommand::ChangeVariablesCommand(
    App *app,
    const QVector<QString> &names,
    const QVector<QVariant> newValues,
    const QVector<VariableType> types
):
    Command(app), _names(names),
    _oldValues(valuesOf(app, names, types)),
    _newValues(newValues),
    _types(types)
{}

void ChangeVariablesCommand::execute(){
    Command::execute();
    app->varManager.changeVariables(_names, _newValues, _types);
}

void ChangeVariablesCommand::undo(){
    Command::undo();
    app->varManager.addVariables(_names, _oldValues, _types);
}
