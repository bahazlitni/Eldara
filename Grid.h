#pragma once
#include "utils/Globals.h"

enum PainterMode {
    CONSTRUCTION_PREVIEW,
    IMAGINARY_LINES,
    SELECTION_BOX,
    WORLDOBJECT_FILL,
    WORLDOBJECT_STROKE,
    INDICATOR_STROKE,
    SELECTED_OBJECT_STROKE,
    GRID_STROKE,
    MERGE_INDICATOR
};

static int constexpr DEFAULT_ZOOM_INDEX = 5;
static int constexpr NUMBER_OF_ZOOM_STATES = 14;
static int constexpr MAJ_ZOOM_INDEX = NUMBER_OF_ZOOM_STATES-1;
float const ZOOM_STATES[NUMBER_OF_ZOOM_STATES] = {
    0.2f, 0.5f, 0.8f, 0.9f, 1.0f, 1.1f, 1.5f,
    2.0f, 2.5f, 3.0f, 4.0f, 5.0f, 6.0f, 8.0f
};

class Scene;
class BCPath;
class Grid: public QRectF {
private:
    Scene *scene;
    QPainter painter;
    int zoomIndex;
    QPixmap gridTile;
    bool dirtyVisibleCheckFlagInitial = false;

    QPointF midp;
    QPointF T;
    QPointF N;
    float ANGLE;

    void setupDipoleDrawingGeometry(const QPointF &A, const QPointF &B){
        ANGLE = std::atan2(B.y() - A.y(), B.x() - A.x());
        T = QPointF(std::cos(ANGLE), std::sin(ANGLE));
        N = QPointF(-T.y(), T.x());
        midp = (A + B)/2.0;
    }


public:
    static constexpr int MAX_COORDINATE_RANGE = 100000;

    Grid(Scene *scene);
    ~Grid(){}

    bool getDirtyVisibleCheckFlagInitial() const { return dirtyVisibleCheckFlagInitial; }

    QSet<SharedAlias> visibleAliases;
    QSet<SharedDipole> visibleDipoles;

    inline void addVisible(const SharedAlias &alias){ visibleAliases.insert(alias); }
    inline void removeVisible(const SharedAlias &alias){ visibleAliases.remove(alias); }
    void updateVisibility(const SharedAlias &alias);

    inline void addVisible(const SharedDipole &dipole){ visibleDipoles.insert(dipole); }
    inline void removeVisible(const SharedDipole &dipole){ visibleDipoles.remove(dipole); }
    void updateVisibility(const SharedDipole &dipole);

    void addVisible(const SharedObject &obj);
    void removeVisible(const SharedObject &obj);
    void updateVisibility(const SharedObject &obj);

    inline void addVisible(const Selection &selection){ for(const auto &obj : selection) addVisible(obj); }
    inline void removeVisible(const Selection &selection){ for(const auto &obj : selection) removeVisible(obj); }
    inline void updateVisibility(const Selection &selection){ for(const auto &obj : selection) updateVisibility(obj); }

    void updateVisibility();
    void updateSize();
    void updateGridTile();

    inline void deepRemoval(const SharedObject &obj) { removeVisible(obj); }

    inline float zoom() const { return ZOOM_STATES[zoomIndex]; };
    void setZoom(const QPointF &p, const int d);

    void render([[maybe_unused]] QPaintEvent *event);

    inline void moveTo(const float x, const float y){
        const QSizeF sz = size();
        setX(x);
        setY(y);
        setSize(sz);
        updateGridTile();
        updateVisibility();
    }
    inline void moveTo(const QPointF &p){ moveTo(p.x(), p.y()); }

    inline float toScreenX(const float x) const { return (x - left())*zoom(); }
    inline float toScreenY(const float y) const { return (y - top())*zoom(); }
    inline QPointF toScreen(const QPointF &pos) const { return QPointF(toScreenX(pos.x()), toScreenY(pos.y())); }
    inline QLine toScreen(const QLineF &line) const { return QLine(toScreenX(line.x1()), toScreenY(line.y1()), toScreenX(line.x2()), toScreenY(line.y2())); }
    inline QRectF toScreen(const QRectF &rect) const { return QRectF(toScreenX(rect.x()), toScreenY(rect.y()), rect.width()*zoom(), rect.height()*zoom()); }
    float snap(float x) const;
    inline QPointF snap(const QPointF &p) const { return QPointF(snap(p.x()), snap(p.y())); }

    inline float toWorldX(const float x) const { return left() + x/zoom(); }
    inline float toWorldY(const float y) const { return top() + y/zoom(); }
    inline QPointF toWorld(const QPointF &pos) const { return QPointF(toWorldX(pos.x()), toWorldY(pos.y())); }
    inline QLine toWorld(const QLineF &line) const { return QLine(toWorldX(line.x1()), toWorldY(line.y1()), toWorldX(line.x2()), toWorldY(line.y2())); }
    inline QRectF toWorld(const QRectF &rect) const { return QRectF(toWorldX(rect.x()), toWorldY(rect.y()), rect.width()/zoom(), rect.height()/zoom()); }

    void drawLabelText(const QString &label, const int height);

    void setupPainterMode(PainterMode type, QPainter &painter);
    inline void setupPainterMode(PainterMode type){ setupPainterMode(type, painter); };

    // Drawing functions
    void drawAlias(const QPointF &center, const QBrush &bursh, const int radius, const QString &label = "");
    void drawGround(const QPointF &center, const QBrush &bursh, const int radius, const QString &label = "");
    void drawResistor(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");
    void drawCapacitor(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");
    void drawInductor(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");
    void drawBattery(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");
    void drawDCV(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");
    void drawDCI(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");

    void drawDipole(
        const ObjectType type,
        const QPointF &A,
        const QPointF &B,
        const QPen &pen,
        const QString &label = ""
    );

    void drawObject(
        const SharedObject &obj,
        const QPen &pen,
        const QBrush &brush
    );
};
