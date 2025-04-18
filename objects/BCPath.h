#pragma once
#include "utils/Globals.h"

#include <memory>
#include <QPointF>
#include <QDebug>

class BCPoint;
class BCPath {
private:
    int _size = 0;
    std::shared_ptr<BCPoint> _first = nullptr;
    std::shared_ptr<BCPoint> _last = nullptr;

public:
    BCPath() = default;
    ~BCPath() = default;

    std::shared_ptr<BCPoint> first() const { return _first; }
    std::shared_ptr<BCPoint> last() const { return _last; }



    int size() const { return _size; }
    bool isEmpty() const { return _size == 0; }

    void stack(const std::shared_ptr<BCPoint>& bcp);
    std::shared_ptr<BCPoint> unstack();

    void queue(const std::shared_ptr<BCPoint>& bcp);
    std::shared_ptr<BCPoint> dequeue();

    void insert(const std::shared_ptr<BCPoint>& bcp, int pos);

    void remove(const std::shared_ptr<BCPoint>& bcp);
    void remove(int pos);

    void clear();

    void translate(const QPointF &dp);

    BCPath clone() const;
};
