#pragma once
#include "Command.h"
#include "utils/Globals.h"

class VariablesTab;
class ChangeVariablesCommand: public Command {
public:
    ChangeVariablesCommand(
        VariablesTab *variablesTab,
        const QVector<QString> &names,
        const QVector<QVariant> &newValues
    );

    void execute() override;
    void undo() override;

private:
    const VariablesTab *_variablesTab;

    struct VariableBackup {
        QString name;
        QVariant originalValue;
        QVariant currentValue;
        QVector<SharedDipole> dipoles;
        QVector<QVector<Param>> params;
    };

    QVector<VariableBackup> _backup;
};
