#include "SplitDipoleCommand.h"
#include "objects\Dipole.h"
#include "objects\Alias.h"
#include "App.h"
#include "Grid.h"

SplitDipoleCommand::SplitDipoleCommand(
    App *app,
    const SharedDipole &splitted,
    const SharedDipole &resultant,
    const SharedAlias &splitter
):
    Command(app), splitted(splitted), resultant(resultant), splitter(splitter) {
    resultantA = resultant->A();
    resultantB = resultant->B();
    splittedA = splitted->A();
    splittedB = splitted->B();
}

SplitDipoleCommand::~SplitDipoleCommand(){
    app->deepRemoval(resultant);
}


void SplitDipoleCommand::execute(){
    Command::execute();
    splitter->connect(splitted);
    resultantA->connect(resultant);
    resultantB->connect(resultant);
    resultant->setA(resultantA);
    resultant->setB(resultantB);
    resultant->dirtyVisibleCheckFlag = app->grid.getDirtyVisibleCheckFlagInitial();

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
    Command::undo();
    splitter->disconnect(splitted);
    resultantA->disconnect(resultant);
    resultantB->disconnect(resultant);
    splitted->setA(splittedA);
    splitted->setB(splittedB);
    splittedA->connect(splitted);
    splittedB->connect(splitted);

    app->deepRemoval(resultant);
}
