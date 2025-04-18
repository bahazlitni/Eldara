#include "InsertObjectsCommand.h"
#include "utils/Selection.h"

#include "objects/Alias.h"
#include "objects/Dipole.h"

#include "Scene.h"

InsertObjectsCommand::InsertObjectsCommand(
    Scene *scene,
    const Selection &selection,
    QUndoCommand *parent
):
    QUndoCommand(parent), scene(scene), _selection(selection)
{
    if(selection.size() == 1)
        setText(QObject::tr("Insert '%1'").arg((*selection.constBegin())->name()));
    else
        setText(QObject::tr("Insert %n Objects", "", selection.size()));
}

void InsertObjectsCommand::redo(){
    for(const auto &obj : _selection){
        switch(obj->category()){
        case ObjectCategory::Node: {
            const SharedAlias alias = static_pointer_cast<Alias>(obj);
            scene->aliases.insert(alias->id(), alias);
            break;
        }
        case ObjectCategory::Dipole: {
            const SharedDipole dipole = static_pointer_cast<Dipole>(obj);
            if(auto A = dipole->A()) A->connect(dipole);
            if(auto B = dipole->B()) B->connect(dipole);
            dipole->dirtyVisibleCheckFlag = scene->grid.getDirtyVisibleCheckFlagInitial();
            break;
        }
        default:
            break;
        }
    }

    if(scene->mouse->state() == scene->selector.state())
        scene->selector.select(_selection);
}

void InsertObjectsCommand::undo(){
    for(const auto &obj : _selection){
        if(obj->category() == ObjectCategory::Dipole){
            const SharedDipole dipole = static_pointer_cast<Dipole>(obj);
            const SharedAlias A = dipole->A();
            const SharedAlias B = dipole->B();
            if(auto A = dipole->A()) A->disconnect(dipole);
            if(auto B = dipole->B()) B->disconnect(dipole);
        }
        scene->deepRemoval(obj);
    }

    if(scene->mouse->state() == scene->selector.state())
        scene->selector.unselect(_selection);
}
