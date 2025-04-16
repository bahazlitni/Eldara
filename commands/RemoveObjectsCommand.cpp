#include "RemoveObjectsCommand.h"
#include "objects/Alias.h"
#include "objects/Dipole.h"
#include "Grid.h"
#include "App.h"

void RemoveObjectsCommand::execute(){
    Command::execute();
    QSet<SharedDipole> visitedDipoles;
    for(const auto &obj : _selection){
        app->deepRemoval(obj);
        if(obj->category() == _NODE){
            const auto &alias = std::static_pointer_cast<Alias>(obj);
            for(const auto &dipole : alias->connections()){
                if(visitedDipoles.contains(dipole)) continue;
                app->deepRemoval(dipole);
                visitedDipoles.insert(dipole);
                const auto &other = dipole->other(alias);
                if(!other || other->id() == alias->id()) continue;
                other->disconnect(dipole);
            }
        } else if( obj->category() == _DIPOLE ){
            const auto &dipole = std::static_pointer_cast<Dipole>(obj);
            if(visitedDipoles.contains(dipole)) continue;
            visitedDipoles.insert(dipole);
            if(const auto &A = dipole->A()) A->disconnect(dipole);
            if(const auto &B = dipole->B()) B->disconnect(dipole);
        }
    }

    if(app->mouse->state() == app->selector.state())
        app->selector.unselect(_selection);
}
void RemoveObjectsCommand::undo(){
    Command::undo();
    QSet<SharedDipole> visitedDipoles;
    for(const auto &obj : _selection){
        if(obj->category() == _NODE){
            const auto &alias = std::static_pointer_cast<Alias>(obj);
            app->addAlias(alias);
            for(const auto &dipole : alias->connections()){
                if(visitedDipoles.contains(dipole)) continue;
                visitedDipoles.insert(dipole);
                const auto &other = dipole->other(alias);
                if(!other || other->id() == alias->id()) continue;
                other->connect(dipole);
                dipole->dirtyVisibleCheckFlag = app->grid.getDirtyVisibleCheckFlagInitial();
            }
        } else if( obj->category() == _DIPOLE ){
            const auto &dipole = std::static_pointer_cast<Dipole>(obj);
            if(visitedDipoles.contains(dipole)) continue;
            visitedDipoles.insert(dipole);
            if(const auto &A = dipole->A()) A->connect(dipole);
            if(const auto &B = dipole->B()) B->connect(dipole);
            dipole->dirtyVisibleCheckFlag = app->grid.getDirtyVisibleCheckFlagInitial();
        }
    }

    if(app->mouse->state() == app->selector.state())
        app->selector.select(_selection);
}
