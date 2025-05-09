#pragma once
// C++ Standard Library
#include <atomic>
#include <cstdint>
#include <memory>
#include <limits>
#include <cmath>
#include <chrono>
#include <stdexcept>

// Qt Core
#include <QString>
#include <QHash>
#include <QSet>
#include <QPair>
#include <QVector>
#include <QPointF>
#include <QColor>
#include <QtGlobal>

// =============================================
// Forward Declarations
// =============================================
class Object;
class Alias;
class Dipole;
class Wire;
class BCPoint;
class WorldPoint;
class Scene;
class QPainterPath;

// =============================================
// Type Aliases
// =============================================

// ID generator
using IDTracker = std::atomic<uint64_t>;

// Shared pointers
using SharedPoint      = std::shared_ptr<QPointF>;
using SharedWorldPoint = std::shared_ptr<WorldPoint>;

// Weak pointers
using WeakObject = std::weak_ptr<Object>;
using WeakAlias  = std::weak_ptr<Alias>;
using WeakDipole = std::weak_ptr<Dipole>;
using WeakWire   = std::weak_ptr<Wire>;

// Shared pointers for objects
using SharedObject = std::shared_ptr<Object>;
using SharedAlias  = std::shared_ptr<Alias>;
using SharedDipole = std::shared_ptr<Dipole>;

// Maps and Sets
using AliasMap    = QHash<int, SharedAlias>;
using MergeMap    = QHash<SharedObject, SharedObject>;
using MovementMap = QHash<SharedPoint, QPair<QPointF, QPointF>>;

using DipolesSet = QSet<SharedDipole>;
using Selection  = QSet<std::shared_ptr<Object>>;

// =============================================
// Enumerations & Constants
// =============================================
constexpr int VARNAME_MAX_LENGTH             = 32;
constexpr int VARVALUE_STRING_MAX_LENGTH     = 64;

enum class ObjectCategory { Void = -1, Node, Dipole, Edit };

enum class ObjectType {
    Void = -1, Alias, Ground, Wire, Resistor,
    Capacitor, Inductor, Battery,
    DCV, ACV, DCI, ACI, Diode,
    BCP, BCCP
};

enum VariableType {
    VAR_NULL = -1,
    VAR_STRING,
    VAR_DOUBLE,
    VAR_RESISTANCE,
    VAR_CAPACITANCE,
    VAR_VOLTAGE,
    VAR_INDUCTANCE,
    VAR_QUANTITY,
    VAR_INTENSITY,
    VAR_INT
};

enum SIUnit { NO_UNIT = -1, OHM, FARAD, HENRY, VOLT, AMP, COULOMB };

enum class Attr {
    None = -1, ID, Address,
    ShowLabel, Name, Label,
    StrokeColor, FillColor, StrokeWidth, AliasOutline,
    X, Y, X1, X2, Y1, Y2, P, P1, P2,
    Width, Height, Size, Radius, Diameter, Rect,
    Length, Angle,
    Gnd
};

enum class ToolType { Selector, Pen, Grabber };
enum class DCRole   { R, V, I };
enum class Param    { None, R, C, L, I, V, Q, R0, C0, L0, I0, V0, Q0 };

// =============================================
// Inline Helper Functions
// =============================================
inline QString getVariableKey(SIUnit unit) {
    switch (unit) {
    case OHM:     return "r";
    case FARAD:   return "c";
    case HENRY:   return "l";
    case VOLT:    return "e";
    case AMP:     return "i";
    case COULOMB: return "q";
    default:      return "";
    }
}

inline QString getVariableKey(VariableType type) {
    switch (type) {
    case VAR_RESISTANCE:  return "r";
    case VAR_CAPACITANCE: return "c";
    case VAR_INDUCTANCE:  return "l";
    case VAR_VOLTAGE:     return "e";
    case VAR_INTENSITY:   return "i";
    case VAR_QUANTITY:    return "q";
    case VAR_STRING:      return "txt";
    case VAR_DOUBLE:      return "x";
    default:              return "";
    }
}

inline QString getUnitTextLabel(SIUnit unit) {
    switch (unit) {
    case OHM:     return "Resistance";
    case FARAD:   return "Capacitance";
    case HENRY:   return "Inductance";
    case VOLT:    return "Voltage";
    case AMP:     return "Intensity";
    case COULOMB: return "Quantity";
    default:      return "";
    }
}

inline QString getOrderSymbol(int order) {
    if (order < -9) return "p";
    if (order < -6) return "n";
    if (order < -3) return "μ";
    if (order <  0) return "m";
    if (order <  3) return "";
    if (order <  6) return "k";
    if (order <  9) return "M";
    if (order < 12) return "G";
    return "T";
}

inline int magnitude(double value) {
    if (value == 0.0) return 0;
    double v = std::abs(value);
    if (v < 1e-9)  return -12;
    if (v < 1e-6)  return -9;
    if (v < 1e-3)  return -6;
    if (v < 1)     return -3;
    if (v < 1e3)   return 0;
    if (v < 1e6)   return 3;
    if (v < 1e9)   return 6;
    if (v < 1e12)  return 9;
    return 12;
}

inline QPair<int,int> unitMagnitudeRange(SIUnit unit) {
    switch (unit) {
    case OHM:      return qMakePair(-3, 9);
    case FARAD:
    case HENRY:    return qMakePair(-12, 3);
    case VOLT:
    case AMP:
    case COULOMB:  return qMakePair(-9, 3);
    default:       return qMakePair(0,0);
    }
}

inline SIUnit getUnitOfVariableType(VariableType type) {
    switch (type) {
    case VAR_RESISTANCE:  return OHM;
    case VAR_CAPACITANCE: return FARAD;
    case VAR_VOLTAGE:     return VOLT;
    case VAR_INDUCTANCE:  return HENRY;
    case VAR_QUANTITY:    return COULOMB;
    case VAR_INTENSITY:   return AMP;
    default:              return NO_UNIT;
    }
}

inline bool varIsDouble(VariableType type) {
    switch (type) {
    case VAR_RESISTANCE:
    case VAR_CAPACITANCE:
    case VAR_VOLTAGE:
    case VAR_INDUCTANCE:
    case VAR_QUANTITY:
    case VAR_INTENSITY:
    case VAR_DOUBLE:
        return true;
    default:
        return false;
    }
}

inline SIUnit SIUnitOfVariable(VariableType type) {
    return getUnitOfVariableType(type);
}

inline VariableType varTypeOf(ObjectType t) {
    switch (t) {
    case ObjectType::Resistor:  return VAR_RESISTANCE;
    case ObjectType::Capacitor: return VAR_CAPACITANCE;
    case ObjectType::Inductor:  return VAR_INDUCTANCE;
    case ObjectType::Battery:
    case ObjectType::DCV:       return VAR_VOLTAGE;
    case ObjectType::DCI:       return VAR_INTENSITY;
    default:                    return VAR_NULL;
    }
}

inline QPair<double,double> rangeOfVariable(VariableType type) {
    if (varIsDouble(type)) {
        auto r = unitMagnitudeRange(SIUnitOfVariable(type));
        return qMakePair(std::pow(10.0, r.first),
                         std::pow(10.0, r.second + 3));
    }
    if (type == VAR_STRING) {
        return qMakePair(0.0, 255.0);
    }
    if (type == VAR_INT) {
        return qMakePair(
            static_cast<double>(std::numeric_limits<int>::min()),
            static_cast<double>(std::numeric_limits<int>::max())
            );
    }
    return qMakePair(0.0, 0.0);
}

inline QString unitSymbol(SIUnit unit) {
    switch (unit) {
    case OHM:     return "Ω";
    case FARAD:   return "F";
    case HENRY:   return "H";
    case VOLT:    return "V";
    case AMP:     return "A";
    case COULOMB: return "C";
    default:      return "";
    }
}

inline QString paramKey(Param p) {
    switch (p) {
    case Param::R:  return "r";
    case Param::C:  return "c";
    case Param::L:  return "l";
    case Param::I:  return "i";
    case Param::V:  return "v";
    case Param::Q:  return "q";
    case Param::R0: return "r0";
    case Param::C0: return "c0";
    case Param::L0: return "l0";
    case Param::I0: return "i0";
    case Param::V0: return "v0";
    case Param::Q0: return "q0";
    default:        return "";
    }
}

inline Param keyParam(const QString &key) {
    if      (key == "r")  return Param::R;
    else if (key == "c")  return Param::C;
    else if (key == "l")  return Param::L;
    else if (key == "i")  return Param::I;
    else if (key == "v")  return Param::V;
    else if (key == "q")  return Param::Q;
    else if (key == "r0") return Param::R0;
    else if (key == "c0") return Param::C0;
    else if (key == "l0") return Param::L0;
    else if (key == "i0") return Param::I0;
    else if (key == "v0") return Param::V0;
    else if (key == "q0") return Param::Q0;
    return Param::None;
}

inline QVector<Param> objectParams(ObjectType t) {
    switch (t) {
    case ObjectType::Resistor:  return {Param::R};
    case ObjectType::Capacitor: return {Param::C, Param::Q0};
    case ObjectType::Inductor:  return {Param::L, Param::I0};
    case ObjectType::DCV:
    case ObjectType::Battery:   return {Param::V};
    case ObjectType::DCI:       return {Param::I};
    default:                    return {};
    }
}

inline SIUnit paramUnit(Param p) {
    switch (p) {
    case Param::R:  case Param::R0: return OHM;
    case Param::C:  case Param::C0: return FARAD;
    case Param::L:  case Param::L0: return HENRY;
    case Param::I:  case Param::I0: return AMP;
    case Param::Q:  case Param::Q0: return COULOMB;
    case Param::V:  case Param::V0: return VOLT;
    default:                     return NO_UNIT;
    }
}

inline VariableType paramVartype(Param p) {
    switch (p) {
    case Param::R:  case Param::R0: return VAR_RESISTANCE;
    case Param::C:  case Param::C0: return VAR_CAPACITANCE;
    case Param::L:  case Param::L0: return VAR_INDUCTANCE;
    case Param::I:  case Param::I0: return VAR_INTENSITY;
    case Param::Q:  case Param::Q0: return VAR_QUANTITY;
    case Param::V:  case Param::V0: return VAR_VOLTAGE;
    default:                    return VAR_NULL;
    }
}

inline QString formatDouble(double value, int precision, SIUnit unit, bool raw) {
    static constexpr int LENGTH = 11, CENTER = 5;
    static const QString prefixes[LENGTH] = { "f","p","n","µ","m","","k","M","G","T","P" };
    if (value == 0.0) return "0";
    int idx = CENTER;
    if (!raw) {
        if (value > 0) {
            while (value < 1 && idx > 1) { value *= 1000.0; --idx; }
            while (value >= 10 && idx < LENGTH-1) { value /= 1000.0; ++idx; }
        } else {
            while (value > -1 && idx > 1) { value *= 1000.0; --idx; }
            while (value <= -10 && idx < LENGTH-1) { value /= 1000.0; ++idx; }
        }
    }
    return QString::number(value, 'f', precision)
           + prefixes[idx]
           + unitSymbol(unit);
}

inline long long now() {
    using ms_t = std::chrono::milliseconds;
    return std::chrono::duration_cast<ms_t>(
               std::chrono::system_clock::now().time_since_epoch()
               ).count();
}

QPointF indicateTarget(
    Scene *scene,
    QPainterPath &indicators,
    const QPointF &target,
    const QVector<QPointF> &points,
    float zoom
    );

inline QString numToAlpha(uint64_t number) {
    QString result;
    number++;
    while (number > 0) {
        number--;
        int r = number % 26;
        result.prepend(QChar('A' + r));
        number /= 26;
    }
    return result;
}

inline uint64_t alphaToNum(const QString &str) {
    uint64_t result = 0;
    for (int i = 0; i < str.length(); ++i) {
        QChar ch = str[i].toUpper();
        if (ch < 'A' || ch > 'Z')
            throw std::invalid_argument("Invalid character in base-26 string");
        result = result * 26 + (ch.unicode() - 'A' + 1);
    }
    return result - 1;
}

// =============================================
// Color Palette
// =============================================
namespace Palette {
const QColor HOVER      = QColor("#50B4FF");
const QColor SELECT     = QColor("#288CFF");
const QColor RUBBER_BAND_FILL   = QColor("#9FC8FF32");
const QColor RUBBER_BAND_STROKE = QColor("#9FC8FF");
const QColor INDICATOR_STROKE       = QColor("#FF5028");
const QColor MERGE_INDICATOR_STROKE = QColor("#2979FF");
const QColor CONSTRUCTION_ALLOWED    = QColor("#50FF28");
const QColor CONSTRUCTION_PROHIBITED = QColor("#FF4C4C");
const QColor BCCP  = QColor("#2979FF");
const QColor BCP   = QColor("#F5F5F5");
}

inline bool isDarkColor(const QColor &c) {
    // brightness = 0.299 R + 0.587 G + 0.114 B
    return (0.299*c.red() + 0.587*c.green() + 0.114*c.blue()) < 128;
}



// =============================================
// Limits
// =============================================
namespace Limits {
// UI/Geometry
constexpr int    MIN_ALIAS_RADIUS      =    6;
constexpr int    MAX_ALIAS_RADIUS      =   16;
constexpr int    MIN_STROKE_WIDTH      =    1;
constexpr int    MAX_STROKE_WIDTH      =    4;
constexpr int    MIN_ALIAS_OUTLINE     =    0;
constexpr int    MAX_ALIAS_OUTLINE     =    3;
constexpr int    MIN_TILESIZE          =   10;
constexpr int    MAX_TILESIZE          =  250;

// Physics
constexpr double MIN_RESISTANCE_VALUE  =  0.0;
constexpr double MAX_RESISTANCE_VALUE  = 1e12;
constexpr double MIN_CAPACITANCE_VALUE =  0.0;
constexpr double MAX_CAPACITANCE_VALUE =  1e2;
constexpr double MIN_INDUCTANCE_VALUE  =  0.0;
constexpr double MAX_INDUCTANCE_VALUE  =  1e3;
constexpr double MIN_VOLTAGE_VALUE     = -1e6;
constexpr double MAX_VOLTAGE_VALUE     =  1e6;
constexpr double MIN_CURRENT_VALUE     = -1e3;
constexpr double MAX_CURRENT_VALUE     =  1e3;
constexpr double MIN_QUANTITY_VALUE    =  0.0;
constexpr double MAX_QUANTITY_VALUE    =  1e3;
}
