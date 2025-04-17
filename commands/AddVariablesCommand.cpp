#include "AddVariablesCommand.h"
#include "widgets/tabs/VariablesTab.h"

AddVariablesCommand::AddVariablesCommand(
    VariablesTab *variablesTab,
    const QVector<QString> &names,
    const QVector<QVariant> &values,
    const QVector<VariableType> &types,
    QUndoCommand *parent
    ) : QUndoCommand(parent),
    m_variablesTab(variablesTab),
    m_names(names),
    m_values(values),
    m_types(types)
{
    if(names.size() == 1)
        setText(QObject::tr("Add Variable '%1'").arg(names[0]));
    else
        setText(QObject::tr("Add %n Variables", "", names.size()));
}

void AddVariablesCommand::redo() {
    m_variablesTab->addVariables(m_names, m_values, m_types);
}

void AddVariablesCommand::undo() {
    m_variablesTab->removeVariables(m_names);
}
