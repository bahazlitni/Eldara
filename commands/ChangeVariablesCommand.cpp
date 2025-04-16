#include "ChangeVariablesCommand.h"
#include "App.h"
#include "widgets/tabs/VariablesTab.h"
#include "objects/Alias.h"
#include "objects/Dipole.h"
#include "widgets/MainPanel.h"

ChangeVariablesCommand::ChangeVariablesCommand(
    VariablesTab *variablesTab,
    const QVector<QString> &names,
    const QVector<QVariant> &newValues
):
    Command(variablesTab->mainPanel->app), _variablesTab(variablesTab)
{
    // Cache backup for each impacted variable.
    for (int i = 0; i < names.size(); ++i) {
        VariableBackup vb;
        vb.name = names[i];
        vb.originalValue = variablesTab->value(vb.name);
        vb.currentValue = newValues[i];

        QSet<SharedDipole> visited;
        // Scan through each alias and dipole to collect dipoles and their associated params.
        for (const auto &alias : app->aliases) {
            for (const auto &dipole : alias->connections()) {
                if (visited.contains(dipole)) continue;
                visited.insert(dipole);

                QVector<Param> matchedParams = dipole->varnameParams(vb.name);
                if (!matchedParams.isEmpty()) {
                    vb.dipoles.append(dipole);
                    vb.params.append(matchedParams);
                }
            }
        }
        _backup.append(vb);
    }
}

void ChangeVariablesCommand::execute(){
    Command::execute();

}

void ChangeVariablesCommand::undo(){
    Command::undo();

}
