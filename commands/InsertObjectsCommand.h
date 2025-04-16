#pragma once
#include "Command.h"
#include "utils/Globals.h"

#include <QHash>

class Scene;
class InsertObjectsCommand: public Command {
public:
    InsertObjectsCommand(Scene *scene, const Selection &selection):
        Command(scene), _selection(selection) {}

    void execute() override;
    void undo() override;

private:
    Selection _selection;
};
