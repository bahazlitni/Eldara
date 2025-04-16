#include "BCPath.h"
#include "BCPoint.h"
#include "BCControlPoint.h"


// Sceneend a new point at the tail (stack push)
void BCPath::stack(const std::shared_ptr<BCPoint>& bcp) {
    if (!bcp) return;
    // For safety, ensure the new node's pointers are cleared.
    bcp->setNext(nullptr);
    bcp->setPrev(nullptr);

    if (!_first) { // Empty list
        _first = bcp;
        _last = bcp;
    } else {
        // Link the new node after _last.
        bcp->setPrev(_last);
        _last->setNext(bcp);
        _last = bcp;
    }
    ++_size;
}

// Remove the last point (stack pop)
std::shared_ptr<BCPoint> BCPath::unstack() {
    if (!_last) return nullptr; // Empty list

    auto removed = _last;
    if (_first == _last) { // Only one element.
        _first = nullptr;
        _last = nullptr;
    } else {
        // Set the new tail.
        auto newLast = _last->prev();
        newLast->setNext(nullptr);
        _last = newLast;
    }
    --_size;
    // Detach the removed node.
    removed->setPrev(nullptr);
    return removed;
}

// Insert a new point at the head (queue push)
void BCPath::queue(const std::shared_ptr<BCPoint>& bcp) {
    if (!bcp) return;
    bcp->setNext(nullptr);
    bcp->setPrev(nullptr);

    if (!_first) { // Empty list.
        _first = bcp;
        _last = bcp;
    } else {
        bcp->setNext(_first);
        _first->setPrev(bcp);
        _first = bcp;
    }
    ++_size;
}

// Remove and return the first point (queue pop)
std::shared_ptr<BCPoint> BCPath::dequeue() {
    if (!_first) return nullptr; // Empty list

    auto removed = _first;
    if (_first == _last) { // Only one element.
        _first = nullptr;
        _last = nullptr;
    } else {
        auto newFirst = _first->next();
        newFirst->setPrev(nullptr);
        _first = newFirst;
    }
    --_size;
    removed->setNext(nullptr);
    return removed;
}

// Insert a point at an arbitrary position (0-indexed).
// pos == 0: same as queue()
// pos == _size: same as stack()
void BCPath::insert(const std::shared_ptr<BCPoint>& bcp, int pos) {
    if (!bcp || pos < 0 || pos > _size) return;  // Out-of-range position

    if (pos == 0) {
        queue(bcp);
        return;
    }
    if (pos == _size) {
        stack(bcp);
        return;
    }
    // Traverse to the node at position pos-1.
    auto current = _first;
    for (int i = 1; i < pos; ++i) {
        current = current->next();
    }
    // current is the node after which bcp is inserted.
    auto nextNode = current->next();
    bcp->setNext(nextNode);
    bcp->setPrev(current);
    current->setNext(bcp);
    if (nextNode) {
        nextNode->setPrev(bcp);
    }
    ++_size;
}



// Clear the entire list, disconnecting each node.
void BCPath::clear() {
    auto current = _first;
    while (current) {
        auto next = current->next();
        current->setNext(nullptr);
        current->setPrev(nullptr);
        current = next;
    }
    _first = nullptr;
    _last = nullptr;
    _size = 0;
}



// Remove the point at position pos (0-indexed) from the path.
// Does not delete the pointer.
void BCPath::remove(int pos) {
    if (pos < 0 || pos >= _size)
        return; // Out-of-range; could also throw an exception.

    // Traverse to the node at position pos.
    auto current = _first;
    for (int i = 0; i < pos; ++i) {
        current = current->next();
    }

    // Update the previous node (if any).
    if (auto prevNode = current->prev()) {
        prevNode->setNext(current->next());
    } else {
        // current is the first node.
        _first = current->next();
    }

    // Update the next node (if any).
    if (auto nextNode = current->next()) {
        nextNode->setPrev(current->prev());
    } else {
        // current is the last node.
        _last = current->prev();
    }

    // Disconnect the current node from the chain.
    current->setNext(nullptr);
    current->setPrev(nullptr);
    --_size;
}

// Remove the given BCPoint from the path (if found).
// This overload searches the list for the pointer and then removes it.
void BCPath::remove(const std::shared_ptr<BCPoint>& bcp) {
    if (!bcp)
        return;

    auto current = _first;
    int pos = 0;
    while (current) {
        if (current == bcp) {
            remove(pos);
            break;
        }
        current = current->next();
        ++pos;
    }
}




// Translate all points (BCPoints and their ControlPoints) by the given delta.
void BCPath::translate(const QPointF &dp){
    auto current = _first;
    while (current) {
        current->translate(dp);
        if (auto ctrlBefore = current->before())
            ctrlBefore->translate(dp);

        if (auto ctrlAfter = current->after())
            ctrlAfter->translate(dp);

        current = current->next();
    }
}


BCPath BCPath::clone() const {
    BCPath copy;
    auto cur = _first;
    while (cur) {
        // Instead of using cur->p(), we use cur->share() to copy the shared pointer.
        std::shared_ptr<BCControlPoint> newBefore = nullptr;
        std::shared_ptr<BCControlPoint> newAfter = nullptr;

        if (cur->before())
            newBefore = std::make_shared<BCControlPoint>(cur->before()->share());

        if (cur->after())
            newAfter = std::make_shared<BCControlPoint>(cur->after()->share());

        auto newPoint = std::make_shared<BCPoint>(cur->share(), newBefore, newAfter);
        copy.stack(newPoint);
        cur = cur->next();
    }
    return copy;
}
