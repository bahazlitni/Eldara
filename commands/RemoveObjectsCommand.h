#pragma once
#include "Command.h"
#include "utils/Types.h"

class App;
class RemoveObjectsCommand: public Command {
public:
    RemoveObjectsCommand(App *app, const LockedSelection &selection):
        Command(app), _selection(selection) {}

    void execute() override;
    void undo() override;

private:
    LockedSelection _selection;
};
