#pragma once
#include <QRectF>
#include <QColor>
#include <iostream>
#include <QPainter>
#include "utils/Types.h"

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

    Object() = default;
    virtual ~Object() = default;
    virtual ObjectType type() const { return VOID; };
    virtual ObjectCategory category() const { return _VOID; };
    virtual bool hover(const QPointF &p, [[maybe_unused]] const float zoom) = 0;
    virtual bool inside(const QRectF &box, [[maybe_unused]] const float zoom) = 0;
    virtual bool visible(const QRectF &viewport, [[maybe_unused]] const float zoom) = 0;

    virtual void setColor([[maybe_unused]] const QColor &color){}
    virtual QColor color() const { return Qt::white; };

    virtual QString dataString(const QString &key) const {
        if(key == "color") return color().name();
        if(key == "name") return name();
        return "";
    }

    virtual void setData(const QString &key, const QString &value){
        if(key == "color") setColor(QColor(value));
    }

    virtual QString name() const { return Object::name(type()); };

    friend std::ostream& operator<<(std::ostream& os, const Object& p) {
        return os << p.name().toStdString();
    }
};
