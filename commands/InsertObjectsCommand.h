#pragma once
#include "Command.h"
#include "utils/Globals.h"

#include <QHash>

class App;
class InsertObjectsCommand: public Command {
public:
    InsertObjectsCommand(App *app, const Selection &selection):
        Command(app), _selection(selection) {}

    void execute() override;
    void undo() override;

private:
    Selection _selection;
};
