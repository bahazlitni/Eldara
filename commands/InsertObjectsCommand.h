#pragma once
#include "Command.h"
#include "utils/Types.h"

#include <QHash>

class App;
class InsertObjectsCommand: public Command {
public:
    InsertObjectsCommand(App *app, const LockedSelection &selection):
        Command(app), _selection(selection) {}

    void execute() override;
    void undo() override;

private:
    LockedSelection _selection;
};
