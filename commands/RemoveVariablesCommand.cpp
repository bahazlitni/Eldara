#include "RemoveVariablesCommand.h"
#include "widgets/tabs/VariablesTab.h"
#include "Scene.h"
#include "objects/Alias.h"
#include "objects/Dipole.h"
#include "widgets/MainPanel.h"

RemoveVariablesCommand::RemoveVariablesCommand(
    VariablesTab *variablesTab,
    const QVector<QString> &names,
    QUndoCommand *parent
):
    QUndoCommand(parent),
    m_variablesTab(variablesTab)
{
    if(names.size() == 1)
        setText(QObject::tr("Remove Variable '%1'").arg(names[0]));
    else
        setText(QObject::tr("Remove %n Variables", "", names.size()));

    // Cache backup for each impacted variable.
    for (const QString &name : names) {
        const QVariant value = variablesTab->value(name);
        const VariableType type = variablesTab->vartype(name);

        if(varIsDouble(type)){
            m_dbl_names.append(name);
            m_dbl_values.append(value.toDouble());
            m_dbl_types.append(type);
        }
        else if(type == VAR_INT){
            m_int_names.append(name);
            m_int_values.append(value.toInt());
        }
        else if(type == VAR_STRING){
            m_str_names.append(name);
            m_str_values.append(value.toString());
        }
    }

    DipolesSet visitedDipoles;
    for(const auto &alias : m_variablesTab->mainPanel->scene->aliases){
        for(const auto &dipole : alias->connections()){
            if(visitedDipoles.contains(dipole)) continue;
            visitedDipoles.insert(dipole);
            for(const QString &name : names){
                const QVector<Param> params = dipole->varnameParams(name);
                if(params.isEmpty()) continue;
                if(m_dipoles.contains(name))
                    m_dipoles[name].append(qMakePair(dipole, params));
                else
                    m_dipoles.insert(name, QVector{qMakePair(dipole, params)});
            }
        }
    }
}

void RemoveVariablesCommand::redo(){
    const int N_DBL = m_dbl_names.size();
    const int N_INT = m_int_names.size();
    const int N_STR = m_str_names.size();
    const int N_ALL = N_DBL + N_INT + N_STR;

    QVector<QString> names;
    names.reserve(N_ALL);

    for (int i = 0; i < N_DBL; ++i) {
        const QString &name = m_dbl_names[i];

        if(m_dipoles.contains(name)){
            const double value = m_dbl_values[i];
            for(const auto &member : m_dipoles[name]){
                auto &dipole = member.first;
                auto &params = member.second;
                for(const Param p : params){
                    dipole->setParamValue(p, value);
                    dipole->setParamVarname(p, "");
                }
            }
        }

        names.append(name);
    }

    // Currrently There are no objects attached to an int or a string.
    for (int i = 0; i < N_INT; ++i) {
        const QString &name = m_int_names[i];
        names.append(name);
    }

    for (int i = 0; i < N_STR; ++i) {
        const QString &name = m_str_names[i];
        names.append(name);
    }

    m_variablesTab->removeVariables(names);
}

void RemoveVariablesCommand::undo(){
    const int N_DBL = m_dbl_names.size();
    const int N_INT = m_int_names.size();
    const int N_STR = m_str_names.size();
    const int N_ALL = N_DBL + N_INT + N_STR;

    QVector<QString> names;
    QVector<QVariant> values;
    QVector<VariableType> types;
    names.reserve(N_ALL);
    values.reserve(N_ALL);
    types.reserve(N_ALL);

    for (int i = 0; i < N_DBL; ++i) {
        const QString &name     = m_dbl_names[i];
        const double value      = m_dbl_values[i];
        const VariableType type = m_dbl_types[i];

        if(m_dipoles.contains(name)){
            for(const auto &member : m_dipoles[name]){
                auto &dipole = member.first;
                auto &params = member.second;
                for(const Param p : params)
                    dipole->setParamVarname(p, name);
            }
        }

        names.append(name);
        values.append(value);
        types.append(type);
    }

    // Currrently There are no objects attached to an int or a string.
    for (int i = 0; i < N_INT; ++i) {
        const QString &name     = m_int_names[i];
        const int value         = m_int_values[i];
        const VariableType type = VAR_INT;
        names.append(name);
        values.append(value);
        types.append(type);
    }

    for (int i = 0; i < N_STR; ++i) {
        const QString &name     = m_str_names[i];
        const QString &value    = m_str_values[i];
        const VariableType type = VAR_STRING;
        names.append(name);
        values.append(value);
        types.append(type);
    }

    m_variablesTab->addVariables(names, values, types);
}
