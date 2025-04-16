#pragma once
#include <QRectF>
#include <QLineF>
#include <QPointF>
#include <cmath>

constexpr double DEG1   = 0.0174532925199433;
constexpr double DEG360 = 360*DEG1;
constexpr double DEG345 = 345*DEG1;
constexpr double DEG330 = 330*DEG1;
constexpr double DEG315 = 315*DEG1;
constexpr double DEG300 = 300*DEG1;
constexpr double DEG285 = 285*DEG1;
constexpr double DEG270 = 270*DEG1;
constexpr double DEG255 = 255*DEG1;
constexpr double DEG240 = 240*DEG1;
constexpr double DEG225 = 225*DEG1;
constexpr double DEG210 = 210*DEG1;
constexpr double DEG195 = 195*DEG1;
constexpr double DEG180 = 180*DEG1;
constexpr double DEG165 = 165*DEG1;
constexpr double DEG150 = 150*DEG1;
constexpr double DEG135 = 135*DEG1;
constexpr double DEG120 = 120*DEG1;
constexpr double DEG105 = 105*DEG1;
constexpr double DEG90  =  90*DEG1;
constexpr double DEG75  =  75*DEG1;
constexpr double DEG60  =  60*DEG1;
constexpr double DEG45  =  45*DEG1;
constexpr double DEG30  =  30*DEG1;
constexpr double DEG15  =  15*DEG1;
constexpr double EPSILON = 1e-6;
constexpr double EPSILON_SQ = EPSILON * EPSILON;

constexpr double SQRT_2 = 1.4142135623730950;

constexpr int MAX_ITER = 100;

constexpr int STROKE_HOVER_MAGNET = 6;

constexpr int MARGIN_NODE_NAME = 6;

constexpr int DEFAULT_WORLDOBJECT_STATE = 0;
constexpr int HOVER_WORLDOBJECT_STATE = 1;
constexpr int SELECTION_WORLDOBJECT_STATE = 2;

constexpr int DIPOLE_BOX_HEIGHT = 44;
constexpr int DIPOLE_BOX_WIDTH = 64;

constexpr int RESISTOR_WIDTH = 32;
constexpr int RESISTOR_HEIGHT = 22;
constexpr int RESISTOR_OSCILLATIONS = 9;

constexpr int CAPACITOR_WIDTH = 6;
constexpr int CAPACITOR_HEIGHT = 30;

constexpr int BATTERY_WIDTH = 8;
constexpr int BATTERY_POSITIVE_HEIGHT = 36;
constexpr int BATTERY_NEGATIVE_HEIGHT = 12;

constexpr int INDUCTOR_WIDTH = 60;
constexpr int INDUCTOR_OSCILLATIONS = 4;
constexpr int INDUCTOR_HEIGHT = 20;

constexpr int BC_EDIT_POINT_WIDTH = 6;
constexpr int BC_EDIT_CONTROL_POINT_WIDTH = 8;
constexpr double BC_EDIT_CONTROL_POINT_DIAGONAL_WIDTH = BC_EDIT_CONTROL_POINT_WIDTH/SQRT_2;

constexpr int DEFAULT_CIRCLE_RADIUS = 20;
constexpr int DEFAULT_CIRCLE_DIAMETER = 40;

constexpr double DEFAULT_ARROW_ANGLE = DEG45;

constexpr int DCV_ARROW_H = 6;

constexpr int GROUND_ANTENNA_H = 6;
constexpr int GROUND_ROW_GAP = 4;
constexpr int GROUND_WIDTH_1 = 16;
constexpr int GROUND_WIDTH_2 = 8;
constexpr int GROUND_WIDTH_3 = 4;

// HALFING
constexpr int HALF_DIPOLE_BOX_HEIGHT = DIPOLE_BOX_HEIGHT/2;
constexpr int HALF_DIPOLE_BOX_WIDTH = DIPOLE_BOX_WIDTH/2;
constexpr int HALF_RESISTOR_HEIGHT = RESISTOR_HEIGHT/2;
constexpr int HALF_CAPACITOR_HEIGHT = CAPACITOR_HEIGHT/2;
constexpr int HALF_BATTERY_POSITIVE_HEIGHT = BATTERY_POSITIVE_HEIGHT/2;
constexpr int HALF_BATTERY_NEGATIVE_HEIGHT = BATTERY_NEGATIVE_HEIGHT/2;
constexpr int HALF_INDUCTOR_HEIGHT = INDUCTOR_HEIGHT/2;

constexpr int HALF_BC_EDIT_POINT_WIDTH = BC_EDIT_POINT_WIDTH/2;
constexpr int HALF_BC_EDIT_CONTROL_POINT_WIDTH = BC_EDIT_CONTROL_POINT_WIDTH/2;

constexpr int HALF_DCV_ARROW_H = DCV_ARROW_H/2;

constexpr int HALF_GROUND_ANTENNA_H = GROUND_ANTENNA_H/2;
constexpr int HALF_GROUND_ROW_GAP = GROUND_ROW_GAP/2;
constexpr int HALF_GROUND_WIDTH_1 = GROUND_WIDTH_1/2;
constexpr int HALF_GROUND_WIDTH_2 = GROUND_WIDTH_2/2;
constexpr int HALF_GROUND_WIDTH_3 = GROUND_WIDTH_3/2;

constexpr int HALF_CAPACITOR_WIDTH = CAPACITOR_WIDTH/2;
constexpr int HALF_RESISTOR_WIDTH = RESISTOR_WIDTH/2;
constexpr int HALF_BATTERY_WIDTH = BATTERY_WIDTH/2;
constexpr int HALF_INDUCTOR_WIDTH = INDUCTOR_WIDTH/2;


// Red means "reduceable", it checks whether there is useful information in the geometry.
// Inline trivial functions for better performance:
inline bool PPfuzzyCmp(const QPointF &p1, const QPointF &p2) {
    return qFuzzyCompare(p1.x(), p2.x()) && qFuzzyCompare(p1.y(), p2.y());
}
inline bool PLessThan(const QPointF &p1, const QPointF &p2) {
    return (p1.x() < p2.x() || (qFuzzyCompare(p1.x(), p2.x()) && p1.y() < p2.y()));
}
inline bool between(float a, float x, float b) { return !(x < a || x > b); }

inline bool isLV(const QLineF &l){ return qFuzzyCompare(std::fabs(l.dx()), 1e-6); }
inline bool isLH(const QLineF &l){ return qFuzzyCompare(std::fabs(l.dy()), 1e-6); }

// QPointF Topology
inline bool PinR(const QPointF &p, const QRectF &rect){ return rect.contains(p); }
inline QRectF PtoR(const QPointF &p, float r) { return QRectF(p.x() - r, p.y() - r, 2*r, 2*r); }
bool PLessThan(const QPointF &p1, const QPointF &p2);
bool PPfuzzyCmp(const QPointF &p1, const QPointF &p2);
bool PinL(const QPointF &p, const QLineF &l, float lw=1.0);
bool PinC(const QPointF &p, const QPointF &c, float r);
bool PinBC(const QPointF &p, const QPointF &P0, const QPointF &P1, const QPointF &P2, const QPointF &P3, float lw);
bool PinRotatedR(const QPointF &p, const QPointF &center, const float halfW, const float halfH, float angle);
bool PinDia(const QPointF &p, const QPointF &center, const float d1, const float d2);


// Circle Topology
bool CinR(const QPointF &center, const float radius, const QRectF &rect);


// Line Topology
QRectF LtoR(const QLineF &l);
QRectF LtoR(const QLine &l);
bool LinL(const QLineF &l1, const QLineF &l2);
bool LinR(const QLineF &l, const QRectF &rect);
QPair<QLineF::IntersectionType, QLineF> LlapL(const QLineF &l1, const QLineF &l2);
QLineF::IntersectionType T_LlapL(const QLineF &l1, const QLineF &l2, bool closed = false);

bool segmentContains(const QLineF& line, const QPointF& point);

// Projections
QPointF pA(const QPointF& A, const QPointF& B);
QPointF pLA(const QLineF& L, const QPointF& A);

// BC Topology
QRectF BCtoR(const QPointF &P0, const QPointF &P1, const QPointF &P2, const QPointF &P3);
bool BCinR(const QPointF &P0, const QPointF &P1, const QPointF &P2, const QPointF &P3, const QRectF &rect);


void solveSecond(float roots[3], float a, float b, float c);
void solveThird(float roots[4], float a, float b, float c, float d);
