#include "EmbedParamCommand.h"
#include "widgets/tabs/VariablesTab.h"
#include "objects/Dipole.h"

EmbedParamCommand::EmbedParamCommand(
    VariablesTab *variablesTab,
    const Selection &selection,
    const QString &name,
    Param param,
    QUndoCommand *parent
    ) : QUndoCommand(parent),
    m_variablesTab(variablesTab),
    m_name(name),
    m_param(param)
{
    setText(QObject::tr("Embed parameter '%1'").arg(name));

    for (auto &obj : selection) {
        if (obj->category() != ObjectCategory::Dipole) continue;
        auto dip = std::static_pointer_cast<Dipole>(obj);
        m_cache.push_back({ dip,
            dip->paramValue(param),
            dip->paramVarname(param)
        });
    }
}

void EmbedParamCommand::redo() {
    double v = m_variablesTab->value(m_name).toDouble();
    for (auto &c : m_cache) {
        c.dipole->setParamValue(m_param, v);
        c.dipole->setParamVarname(m_param, m_name);
    }
}

void EmbedParamCommand::undo() {
    for (auto &c : m_cache) {
        c.dipole->setParamValue(m_param, c.oldValue);
        c.dipole->setParamVarname(m_param, c.oldName);
    }
}
