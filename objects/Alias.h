#pragma once
#include "WorldPoint.h"
#include "../utils/Globals.h"
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
    DipolesSet _connections;
    bool _showLabel;

public:
    // Constructor & Destructor
    Alias(
        uint64_t id,
        const QString &address,
        float x, float y, int r,
        const QBrush &brush,
        const bool showLabel
    ) :
        WorldPoint(x, y, brush),
        _id(id), _address(address),
        _radius(r),
        _showLabel(showLabel)
    {}

    ~Alias() override = default;

    // Operator Overloads
    bool operator==(const Alias &other) const {
        return id() == other.id();
    }

    // Getters and Setters
    uint64_t id() const { return _id; }
    inline QString label([[maybe_unused]] const bool raw) const override {
        return _address;
    }

    int diameter() const { return _radius*2; }
    int radius() const { return _radius; }
    void setRadius(int r) { _radius = r; }

    float width() const { return (float) diameter(); }
    float height() const { return width(); }
    QSizeF size() const { return QSizeF(width(), height()); }
    QRectF rect() const { return QRectF(x() - _radius, y() - _radius, width(), height()); }

    bool showLabel() const override { return _showLabel; }
    void setShowLabel(bool b) override { _showLabel = b; }

    // Interface Methods (type and category)
    ObjectType type() const override { return ObjectType::Alias; }
    ObjectCategory category() const override { return ObjectCategory::Node; }

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
    DipolesSet connections() const { return _connections; }

    // UI Methods (getAttr and setData)
    QVariant getAttr(const Attr attr) const override {
        switch(attr){
        case Attr::ID: return _id;
        case Attr::Address: return _address;
        case Attr::Radius: return _radius;
        case Attr::Diameter: return diameter();
        case Attr::Width: return width();
        case Attr::Height: return height();
        case Attr::Size: return size();
        case Attr::Rect: return rect();
        default: return WorldPoint::getAttr(attr);
        }
    }
    void setAttr(const Attr attr, const QVariant &v) override {
        switch(attr){
        case Attr::Address: setAddress(v.toString()); return;
        case Attr::Radius: setRadius(v.toUInt()); return;
        default: WorldPoint::setAttr(attr, v); return;
        }
    }

    SharedAlias clone(const uint64_t id) const {
        return std::make_shared<Alias>(id, address(), x(), y(), radius(), _brush, showLabel());
    }
};
