#pragma once
#include "Command.h"
#include "utils/Globals.h"

class VariablesTab;
class AddVariablesCommand: public Command {
public:
    AddVariablesCommand(
        VariablesTab *variablesTab,
        const QVector<QString> &names,
        const QVector<QVariant> values,
        const QVector<VariableType> &types
    );

    void execute() override;
    void undo() override;

private:
    VariablesTab *_variablesTab;
    const QVector<QString> _names;
    const QVector<QVariant> _values;
    const QVector<VariableType> _types;
};
