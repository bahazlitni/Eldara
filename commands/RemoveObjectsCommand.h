#pragma once
#include "Command.h"
#include "utils/Globals.h"

class App;
class RemoveObjectsCommand: public Command {
public:
    RemoveObjectsCommand(App *app, const Selection &selection):
        Command(app), _selection(selection) {}

    void execute() override;
    void undo() override;

private:
    Selection _selection;
};
