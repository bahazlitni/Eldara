#pragma once
#include "utils/Globals.h"

#include <QUndoCommand>

class Scene;

class RemoveObjectsCommand: public QUndoCommand {
public:
    RemoveObjectsCommand(Scene *scene, const Selection &selection, QUndoCommand *parent = nullptr);

    void redo() override;
    void undo() override;

private:
    Scene *scene;
    Selection _selection;
};
