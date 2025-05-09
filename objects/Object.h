#pragma once
#include "utils/Globals.h"

#include <iostream>

#include <QColor>
#include <QPainter>
#include <QVariant>
#include <QString>
#include <QSizeF>
#include <QPointF>
#include <QBrush>
#include <QPen>
#include <QRectF>
#include <QLineF>

class Object {
public:
    constexpr static char ALIAS_DISPLAY_NAME[] = "Alias";
    constexpr static char WIRE_DISPLAY_NAME[] = "Wire";
    constexpr static char RESISTOR_DISPLAY_NAME[] = "Resistor";
    constexpr static char CAPACITOR_DISPLAY_NAME[] = "Capacitor";
    constexpr static char INDUCTOR_DISPLAY_NAME[] = "Inductor";
    constexpr static char BATTERY_DISPLAY_NAME[] = "Battery";
    constexpr static char DCV_DISPLAY_NAME[] = "DCV Generator";
    constexpr static char DCI_DISPLAY_NAME[] = "DCI Generator";

    static ObjectType parseType(const QString &type) {
        if(type == ALIAS_DISPLAY_NAME) return ObjectType::Alias;
        if(type == WIRE_DISPLAY_NAME) return ObjectType::Wire;
        if(type == RESISTOR_DISPLAY_NAME) return ObjectType::Resistor;
        if(type == CAPACITOR_DISPLAY_NAME) return ObjectType::Capacitor;
        if(type == INDUCTOR_DISPLAY_NAME) return ObjectType::Inductor;
        if(type == BATTERY_DISPLAY_NAME) return ObjectType::Battery;
        if(type == DCV_DISPLAY_NAME) return ObjectType::DCV;
        if(type == DCI_DISPLAY_NAME) return ObjectType::DCI;
        return ObjectType::Void;
    }

    static QString name(ObjectType type) {
        switch(type){
        case ObjectType::Alias: return QString(ALIAS_DISPLAY_NAME);
        case ObjectType::Wire: return QString(WIRE_DISPLAY_NAME);
        case ObjectType::Resistor: return QString(RESISTOR_DISPLAY_NAME);
        case ObjectType::Capacitor: return QString(CAPACITOR_DISPLAY_NAME);
        case ObjectType::Inductor: return QString(INDUCTOR_DISPLAY_NAME);
        case ObjectType::Battery: return QString(BATTERY_DISPLAY_NAME);
        case ObjectType::DCV: return QString(DCV_DISPLAY_NAME);
        case ObjectType::DCI: return QString(DCI_DISPLAY_NAME);
        default: return "";
        }
    }

    static QString pluralName(ObjectType type) {
        switch(type){
        case ObjectType::Alias: return "Aliases";
        case ObjectType::Wire: return "Wires";
        case ObjectType::Resistor: return "Resistors";
        case ObjectType::Capacitor: return "Capacitors";
        case ObjectType::Inductor: return "Inductors";
        case ObjectType::Battery: return "Batteries";
        case ObjectType::DCV: return "DCV Generators";
        case ObjectType::DCI: return "DCI Generators";
        default: return "";
        }
    }

protected:
    QBrush _brush;
    QPen _pen;

public:
    Object(const QBrush &brush, const QPen &pen): _brush(brush), _pen(pen) {}
    Object(): _brush(Qt::NoBrush), _pen(Qt::NoPen) {}
    Object(const QBrush &brush): _brush(brush), _pen(Qt::NoPen) {}
    Object(const QPen &pen): _brush(Qt::NoBrush), _pen(pen) {}

    virtual ~Object() = default;
    virtual ObjectType type() const { return ObjectType::Void; };
    virtual ObjectCategory category() const { return ObjectCategory::Void; };
    virtual bool hover(const QPointF &p, [[maybe_unused]] const float zoom) = 0;
    virtual bool inside(const QRectF &box, [[maybe_unused]] const float zoom) = 0;
    virtual bool visible(const QRectF &viewport, [[maybe_unused]] const float zoom) = 0;

    inline bool hasPen() const { return _pen.style() != Qt::NoPen; }
    inline bool hasBrush() const { return _brush.style() != Qt::NoBrush; }

    inline QPen pen() const { return _pen; }
    inline QBrush brush() const { return _brush; }
    inline QColor fillColor() const { return _brush.color(); }
    inline QColor strokeColor() const { return _pen.color(); }
    inline int strokeWidth() const { return _pen.width(); }

    virtual inline void setPen(const QPen &pen) { _pen = pen; }
    virtual inline void setBrush(const QBrush &brush) { _brush = brush; }
    virtual inline void setFillColor(const QColor &color) { _brush.setColor(color); }
    virtual inline void setStrokeColor(const QColor &color) { _pen.setColor(color); }
    virtual inline void setStrokeWidth(const int w) { _pen.setWidth(w); }

    inline virtual void setShowLabel([[maybe_unused]] const bool b) {}
    inline virtual bool showLabel() const { return false; }
    inline virtual QString label([[maybe_unused]] const bool raw = true) const { return ""; }

    virtual QVariant getAttr(const Attr attr) const {
        switch (attr) {
        case Attr::ShowLabel: return showLabel();
        case Attr::Label: return label();
        case Attr::Name: return name();
        case Attr::StrokeColor: return fillColor();
        case Attr::FillColor: return strokeColor();
        case Attr::StrokeWidth: return strokeWidth();
        default: return "";
        }
    }

    virtual void setAttr(const Attr attr, const QVariant &v){
        switch (attr) {
        case Attr::ShowLabel: return setShowLabel(v.toBool()); return;
        case Attr::StrokeColor: setFillColor(v.toString()); return;
        case Attr::FillColor: setStrokeColor(v.toString()); return;
        case Attr::StrokeWidth: setStrokeWidth(v.toUInt()); return;
        default: return;
        }
    }

    inline QString name() const { return Object::name(type()); };
    friend std::ostream& operator<<(std::ostream& os, const Object& p) {
        return os << p.name().toStdString();
    }
};
