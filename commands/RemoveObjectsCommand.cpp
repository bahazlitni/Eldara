#include "RemoveObjectsCommand.h"
#include "objects/Alias.h"
#include "objects/Dipole.h"
#include "Grid.h"
#include "Scene.h"

RemoveObjectsCommand::RemoveObjectsCommand(
    Scene *scene,
    const Selection &selection,
    QUndoCommand *parent
):
    QUndoCommand(parent),
    scene(scene),
    _selection(selection)
{
    if(selection.size() == 1)
        setText(QObject::tr("Remove '%1'").arg((*selection.constBegin())->name()));
    else
        setText(QObject::tr("Remove %n Objects", "", selection.size()));
}

void RemoveObjectsCommand::redo(){
    QSet<SharedDipole> visitedDipoles;
    for(const auto &obj : _selection){
        scene->deepRemoval(obj);
        if(obj->category() == ObjectCategory::Node){
            const auto &alias = std::static_pointer_cast<Alias>(obj);
            for(const auto &dipole : alias->connections()){
                if(visitedDipoles.contains(dipole)) continue;
                scene->deepRemoval(dipole);
                visitedDipoles.insert(dipole);
                const auto &other = dipole->other(alias);
                if(!other || other->id() == alias->id()) continue;
                other->disconnect(dipole);
            }
        } else if( obj->category() == ObjectCategory::Dipole ){
            const auto &dipole = std::static_pointer_cast<Dipole>(obj);
            if(visitedDipoles.contains(dipole)) continue;
            visitedDipoles.insert(dipole);
            if(const auto &A = dipole->A()) A->disconnect(dipole);
            if(const auto &B = dipole->B()) B->disconnect(dipole);
        }
    }

    if(scene->mouse->state() == scene->selector.state())
        scene->selector.unselect(_selection);
}

void RemoveObjectsCommand::undo(){
    QSet<SharedDipole> visitedDipoles;
    for(const auto &obj : _selection){
        if(obj->category() == ObjectCategory::Node){
            const auto &alias = std::static_pointer_cast<Alias>(obj);
            scene->addAlias(alias);
            for(const auto &dipole : alias->connections()){
                if(visitedDipoles.contains(dipole)) continue;
                visitedDipoles.insert(dipole);
                const auto &other = dipole->other(alias);
                if(!other || other->id() == alias->id()) continue;
                other->connect(dipole);
                dipole->dirtyVisibleCheckFlag = scene->grid.getDirtyVisibleCheckFlagInitial();
            }
        } else if( obj->category() == ObjectCategory::Dipole ){
            const auto &dipole = std::static_pointer_cast<Dipole>(obj);
            if(visitedDipoles.contains(dipole)) continue;
            visitedDipoles.insert(dipole);
            if(const auto &A = dipole->A()) A->connect(dipole);
            if(const auto &B = dipole->B()) B->connect(dipole);
            dipole->dirtyVisibleCheckFlag = scene->grid.getDirtyVisibleCheckFlagInitial();
        }
    }


    if(scene->mouse->state() == scene->selector.state())
        scene->selector.select(_selection);
}
