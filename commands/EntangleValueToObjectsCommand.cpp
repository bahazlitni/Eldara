#include "EntangleValueToObjectsCommand.h"
#include "objects/Object.h"
#include "objects/UnitDipole.h"
#include "App.h"
#include "VariablesManager.h"

EntangleValueToObjectsCommand::EntangleValueToObjectsCommand(App *app, const LockedSelection &selection, const QString &name, const VariableType type):
    Command(app), _selection(selection), _name(name), _type(type) {

    // Filter out non unit dipoles
    for(const auto &obj : selection){
        if(obj->category() != _DIPOLE) continue;
        const auto &unitDipole = std::dynamic_pointer_cast<UnitDipole>(obj);
        if(unitDipole) _cache.append(qMakePair(unitDipole, unitDipole->share()));
    }
}


void EntangleValueToObjectsCommand::execute(){
    Command::execute();
    app->varManager.attachDoubleToObjects(_name, _selection);
}

void EntangleValueToObjectsCommand::undo(){
    Command::undo();
    for(const auto &cached : _cache) cached.first->setValue(cached.second);
}
