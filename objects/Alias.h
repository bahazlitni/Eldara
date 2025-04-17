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
    uint64_t _address;
    QString _label;
    int _radius;
    QColor _color;
    DipolesSet _connections;
    bool _showLabel;

public:
    // Constructor & Destructor
    Alias(const uint64_t id,
        const uint64_t address,
        float x, float y, int r,
        const QBrush &brush,
        const bool showLabel
    ) :
        WorldPoint(x, y, brush),
        _id(id),
        _radius(r),
        _showLabel(showLabel)
    {
        setAddress(address);
    }

    Alias(const uint64_t id,
          const uint64_t address,
          float x, float y, int r,
          const QBrush &brush,
          const QPen &pen,
          const bool showLabel
          ) :
        WorldPoint(x, y, brush, pen),
        _id(id),
        _radius(r),
        _showLabel(showLabel)
    {
        setAddress(address);
    }

    ~Alias() override = default;

    // Operator Overloads
    bool operator==(const Alias &other) const {
        return id() == other.id();
    }

    // Getters and Setters
    inline uint64_t id() const { return _id; }
    inline QString label([[maybe_unused]] const bool raw) const override {
        return _label;
    }

    inline int diameter() const { return _radius*2; }
    inline int radius() const { return _radius; }
    inline void setRadius(int r) { _radius = r; }

    inline float width() const { return (float) diameter(); }
    inline float height() const { return width(); }
    inline QSizeF size() const { return QSizeF(width(), height()); }
    inline QRectF rect() const { return QRectF(x() - _radius, y() - _radius, width(), height()); }

    inline bool showLabel() const override { return _showLabel; }
    inline void setShowLabel(bool b) override { _showLabel = b; }

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
    inline void setAddress(uint64_t address) {
        _address = address;
        _label = numToAlpha(address);
    }
    inline uint64_t address() const { return _address; }

    // Connection Methods
    void connect(const SharedDipole &dipole) { _connections.insert(dipole); }
    void disconnect(const SharedDipole &dipole) { _connections.remove(dipole); }
    DipolesSet connections() const { return _connections; }

    // UI Methods (getAttr and setData)
    QVariant getAttr(const Attr attr) const override {
        switch(attr){
        case Attr::ID: return _id;
        case Attr::Address: return numToAlpha(_address);
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
        case Attr::Address: setAddress(alphaToNum(v.toString())); return;
        case Attr::Radius: setRadius(v.toUInt()); return;
        default: WorldPoint::setAttr(attr, v); return;
        }
    }

    SharedAlias clone(const uint64_t id) const {
        return std::make_shared<Alias>(id, address(), x(), y(), radius(), _brush, showLabel());
    }
};
