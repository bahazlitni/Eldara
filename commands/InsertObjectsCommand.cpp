#include "InsertObjectsCommand.h"
#include "utils/Selection.h"
#include "App.h"

#include "objects/Object.h"
#include "objects/Alias.h"
#include "objects/Dipole.h"

void InsertObjectsCommand::execute(){
    Command::execute();
    for(const auto &obj : _selection){
        switch(obj->category()){
        case _NODE: {
            const SharedAlias alias = static_pointer_cast<Alias>(obj);
            app->aliases.insert(alias->id(), alias);
            break;
        }
        case _DIPOLE: {
            const SharedDipole dipole = static_pointer_cast<Dipole>(obj);
            if(auto A = dipole->A()) A->connect(dipole);
            if(auto B = dipole->B()) B->connect(dipole);
            dipole->dirtyVisibleCheckFlag = app->grid.getDirtyVisibleCheckFlagInitial();
            break;
        }
        default:
            break;
        }
    }

    if(app->mouse->state() == app->selector.state())
        app->selector.select(_selection);
}

void InsertObjectsCommand::undo(){
    Command::undo();
    for(const auto &obj : _selection){
        if(obj->category() == _DIPOLE){
            const SharedDipole dipole = static_pointer_cast<Dipole>(obj);
            const SharedAlias A = dipole->A();
            const SharedAlias B = dipole->B();
            if(auto A = dipole->A()) A->disconnect(dipole);
            if(auto B = dipole->B()) B->disconnect(dipole);
        }
        app->deepRemoval(obj);
    }

    if(app->mouse->state() == app->selector.state())
        app->selector.unselect(_selection);
}
