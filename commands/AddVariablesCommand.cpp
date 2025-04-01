#include "AddVariablesCommand.h"
#include "App.h"
#include "VariablesManager.h"

void AddVariablesCommand::execute(){
    Command::execute();
    app->varManager.addVariables(_names, _values, _types);
}

void AddVariablesCommand::undo(){
    Command::undo();
    app->varManager.removeVariables(_names, _types);
}
