#include "AddVariablesCommand.h"
#include "Scene.h"
#include "widgets/tabs/VariablesTab.h"
#include "widgets/MainPanel.h"

AddVariablesCommand::AddVariablesCommand(
    VariablesTab *variablesTab,
    const QVector<QString> &names,
    const QVector<QVariant> values,
    const QVector<VariableType> &types
):
    Command(variablesTab->mainPanel->scene),
    _variablesTab(variablesTab),
    _names(names),
    _values(values),
    _types(types)
{}


void AddVariablesCommand::execute(){
    Command::execute();
    _variablesTab->addVariables(_names, _values, _types);
}

void AddVariablesCommand::undo(){
    Command::undo();
    _variablesTab->removeVariables(_names);
}
