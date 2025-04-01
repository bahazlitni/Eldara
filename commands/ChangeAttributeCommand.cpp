#include "ChangeAttributeCommand.h"
#include "App.h"
#include "objects/Object.h"

ChangeAttributeCommand::ChangeAttributeCommand(App *app, const LockedSelection &selection, const QString key, const QString value):
    Command(app), _selection(selection), key(key), value(value) {
    for(const auto &obj : _selection) oldValues[obj] = obj->dataString(key);
}

void ChangeAttributeCommand::execute(){
    Command::execute();
    for(const auto &obj : _selection) obj->setData(key, value);
}

void ChangeAttributeCommand::undo(){
    Command::undo();
    for(const auto &obj : _selection) obj->setData(key, oldValues[obj]);
}
