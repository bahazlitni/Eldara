#include "SplitDipoleCommand.h"
#include "objects\Dipole.h"
#include "objects\Alias.h"
#include "Scene.h"
#include "Grid.h"

SplitDipoleCommand::SplitDipoleCommand(
    Scene *scene,
    const SharedDipole &splitted,
    const SharedDipole &resultant,
    const SharedAlias &splitter,
    QUndoCommand *parent
):
    QUndoCommand(parent),
    scene(scene),
    splitted(splitted),
    resultant(resultant),
    splitter(splitter),
    resultantA(resultant->A()),
    resultantB(resultant->B()),
    splittedA(splitted->A()),
    splittedB(splitted->B())
{
    setText(QObject::tr("Split Dipole {%1}").arg(splitted->id()));
}


void SplitDipoleCommand::redo(){
    splitter->connect(splitted);
    resultantA->connect(resultant);
    resultantB->connect(resultant);
    resultant->setA(resultantA);
    resultant->setB(resultantB);
    resultant->dirtyVisibleCheckFlag = scene->grid.getDirtyVisibleCheckFlagInitial();

    if(resultantB == splitter){
        if(resultantA == splittedA){
            splittedA->disconnect(splitted);
            splitted->setA(resultantB);
        }
        else {
            splittedB->disconnect(splitted);
            splitted->setB(resultantB);
        }
    } else {
        if(resultantB == splittedB){
            splittedB->disconnect(splitted);
            splitted->setB(resultantA);
        }
        else {
            splittedA->disconnect(splitted);
            splitted->setA(resultantB);
        }
    }
}

void SplitDipoleCommand::undo(){
    splitter->disconnect(splitted);
    resultantA->disconnect(resultant);
    resultantB->disconnect(resultant);
    splitted->setA(splittedA);
    splitted->setB(splittedB);
    splittedA->connect(splitted);
    splittedB->connect(splitted);

    scene->deepRemoval(resultant);
}
