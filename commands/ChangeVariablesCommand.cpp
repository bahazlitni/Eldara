#include "ChangeVariablesCommand.h"
#include "widgets/tabs/VariablesTab.h"
#include "objects/Alias.h"
#include "objects/Dipole.h"
#include "widgets/MainPanel.h"
#include "Scene.h"


ChangeVariablesCommand::ChangeVariablesCommand(
    VariablesTab *variablesTab,
    const QVector<QString> &names,
    const QVector<QVariant> &newValues,
    QUndoCommand *parent
    ) : QUndoCommand(parent),
    m_variablesTab(variablesTab)
{
    if(names.size() == 1)
        setText(QObject::tr("Change Value of '%1' to %2").arg(names[0], newValues[0].toString()));
    else
        setText(QObject::tr("Change %n Variables", "", names.size()));

    // build backup
    for(int i = 0; i < names.size(); ++i) {
        const QString &name = names[i];
        const QVariant oldValue = variablesTab->value(name);
        const QVariant &newValue = newValues[i];
        const VariableType type = variablesTab->vartype(name);

        if(varIsDouble(type)){
            m_dbl_names.append(name);
            m_dbl_oldValues.append(oldValue.toDouble());
            m_dbl_newValues.append(newValue.toDouble());
        }
        else if(type == VAR_INT){
            m_int_names.append(name);
            m_int_oldValues.append(oldValue.toInt());
            m_int_newValues.append(newValue.toInt());
        }
        else if(type == VAR_STRING){
            m_str_names.append(name);
            m_str_oldValues.append(oldValue.toString());
            m_str_newValues.append(newValue.toString());
        }
    }
}

void ChangeVariablesCommand::changeVariables(const bool intoNewValue){
    const int N_DBL = m_dbl_names.size();
    const int N_INT = m_int_names.size();
    const int N_STR = m_str_names.size();
    const int N_ALL = N_DBL + N_INT + N_STR;

    QVector<QString> names;
    QVector<QVariant> values;
    names.reserve(N_ALL);
    values.reserve(N_ALL);

    for (int i = 0; i < N_DBL; ++i) {
        const QString &name = m_dbl_names[i];
        const double value = intoNewValue? m_dbl_newValues[i] : m_dbl_oldValues[i];
        names.append(name);
        values.append(value);
    }

    for (int i = 0; i < N_INT; ++i) {
        const QString &name = m_int_names[i];
        const int value = intoNewValue? m_int_newValues[i] : m_int_oldValues[i];
        names.append(name);
        values.append(value);
    }

    for (int i = 0; i < N_STR; ++i) {
        const QString &name = m_str_names[i];
        const QString &value = intoNewValue? m_str_newValues[i] : m_str_oldValues[i];
        names.append(name);
        values.append(value);
    }

    m_variablesTab->changeVariables(names, values);
}

