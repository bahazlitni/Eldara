#pragma once

#include <vector>
#include <memory>

class Scene;
class Command {
private:
    bool _isExecuted = false;
protected:
    Scene *scene;
public:
    Command(Scene *scene): scene(scene) {}
    virtual ~Command() = default;
    virtual void execute(){ _isExecuted = true; }
    virtual void undo(){ _isExecuted = false; }
    bool isExecuted(){ return _isExecuted; }
};


class ComboCommand : public Command {
private:
    std::vector<std::unique_ptr<Command>> commands;

public:
    ComboCommand() : Command(nullptr) {}

    void addCommand(std::unique_ptr<Command> cmd) {
        commands.push_back(std::move(cmd));
    }

    void execute() override {
        Command::execute();
        for (size_t i = 0; i < commands.size(); ++i) commands[i]->execute();
    }

    void undo() override {
        Command::undo();
        for (size_t i = commands.size(); i-- > 0; ) commands[i]->undo();
    }
};
