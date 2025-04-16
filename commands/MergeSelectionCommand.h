#pragma once
#include "Command.h"
#include "utils/Globals.h"

#include <QHash>

class Scene;
class MergeSelectionCommand: public Command {
public:
    MergeSelectionCommand(Scene *scene, const MergeMap &mergeMap);

    void execute() override;
    void undo() override;

private:
    MergeMap _mergeMap;
    QHash<SharedDipole, QPair<SharedAlias, SharedAlias>> _dipolesConfig;
};
