#pragma once
#include "Command.h"
#include "utils/Globals.h"

class VariablesTab;

class RemoveVariablesCommand : public Command {
public:
    RemoveVariablesCommand(VariablesTab *variablesTab, const QVector<QString> &names);

    void execute() override;
    void undo() override;

private:
    // Struct to encapsulate all backup data for a variable.
    struct VariableBackup {
        QString name;
        QVariant value;
        VariableType type;
        QVector<SharedDipole> dipoles;
        QVector<QVector<Param>> params;
    };

    VariablesTab *_variablesTab;
    // Backup information for each variable being removed.
    QVector<VariableBackup> _backup;
};
