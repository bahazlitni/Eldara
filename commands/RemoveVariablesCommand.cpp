#include "RemoveVariablesCommand.h"
#include "widgets/tabs/VariablesTab.h"
#include "Scene.h"
#include "objects/Alias.h"
#include "objects/Dipole.h"
#include "widgets/MainPanel.h"

RemoveVariablesCommand::RemoveVariablesCommand(VariablesTab *variablesTab, const QVector<QString> &names)
    : Command(variablesTab->mainPanel->scene),
    _variablesTab(variablesTab)
{
    // Cache backup for each impacted variable.
    for (const QString &name : names) {
        VariableBackup vb;
        vb.name = name;
        vb.value = variablesTab->value(name);
        vb.type = variablesTab->vartype(name);

        QSet<SharedDipole> visited;
        // Scan through each alias and dipole to collect dipoles and their associated params.
        for (const auto &alias : scene->aliases) {
            for (const auto &dipole : alias->connections()) {
                if (visited.contains(dipole)) continue;
                visited.insert(dipole);

                QVector<Param> matchedParams = dipole->varnameParams(name);
                if (!matchedParams.isEmpty()) {
                    vb.dipoles.append(dipole);
                    vb.params.append(matchedParams);
                }
            }
        }
        _backup.append(vb);
    }
}

void RemoveVariablesCommand::execute(){
    Command::execute();
    // Extract names from our backup for removal.
    QVector<QString> names;
    for (const auto &vb : _backup) {
        names.append(vb.name);
    }

    // Clear the variable name in every related dipole's parameter.
    for (const auto &vb : _backup) {
        for (int j = 0; j < vb.dipoles.size(); ++j) {
            for (const Param &param : vb.params[j]) {
                vb.dipoles[j]->setParamVarname(param, "");
            }
        }
    }

    _variablesTab->removeVariables(names);
}

void RemoveVariablesCommand::undo(){
    Command::undo();
    // Reconstruct names, values, and types to restore the variables.
    QVector<QString> names;
    QVector<QVariant> values;
    QVector<VariableType> types;
    for (const auto &vb : _backup) {
        names.append(vb.name);
        values.append(vb.value);
        types.append(vb.type);
    }

    // Restore each dipole's parameter with the original name and value.
    for (const auto &vb : _backup) {
        for (int j = 0; j < vb.dipoles.size(); ++j) {
            for (const Param &param : vb.params[j]) {
                vb.dipoles[j]->setParamVarname(param, vb.name);
                vb.dipoles[j]->setParamValue(param, vb.value.toDouble());
            }
        }
    }

    _variablesTab->addVariables(names, values, types);
}
