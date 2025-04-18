#pragma once
#include "utils/Globals.h"

#include <QUndoCommand>

class Scene;
class InsertObjectsCommand: public QUndoCommand {
public:
    InsertObjectsCommand(
        Scene *scene,
        const Selection &selection,
        QUndoCommand *parent = nullptr
    );

    void redo() override;
    void undo() override;

private:
    Scene *scene;
    Selection _selection;
};
