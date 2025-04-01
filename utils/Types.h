#pragma once
#include <QVector>
#include <QSet>
#include <QPointF>
#include <QMap>
#include <memory>
#include <QHash>
#include <QRgb>
#include <QImage>
#include <QStringList>
#include <QString>

#include <QVariant>

using namespace std;

class Object;
class Alias;
class Dipole;
class Wire;
class BCPoint;
class WorldPoint;
class UnitDipole;

using SharedDouble = shared_ptr<double>;
using SharedInt = shared_ptr<int>;
using SharedString = shared_ptr<QString>;
using SharedPoint = shared_ptr<QPointF>;

using WeakObject = weak_ptr<Object>;
using WeakAlias = weak_ptr<Alias>;
using WeakDipole = weak_ptr<Dipole>;
using WeakWire = weak_ptr<Wire>;

using SharedObject = shared_ptr<Object>;
using SharedAlias = shared_ptr<Alias>;
using SharedDipole = shared_ptr<Dipole>;
using SharedWire = shared_ptr<Wire>;
using SharedUnitDipole = shared_ptr<UnitDipole>;

using Selection = QSet<int>;
using AliasMap = QHash<int, SharedAlias>;

using SharedWorldPoint = shared_ptr<WorldPoint>;

using LockedDipoles = QSet<SharedDipole>;
using LockedWorldPointSelection = QSet<SharedWorldPoint>;
using LockedSelection = QSet<shared_ptr<Object>>;

using MergeMap = QHash<SharedObject, SharedObject>;
using MovementMap = QHash<SharedPoint, QPair<QPointF,QPointF>>;


enum ObjectCategory {
    _VOID = -1,
    _NODE = 0,
    _DIPOLE = 1,
    _EDIT = 2
};

enum ObjectType {
    VOID = -1,
    ALIAS = 0,
    WIRE = 1,
    RESISTOR = 2,
    CAPACITOR = 3,
    INDUCTOR = 4,
    DIODE = 5,
    DC_VOLTAGE_GENERATOR = 6,
    BATTERY = 7,
    AC_VOLTAGE_SOURCE = 8,
    GROUND = 9,
    DC_CURRENT_GENERATOR = 10,
    AC_CURRENT_SOURCE = 11,
    NODE = 12,
    BC_POINT = 13,
    BC_CONTROL_POINT = 14
};

enum VariableType {
    VAR_NULL = -1,
    VAR_STRING = 0,
    VAR_DOUBLE = 1,
    VAR_RESISTANCE = 2,
    VAR_CAPACITANCE = 3,
    VAR_VOLTAGE = 4,
    VAR_INDUCTANCE = 5,
    VAR_QUANTITY = 6,
    VAR_INTENSITY = 7,
    VAR_INT = 8
};

enum SIUnit {
    NO_UNIT = -1,
    OHM = 0,
    FARAD = 1,
    HENRY = 2,
    VOLT = 3,
    AMP = 4,
    COULOMB = 5
};

const double DEFAULT_RESISTANCE_VALUE = 1e3;
const double DEFAULT_CAPACITANCE_VALUE = 1e-8;
const double DEFAULT_INDUCTANCE_VALUE = 1e-4;
const double DEFAULT_BATTERY_VALUE = 5;
const double DEFAULT_DC_VOLTAGE_GENERATOR_VALUE = 15.0;
const double DEFAULT_DC_CURRENT_GENERATOR_VALUE = 1e-3;



const QString GIGA_OHM = "GΩ";

const QString MEGA_OHM = "MΩ";

const QString KILO_OHM = "kΩ";
const QString KILO_FARAD = "kF";
const QString KILO_HENRY = "kH";
const QString KILO_VOLT = "kV";
const QString KILO_AMP = "kA";
const QString KILO_COULOMB = "kC";

const QString UNIT_OHM = "Ω";
const QString UNIT_FARAD = "F";
const QString UNIT_HENRY = "H";
const QString UNIT_VOLT = "V";
const QString UNIT_AMP = "A";
const QString UNIT_COULOMB = "C";

const QString MILLI_OHM = "mΩ";
const QString MILLI_FARAD = "mF";
const QString MILLI_HENRY = "mH";
const QString MILLI_VOLT = "mV";
const QString MILLI_AMP = "mA";
const QString MILLI_COULOMB = "mC";

const QString MICRO_FARAD = "μF";
const QString MICRO_HENRY = "μH";
const QString MICRO_VOLT = "μV";
const QString MICRO_AMP = "μA";
const QString MICRO_COULOMB = "μC";

const QString NANO_FARAD = "nF";
const QString NANO_HENRY = "nH";
const QString NANO_VOLT = "nV";
const QString NANO_AMP = "nA";
const QString NANO_COULOMB = "nC";

const QString PICO_FARAD = "pF";
const QString PICO_HENRY = "pH";


inline QString getVariableKey(const SIUnit unit) {
    switch (unit) {
    case OHM:      return "r";
    case FARAD:    return "c";
    case HENRY:    return "l";
    case VOLT:     return "e";
    case AMP:      return "i";
    case COULOMB:  return "q";
    default:       return "";
    }
}

inline QString getVariableKey(const VariableType type){
    switch (type) {
    case VAR_RESISTANCE: return "r";
    case VAR_CAPACITANCE:return "c";
    case VAR_INDUCTANCE: return "l";
    case VAR_VOLTAGE: return "e";
    case VAR_INTENSITY: return "i";
    case VAR_QUANTITY: return "q";
    case VAR_STRING: return "txt";
    case VAR_DOUBLE: return "x";
    default: return "";
    }
}



inline QString getUnitTextLabel(const SIUnit unit) {
    switch (unit) {
    case OHM:      return "Resistance";
    case FARAD:    return "Capacitance";
    case HENRY:    return "Inductance";
    case VOLT:     return "Voltage";
    case AMP:      return "Intensity";
    case COULOMB:  return "Quantity";
    default:       return "";
    }
}

inline QString getOrderSymbol(const int order) {
    if(order < -9) return "p";
    if(order < -6) return "n";
    if(order < -3) return "μ";
    if(order < 0) return "m";
    if(order < 3) return "";
    if(order < 6) return "k";
    if(order < 9) return "M";
    if(order < 12) return "G";
    return "T";
}


inline int magnitude(double value) {
    if(value == 0.0) return 0;
    const double v = std::abs(value);
    if(v < 1e-9) return -12;
    if(v < 1e-6) return -9;
    if(v < 1e-3) return -6;
    if(v < 1   ) return -3;
    if(v < 1e3 ) return 0;
    if(v < 1e6 ) return 3;
    if(v < 1e9 ) return 6;
    if(v < 1e12) return 9;
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
    case OHM:
        return qMakePair(-3, 9);
    case FARAD:
    case HENRY:
        return qMakePair(-12, 3);
    case VOLT:
    case AMP:
    case COULOMB:
        return qMakePair(-9, 3);
    default:
        return qMakePair(0, 0);
    }
}

inline SIUnit getUnitOfVariableType(const VariableType type) {
    switch (type) {
    case VAR_RESISTANCE: return OHM;
    case VAR_CAPACITANCE: return FARAD;
    case VAR_VOLTAGE: return VOLT;
    case VAR_INDUCTANCE: return HENRY;
    case VAR_QUANTITY: return COULOMB;
    case VAR_INTENSITY: return AMP;
    default: return NO_UNIT;
    }
}


inline bool varIsDouble(const VariableType type){
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

inline QVariant defaultOf(const VariableType type){
    switch (type) {
    case VAR_RESISTANCE: return DEFAULT_RESISTANCE_VALUE;
    case VAR_CAPACITANCE: return DEFAULT_CAPACITANCE_VALUE;
    case VAR_VOLTAGE: return DEFAULT_INDUCTANCE_VALUE;
    case VAR_INDUCTANCE: return DEFAULT_BATTERY_VALUE;
    case VAR_QUANTITY: return DEFAULT_DC_VOLTAGE_GENERATOR_VALUE;
    case VAR_INTENSITY: return DEFAULT_DC_CURRENT_GENERATOR_VALUE;
    case VAR_STRING: return "";
    case VAR_DOUBLE: return 0.0;
    case VAR_INT: return 0;
    default: return -1;
    }
}

inline SIUnit SIUnitOfVariable(const VariableType type){
    switch (type) {
    case VAR_RESISTANCE: return OHM;
    case VAR_CAPACITANCE: return FARAD;
    case VAR_VOLTAGE: return VOLT;
    case VAR_INDUCTANCE: return HENRY;
    case VAR_QUANTITY: return COULOMB;
    case VAR_INTENSITY: return AMP;
    default: return NO_UNIT;
    }
}


inline VariableType varTypeOf(const ObjectType type){
    switch(type){
    case RESISTOR: return VAR_RESISTANCE;
    case CAPACITOR: return VAR_CAPACITANCE;
    case INDUCTOR: return VAR_INDUCTANCE;
    case BATTERY:
    case DC_VOLTAGE_GENERATOR: return VAR_VOLTAGE;
    case DC_CURRENT_GENERATOR: return VAR_INTENSITY;
    default: return VAR_NULL;
    }
}

inline QPair<double,double> rangeOfVariable(const VariableType type){
    switch (type) {
    case VAR_RESISTANCE:
    case VAR_CAPACITANCE:
    case VAR_VOLTAGE:
    case VAR_INDUCTANCE:
    case VAR_QUANTITY:
    case VAR_INTENSITY: {
        const QPair<int, int> range = unitMagnitudeRange(SIUnitOfVariable(type));
        return qMakePair(pow(10, range.first), pow(10, range.second+3));
    }
    case VAR_STRING: return qMakePair(0.0, 255.0);
    case VAR_DOUBLE: return qMakePair(0.0, numeric_limits<double>::max());
    case VAR_INT: return qMakePair(numeric_limits<int>::min(), numeric_limits<int>::max());
    default: return qMakePair(0.0, 0.0);
    }
}


inline bool isUnitDipole(const ObjectType type){
    switch(type){
    case RESISTOR:
    case INDUCTOR:
    case CAPACITOR:
    case BATTERY:
    case DC_VOLTAGE_GENERATOR:
    case DC_CURRENT_GENERATOR:
        return true;
    default:
        return false;
    }
}

struct VarMetadata {
    QString name;
    VariableType type;
    VarMetadata(const QString &n, const VariableType type): name(n), type(type){};
};

struct VarValuedata {
    double value;
    VariableType type;
    VarValuedata(const double v, const VariableType type): value(v), type(type){};
};

using SharedVarMetadata = shared_ptr<VarMetadata>;
using DoubleVarTable = QHash<SharedDouble, SharedVarMetadata>;
using StringVarTable = QHash<SharedString, SharedVarMetadata>;
using IntVarTable = QHash<SharedInt, SharedVarMetadata>;
