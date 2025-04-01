#pragma once
#include "WorldPoint.h"
#include "../utils/Types.h"
#include "utils/Geometry.h"
#include <QRegularExpression>
#include <cstdint>

class Dipole;

class Alias : public WorldPoint {
private:
    // Member variables
    const uint64_t _id;
    QString _address;
    int _radius;
    QColor _color;
    LockedDipoles _connections;
    bool _showLabel;

public:
    // Constructor & Destructor
    Alias(
        uint64_t id,
        const QString &address,
        float x, float y, int r,
        const QColor &color,
        const bool showLabel
    ) :
        WorldPoint(QPointF(x, y)),
        _id(id), _address(address),
        _radius(r), _color(color),
        _showLabel(showLabel) {}

    ~Alias() override = default;

    // Operator Overloads
    bool operator==(const Alias &other) const {
        return id() == other.id();
    }

    // Getters and Setters
    uint64_t id() const { return _id; }
    QString name() const override { return _address; }
    QColor color() const override { return _color; }
    void setColor(const QColor &color) override { _color = color; }

    int radius() const { return _radius; }
    void setRadius(int r) { _radius = r; }

    bool showLabel() const { return _showLabel; }
    void setShowLabel(bool b) { _showLabel = b; }

    // Interface Methods (type and category)
    ObjectType type() const override { return ALIAS; }
    ObjectCategory category() const override { return _NODE; }

    // Geometric methods
    bool inside(const QRectF &box, [[maybe_unused]] float zoom) override {
        return CinR(p(), static_cast<float>(_radius) / zoom, box);
    }

    bool visible(const QRectF &viewport, [[maybe_unused]] float zoom) override {
        return inside(viewport, zoom);
    }

    bool hover(const QPointF &p, [[maybe_unused]] float zoom) override {
        return PinC(p, this->p(), static_cast<float>(_radius) / zoom);
    }

    // Address-related Methods
    void setAddress(QString address) { _address = address; }
    QString address() const { return _address; }

    // Connection Methods
    void connect(const SharedDipole &dipole) { _connections.insert(dipole); }
    void disconnect(const SharedDipole &dipole) { _connections.remove(dipole); }
    LockedDipoles connections() const { return _connections; }

    // UI Methods (dataString and setData)
    QString dataString(const QString &key) const override {
        if (key == "id") return QString::number(_id);
        if (key == "address") return _address;
        if (key == "radius") return QString::number(_radius);
        if (key == "show-label") return _showLabel? "1" : "0";
        return WorldPoint::dataString(key);
    }

    void setData(const QString &key, const QString &value) override {
        if (key == "address") setAddress(value);
        else if (key == "radius") setRadius(value.toInt());
        else if (key == "show-label") setShowLabel(value == "1");
        else WorldPoint::setData(key, value);
    }

    SharedAlias clone(const uint64_t id) const {
        return std::make_shared<Alias>(id, address(), x(), y(), radius(), color(), showLabel());
    }
};
