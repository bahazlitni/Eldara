#include "MovePointsCommand.h"
#include "Scene.h"

MovePointsCommand::MovePointsCommand(
    Scene *scene, const MovementMap &movementMap, QUndoCommand *parent
):
    QUndoCommand(parent), scene(scene), movementMap(movementMap)
{
    if(movementMap.size() == 1){
        const auto &points = *movementMap.constBegin();
        setText(QObject::tr("Move Position to (%1,%2)").arg(
            QString::number(points.second.x()),
            QString::number(points.second.y())
        ));
    }
    else
        setText(QObject::tr("Move %n Positions", "", movementMap.size()));
}

void MovePointsCommand::redo(){
    for(auto sharedP : movementMap.keys()){
        const QPointF updatedP = movementMap[sharedP].second;
        sharedP->setX(updatedP.x());
        sharedP->setY(updatedP.y());
    }
}

void MovePointsCommand::undo(){
    for(auto sharedP : movementMap.keys()){
        const QPointF initialP = movementMap[sharedP].first;
        sharedP->setX(initialP.x());
        sharedP->setY(initialP.y());
    }
}
