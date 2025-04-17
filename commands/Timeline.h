#pragma once
#include <deque>
#include <memory>
#include "Command.h"

class Timeline {
public:
    explicit Timeline(std::size_t capacity = 500): _capacity(capacity) {}

    void execute(std::unique_ptr<Command> cmd);
    void push(std::unique_ptr<Command> cmd);
    void undo();
    void redo();
    void clear();

    inline std::size_t capacity() const { return _capacity; }
    inline bool isEmpty() const { return undoDeque.empty(); }

private:
    std::deque<std::unique_ptr<Command>> undoDeque;
    std::deque<std::unique_ptr<Command>> redoDeque;
    std::size_t _capacity;
};
