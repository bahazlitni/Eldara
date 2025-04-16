#pragma once
#include "Command.h"
#include "utils/Globals.h"

class Scene;
class RemoveObjectsCommand: public Command {
public:
    RemoveObjectsCommand(Scene *scene, const Selection &selection):
        Command(scene), _selection(selection) {}

    void execute() override;
    void undo() override;

private:
    Selection _selection;
};
