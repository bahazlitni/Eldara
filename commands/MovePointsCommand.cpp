#include "MovePointsCommand.h"
#include "utils/Selection.h"
#include "App.h"


MovePointsCommand::MovePointsCommand(
    App *app, const MovementMap &movementMap
):
    Command(app), movementMap(movementMap)
{}

void MovePointsCommand::execute(){
    Command::execute();
    for(auto sharedP : movementMap.keys()){
        const QPointF updatedP = movementMap[sharedP].second;
        sharedP->setX(updatedP.x());
        sharedP->setY(updatedP.y());
    }
}

void MovePointsCommand::undo(){
    Command::undo();
    for(auto sharedP : movementMap.keys()){
        const QPointF initialP = movementMap[sharedP].first;
        sharedP->setX(initialP.x());
        sharedP->setY(initialP.y());
    }
}
