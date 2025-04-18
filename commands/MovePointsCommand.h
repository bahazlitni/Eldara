#pragma once
#include "utils/Globals.h"

#include <QUndoCommand>

class Scene;

class MovePointsCommand: public QUndoCommand {
public:
    explicit MovePointsCommand(
        Scene *scene, const MovementMap &movementMap, QUndoCommand *parent = nullptr
    );

    void redo() override;
    void undo() override;

private:
    Scene *scene;
    const MovementMap movementMap;
};
