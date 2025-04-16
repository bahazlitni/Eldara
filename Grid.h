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

class App;
class BCPath;
class Grid: public QRectF {
private:
    App *app;
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
    static constexpr int TILE_SIZE = 10;

    Grid(App *app);
    ~Grid(){}

    bool getDirtyVisibleCheckFlagInitial() const { return dirtyVisibleCheckFlagInitial; }

    QSet<SharedAlias> visibleAliases;
    QSet<SharedDipole> visibleDipoles;

    void addVisible(const SharedAlias &alias){ visibleAliases.insert(alias); }
    void removeVisible(const SharedAlias &alias){ visibleAliases.remove(alias); }
    void updateVisibility(const SharedAlias &alias);

    void addVisible(const SharedDipole &dipole){ visibleDipoles.insert(dipole); }
    void removeVisible(const SharedDipole &dipole){ visibleDipoles.remove(dipole); }
    void updateVisibility(const SharedDipole &dipole);

    void addVisible(const SharedObject &obj);
    void removeVisible(const SharedObject &obj);
    void updateVisibility(const SharedObject &obj);

    void addVisible(const Selection &selection){ for(const auto &obj : selection) addVisible(obj); }
    void removeVisible(const Selection &selection){ for(const auto &obj : selection) removeVisible(obj); }
    void updateVisibility(const Selection &selection){ for(const auto &obj : selection) updateVisibility(obj); }

    void updateVisibility();
    void updateSize();
    void updateGridTile();

    void deepRemoval(const SharedObject &obj) { removeVisible(obj); }

    int tileSize() const { return Grid::TILE_SIZE; }

    float zoom() const { return ZOOM_STATES[zoomIndex]; };
    void setZoom(const QPointF &p, const int d);

    void render([[maybe_unused]] QPaintEvent *event);

    void moveTo(const float x, const float y){
        const QSizeF sz = size();
        setX(x);
        setY(y);
        setSize(sz);
        updateGridTile();
        updateVisibility();
    }
    void moveTo(const QPointF &p){ moveTo(p.x(), p.y()); }

    float toScreenX(const float x){ return (x - left())*zoom(); }
    float toScreenY(const float y){ return (y - top())*zoom(); }
    QPointF toScreen(const QPointF &pos){ return QPointF(toScreenX(pos.x()), toScreenY(pos.y())); }
    QLine toScreen(const QLineF &line){ return QLine(toScreenX(line.x1()), toScreenY(line.y1()), toScreenX(line.x2()), toScreenY(line.y2())); }
    QRectF toScreen(const QRectF &rect){ return QRectF(toScreenX(rect.x()), toScreenY(rect.y()), rect.width()*zoom(), rect.height()*zoom()); }
    float snap(float x) {
        if(x < 0){
            const float r = std::fmod(-x, Grid::TILE_SIZE);
            return x + ((2*r > Grid::TILE_SIZE ? -Grid::TILE_SIZE : 0) + r);
        }
        const float r = std::fmod(x, Grid::TILE_SIZE);
        return x + ((2*r > Grid::TILE_SIZE ? Grid::TILE_SIZE : 0) - r);
    }
    QPointF snap(const QPointF &p){ return QPointF(snap(p.x()), snap(p.y())); }

    float toWorldX(const float x){ return left() + x/zoom(); }
    float toWorldY(const float y){ return top() + y/zoom(); }
    QPointF toWorld(const QPointF &pos){ return QPointF(toWorldX(pos.x()), toWorldY(pos.y())); }
    QLine toWorld(const QLineF &line){ return QLine(toWorldX(line.x1()), toWorldY(line.y1()), toWorldX(line.x2()), toWorldY(line.y2())); }
    QRectF toWorld(const QRectF &rect){ return QRectF(toWorldX(rect.x()), toWorldY(rect.y()), rect.width()/zoom(), rect.height()/zoom()); }

    void drawLabelText(const QString &label, const int height);

    void setupPainterMode(PainterMode type, QPainter &painter);
    void setupPainterMode(PainterMode type){ setupPainterMode(type, painter); };

    // Drawing functions
    void drawAlias(const QPointF &center, const QBrush &bursh, const int radius, const QString &label = "");
    void drawGround(const QPointF &center, const QBrush &bursh, const int radius, const QString &label = "");
    void drawResistor(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");
    void drawCapacitor(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");
    void drawInductor(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");
    void drawBattery(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");
    void drawDC_VoltageGenerator(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");
    void drawDC_CurrentGenerator(const QPointF &A, const QPointF &B, const QPen &pen, const QString &label = "");

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
