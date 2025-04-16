#include "EmbedParamCommand.h"
#include "objects/Object.h"
#include "objects/Dipole.h"
#include "objects/Alias.h"
#include "Scene.h"

#include "widgets/tabs/VariablesTab.h"
#include "widgets/MainPanel.h"


EmbedParamCommand::EmbedParamCommand(
    VariablesTab *variablesTab,
    const Selection &selection,
    const QString &name,
    const Param param
):
    Command(variablesTab->mainPanel->scene),
    _variablesTab(variablesTab),
    _name(name),
    _param(param)
{
    _cache.reserve(selection.size());
    for(const auto &obj : selection){
        if(obj->category() != ObjectCategory::Dipole) continue;
        const auto &dipole = std::static_pointer_cast<Dipole>(obj);
        _cache.append(Cache{
            dipole,
            dipole->paramValue(_param),
            dipole->paramVarname(_param)
        });
    }
}


void EmbedParamCommand::execute(){
    Command::execute();
    const double v = _variablesTab->value(_name).toDouble();
    for(const auto &cache : _cache){
        cache.dipole->setParamValue(_param, v);
        cache.dipole->setParamVarname(_param, _name);
    }
}

void EmbedParamCommand::undo(){
    Command::undo();
    for(const auto &cache : _cache){
        cache.dipole->setParamValue(_param, cache.value);
        cache.dipole->setParamVarname(_param, cache.name);
    }
}
