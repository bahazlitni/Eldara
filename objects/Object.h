#pragma once
#include <QRectF>
#include <QColor>
#include <iostream>
#include <QPainter>
#include "utils/Globals.h"

class Object {
public:
    constexpr static char ALIAS_DISPLAY_NAME[] = "Alias";
    constexpr static char WIRE_DISPLAY_NAME[] = "Wire";
    constexpr static char RESISTOR_DISPLAY_NAME[] = "Resistor";
    constexpr static char CAPACITOR_DISPLAY_NAME[] = "Capacitor";
    constexpr static char INDUCTOR_DISPLAY_NAME[] = "Inductor";
    constexpr static char BATTERY_DISPLAY_NAME[] = "Battery";
    constexpr static char DC_VOLTAGE_GENERATOR_DISPLAY_NAME[] = "DCV Generator";
    constexpr static char DC_CURRENT_GENERATOR_DISPLAY_NAME[] = "DCI Generator";

    static ObjectType parseType(const QString &type) {
        if(type == ALIAS_DISPLAY_NAME) return ALIAS;
        if(type == WIRE_DISPLAY_NAME) return WIRE;
        if(type == RESISTOR_DISPLAY_NAME) return RESISTOR;
        if(type == CAPACITOR_DISPLAY_NAME) return CAPACITOR;
        if(type == INDUCTOR_DISPLAY_NAME) return INDUCTOR;
        if(type == BATTERY_DISPLAY_NAME) return BATTERY;
        if(type == DC_VOLTAGE_GENERATOR_DISPLAY_NAME) return DC_VOLTAGE_GENERATOR;
        if(type == DC_CURRENT_GENERATOR_DISPLAY_NAME) return DC_CURRENT_GENERATOR;
        return VOID;
    }

    static QString name(ObjectType type) {
        switch(type){
        case ALIAS: return QString(ALIAS_DISPLAY_NAME);
        case WIRE: return QString(WIRE_DISPLAY_NAME);
        case RESISTOR: return QString(RESISTOR_DISPLAY_NAME);
        case CAPACITOR: return QString(CAPACITOR_DISPLAY_NAME);
        case INDUCTOR: return QString(INDUCTOR_DISPLAY_NAME);
        case BATTERY: return QString(BATTERY_DISPLAY_NAME);
        case DC_VOLTAGE_GENERATOR: return QString(DC_VOLTAGE_GENERATOR_DISPLAY_NAME);
        case DC_CURRENT_GENERATOR: return QString(DC_CURRENT_GENERATOR_DISPLAY_NAME);
        default: return "";
        }
    }

    static QString pluralName(ObjectType type) {
        switch(type){
        case ALIAS: return "Aliases";
        case WIRE: return "Wires";
        case RESISTOR: return "Resistors";
        case CAPACITOR: return "Capacitors";
        case INDUCTOR: return "Inductors";
        case BATTERY: return "Batteries";
        case DC_VOLTAGE_GENERATOR: return "DCV Generators";
        case DC_CURRENT_GENERATOR: return "DCI Generators";
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
    virtual ObjectType type() const { return VOID; };
    virtual ObjectCategory category() const { return _VOID; };
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

    inline void setPen(const QPen &pen) { _pen = pen; }
    inline void setBrush(const QBrush &brush) { _brush = brush; }
    inline void setFillColor(const QColor &color) { _brush.setColor(color); }
    inline void setStrokeColor(const QColor &color) { _pen.setColor(color); }
    inline void setStrokeWidth(const int w) { _pen.setWidth(w); }

    inline virtual void setShowLabel([[maybe_unused]] const bool b) {}
    inline virtual bool showLabel() const { return false; }
    inline virtual QString label() const { return ""; }

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
