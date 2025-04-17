#pragma once
#include "utils/Globals.h"

class Scene;
class MergeSelectionCommand: public QUndoCommand {
public:
    MergeSelectionCommand(Scene *scene, const MergeMap &mergeMap, QUndoCommand *parent = nullptr);

    void redo() override;
    void undo() override;

private:
    Scene *scene;
    MergeMap _mergeMap;
    QHash<SharedDipole, QPair<SharedAlias, SharedAlias>> _dipolesConfig;
};
