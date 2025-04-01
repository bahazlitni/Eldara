#include "Timeline.h"

void Timeline::push(std::unique_ptr<Command> cmd) {
    if (undoDeque.size() >= _capacity) {
        undoDeque.pop_front();
    }
    undoDeque.push_back(std::move(cmd));
    redoDeque.clear();
}

void Timeline::execute(std::unique_ptr<Command> cmd) {
    cmd->execute();
    push(std::move(cmd));
}

void Timeline::undo() {
    if (undoDeque.empty()) return;

    auto cmd = std::move(undoDeque.back());
    undoDeque.pop_back();
    cmd->undo();
    redoDeque.push_back(std::move(cmd));
}

void Timeline::redo() {
    if (redoDeque.empty()) return;

    auto cmd = std::move(redoDeque.back());
    redoDeque.pop_back();

    cmd->execute();

    if (undoDeque.size() >= _capacity)
        undoDeque.pop_front();

    undoDeque.push_back(std::move(cmd));
}

void Timeline::clear() {
    undoDeque.clear();
    redoDeque.clear();
}
