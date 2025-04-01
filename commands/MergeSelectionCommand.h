#pragma once
#include "Command.h"
#include "utils/Types.h"

#include <QHash>

class App;
class MergeSelectionCommand: public Command {
public:
    MergeSelectionCommand(App *app, const MergeMap &mergeMap);

    void execute() override;
    void undo() override;

private:
    MergeMap _mergeMap;
    QHash<SharedDipole, QPair<SharedAlias, SharedAlias>> _dipolesConfig;
};
