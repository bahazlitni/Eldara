#pragma once

// =============================================
// Includes
// =============================================
// Standard C++ headers
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <memory>

// Qt Core
#include <QVector>
#include <QHash>
#include <QMap>
#include <QPointF>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QVariant>
#include <QLineF>
#include <QObject>
#include <QtMath>
#include <QFile>

// Qt Gui
#include <QColor>
#include <QCursor>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QTransform>
#include <QRgb>
#include <QGraphicsColorizeEffect>
#include <QSvgRenderer>
#include <QGuiApplication>

// Qt Widgets
#include <QWidget>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QStack>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QCompleter>
#include <QStyle>
#include <QBoxLayout>
#include <QRegularExpressionValidator>
#include <QRegularExpression>


#include <QGraphicsPixmapItem>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QMutex>


// Using standard namespace
using namespace std;

// =============================================
// Forward Declarations
// =============================================
class Object;
class Alias;
class Dipole;
class Wire;
class BCPoint;
class WorldPoint;
class App;

// =============================================
// Type Aliases
// =============================================

// Exotic types
using IDTracker = std::atomic<uint64_t>;

// Shared pointers for basic types
using SharedPoint  = shared_ptr<QPointF>;
using SharedWorldPoint = shared_ptr<WorldPoint>;

// Weak pointers for object types
using WeakObject = weak_ptr<Object>;
using WeakAlias  = weak_ptr<Alias>;
using WeakDipole = weak_ptr<Dipole>;
using WeakWire   = weak_ptr<Wire>;

// Shared pointers for objects
using SharedObject = shared_ptr<Object>;
using SharedAlias  = shared_ptr<Alias>;
using SharedDipole = shared_ptr<Dipole>;

// Maps
using AliasMap    = QHash<int, SharedAlias>;
using MergeMap    = QHash<SharedObject, SharedObject>;
using MovementMap = QHash<SharedPoint, QPair<QPointF,QPointF>>;

// Sets
using DipolesSet = QSet<SharedDipole>;
using Selection  = QSet<shared_ptr<Object>>;

// =============================================
// Enumerations
// =============================================
enum ObjectCategory {
    _VOID = -1,
    _NODE = 0,
    _DIPOLE = 1,
    _EDIT = 2
};

enum ObjectType {
    VOID = -1,
    ALIAS,
    WIRE,
    RESISTOR,
    CAPACITOR,
    INDUCTOR,
    DIODE,
    DC_VOLTAGE_GENERATOR,
    BATTERY,
    AC_VOLTAGE_SOURCE,
    GROUND,
    DC_CURRENT_GENERATOR,
    AC_CURRENT_SOURCE,
    NODE,
    BC_POINT,
    BC_CONTROL_POINT
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
    StrokeColor, FillColor, StrokeWidth,
    X, Y, X1, X2, Y1, Y2, P, P1, P2,
    Width, Height, Size, Radius, Diameter, Rect,
    Length, Angle
};

// =============================================
// Constants
// =============================================
// Default values for electrical components
const double DEFAULT_RESISTANCE_VALUE = 1e3;
const double DEFAULT_CAPACITANCE_VALUE = 1e-8;
const double DEFAULT_INDUCTANCE_VALUE = 1e-4;
const double DEFAULT_BATTERY_VALUE = 5;
const double DEFAULT_DC_VOLTAGE_GENERATOR_VALUE = 15.0;
const double DEFAULT_DC_CURRENT_GENERATOR_VALUE = 1e-3;

// =============================================
// Variable and SI Unit Helper Functions
// =============================================

inline QString getVariableKey(const SIUnit unit) {
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

inline QString getVariableKey(const VariableType type) {
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

inline QString getUnitTextLabel(const SIUnit unit) {
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

inline QString getOrderSymbol(const int order) {
    if (order < -9) return "p";
    if (order < -6) return "n";
    if (order < -3) return "μ";
    if (order < 0)  return "m";
    if (order < 3)  return "";
    if (order < 6)  return "k";
    if (order < 9)  return "M";
    if (order < 12) return "G";
    return "T";
}

inline int magnitude(double value) {
    if (value == 0.0) return 0;
    const double v = std::abs(value);
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

inline QString getUnitSymbol(const SIUnit unit) {
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

inline QPair<int, int> unitMagnitudeRange(const SIUnit unit) {
    switch (unit) {
    case OHM:      return qMakePair(-3, 9);
    case FARAD:
    case HENRY:    return qMakePair(-12, 3);
    case VOLT:
    case AMP:
    case COULOMB:  return qMakePair(-9, 3);
    default:       return qMakePair(0, 0);
    }
}

inline SIUnit getUnitOfVariableType(const VariableType type) {
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

inline bool varIsDouble(const VariableType type) {
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

inline QVariant defaultOf(const VariableType type) {
    switch (type) {
    case VAR_RESISTANCE:  return DEFAULT_RESISTANCE_VALUE;
    case VAR_CAPACITANCE: return DEFAULT_CAPACITANCE_VALUE;
    case VAR_VOLTAGE:     return DEFAULT_INDUCTANCE_VALUE;
    case VAR_INDUCTANCE:  return DEFAULT_BATTERY_VALUE;
    case VAR_QUANTITY:    return DEFAULT_DC_VOLTAGE_GENERATOR_VALUE;
    case VAR_INTENSITY:   return DEFAULT_DC_CURRENT_GENERATOR_VALUE;
    case VAR_STRING:      return "";
    case VAR_DOUBLE:      return 0.0;
    case VAR_INT:         return 0;
    default:              return -1;
    }
}

inline SIUnit SIUnitOfVariable(const VariableType type) {
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

inline VariableType varTypeOf(const ObjectType type) {
    switch (type) {
    case RESISTOR: return VAR_RESISTANCE;
    case CAPACITOR: return VAR_CAPACITANCE;
    case INDUCTOR: return VAR_INDUCTANCE;
    case BATTERY:
    case DC_VOLTAGE_GENERATOR: return VAR_VOLTAGE;
    case DC_CURRENT_GENERATOR: return VAR_INTENSITY;
    default: return VAR_NULL;
    }
}

inline QPair<double, double> rangeOfVariable(const VariableType type) {
    switch (type) {
    case VAR_RESISTANCE:
    case VAR_CAPACITANCE:
    case VAR_VOLTAGE:
    case VAR_INDUCTANCE:
    case VAR_QUANTITY:
    case VAR_INTENSITY: {
        const QPair<int, int> range = unitMagnitudeRange(SIUnitOfVariable(type));
        return qMakePair(pow(10, range.first), pow(10, range.second + 3));
    }
    case VAR_STRING:  return qMakePair(0.0, 255.0);
    case VAR_DOUBLE:  return qMakePair(0.0, numeric_limits<double>::max());
    case VAR_INT:     return qMakePair(numeric_limits<int>::min(), numeric_limits<int>::max());
    default:          return qMakePair(0.0, 0.0);
    }
}

inline QString unitLabel(const SIUnit unit){
    switch(unit){
    case OHM: return "Resistance";
    case FARAD: return "Capacitance";
    case HENRY: return "Inductance";
    case AMP: return "Intensity";
    case COULOMB: return "Quantity";
    case VOLT: return "Voltage";
    default: return "";
    }
}

// =============================================
// Parameter Helper Functions
// =============================================
enum class Param {
    None, R, C, L, I, V, Q,
    R0, C0, L0, I0, V0, Q0
};

inline QString paramKey(const Param param) {
    switch (param) {
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
    if (key == "r")  return Param::R;
    if (key == "c")  return Param::C;
    if (key == "l")  return Param::L;
    if (key == "i")  return Param::I;
    if (key == "v")  return Param::V;
    if (key == "q")  return Param::Q;
    if (key == "r0") return Param::R0;
    if (key == "c0") return Param::C0;
    if (key == "l0") return Param::L0;
    if (key == "i0") return Param::I0;
    if (key == "v0") return Param::V0;
    if (key == "q0") return Param::Q0;
    return Param::None;
}

inline QVector<Param> objectParams(const ObjectType type){
    switch (type) {
    case RESISTOR: return QVector<Param>{Param::R};
    case CAPACITOR: return QVector<Param>{Param::C, Param::Q0};
    case INDUCTOR: return QVector<Param>{Param::L, Param::I0};
    case DC_VOLTAGE_GENERATOR:
    case BATTERY: return QVector<Param>{Param::V};
    case DC_CURRENT_GENERATOR: return QVector<Param>{Param::I};
    default: return QVector<Param>();
    }
}

inline SIUnit paramUnit(const Param param){
    switch(param){
    case Param::R : case Param::R0 : return OHM;
    case Param::C : case Param::C0 : return FARAD;
    case Param::L : case Param::L0 : return HENRY;
    case Param::I : case Param::I0 : return AMP;
    case Param::Q : case Param::Q0 : return COULOMB;
    case Param::V : case Param::V0 : return VOLT;
    default: return NO_UNIT;
    }
}

inline VariableType paramVartype(const Param param){
    switch(param){
    case Param::R : case Param::R0 : return VAR_RESISTANCE;
    case Param::C : case Param::C0 : return VAR_CAPACITANCE;
    case Param::L : case Param::L0 : return VAR_INDUCTANCE;
    case Param::I : case Param::I0 : return VAR_INTENSITY;
    case Param::Q : case Param::Q0 : return VAR_QUANTITY;
    case Param::V : case Param::V0 : return VAR_VOLTAGE;
    default: return VAR_NULL;
    }
}

// =============================================
// Miscellaneous Utility Functions
// =============================================
inline QString formatDouble(double value, int precision, SIUnit unit) {
    static constexpr int LENGTH = 11;
    static constexpr int i0 = 5;
    static const QString prefixes[LENGTH] = {
        "f","p","n","µ","m","","k","M","G","T","P"
    };

    if(value == 0) return "0";

    int i = i0;

    if(value > 0){
        while (value < 1 && i > 1) {
            value *= 1000.0f;
            --i;
        }
        while (value >= 10.0 && i < LENGTH - 1) {
            value /= 1000.0f;
            ++i;
        }
    }
    else {
        while (value > -1 && i > 1) {
            value *= 1000.0f;
            --i;
        }
        while (value <= -10.0 && i < LENGTH - 1) {
            value /= 1000.0f;
            ++i;
        }
    }

    return QString::number(value, 'f', precision)
        + prefixes[i]
        + getUnitSymbol(unit);
}


inline long long now() {
    auto currentTime = std::chrono::system_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(currentTime.time_since_epoch());
    return static_cast<long long>(ms.count());
}

QPointF indicateTarget(
    App *app,
    QPainterPath &indicators,
    const QPointF &target,
    const QVector<QPointF> &points,
    const float zoom
);

// =============================================
// Colors
// =============================================
namespace Palette {
    // Background and selection colors
    const QColor BACKGROUND = QColor("#212121");
    const QColor HOVER      = QColor("#50B4FF");
    const QColor SELECT     = QColor("#288CFF");

    // Rubber band colors
    const QColor RUBBER_BAND_FILL   = QColor("#9FC8FF32");
    const QColor RUBBER_BAND_STROKE = QColor("#9FC8FF");

    // Indicator colors
    const QColor INDICATOR_STROKE       = QColor("#FF5028");
    const QColor MERGE_INDICATOR_STROKE = QColor("#2979FF");

    // Construction colors
    const QColor CONSTRUCTION_ALLOWED    = QColor("#50FF28");
    const QColor CONSTRUCTION_PROHIBITED = QColor("#FF4C4C");

    // Bezier curve colors
    const QColor BC_CONTROL_POINT = QColor("#2979FF");
    const QColor BC_POINT         = QColor("#F5F5F5");

    // Grid
    const QColor GRID_STROKE = QColor(255,255,255,10);
};


inline bool isDarkColor(const QColor &color) {
    // Using the luminance formula to decide if a color is dark.
    return (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue()) < 128;
}

