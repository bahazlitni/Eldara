#pragma once
#include <QColor>
#include <QLineF>
#include <QPainter>
#include <QPainterPath>
#include <QPointF>
#include <QWidget>
#include <QHash>
#include <QTransform>
#include <QtMath>

#include <QGraphicsColorizeEffect>
#include <QSvgRenderer>

#include "utils/Types.h"

enum PainterMode {
    CONSTRUCTION_PREVIEW = 0,
    IMAGINARY_LINES = 1,
    SELECTION_BOX = 2,
    WORLDOBJECT_FILL = 3,
    WORLDOBJECT_STROKE = 4,
    INDICATOR_STROKE = 5,
    SELECTED_OBJECT_STROKE = 6,
    GRID_STROKE = 7,
    MERGE_INDICATOR = 8
};

int constexpr DEFAULT_ZOOM_INDEX = 5;
int constexpr NUMBER_OF_ZOOM_STATES = 14;
int constexpr MAJ_ZOOM_INDEX = NUMBER_OF_ZOOM_STATES-1;
float const ZOOM_STATES[NUMBER_OF_ZOOM_STATES] = {
    0.2f, 0.5f, 0.8f, 0.9f, 1.0f, 1.1f, 1.5f,
    2.0f, 2.5f, 3.0f, 4.0f, 5.0f, 6.0f, 8.0f
};

const QColor WORLD_SPACE_BACKGROUND_COLOR = QColor(28, 30, 29);
const QColor WORLD_OBJECT_HOVER_COLOR = QColor(80, 180, 255);
const QColor WORLD_OBJECT_SELECTION_COLOR = QColor(40, 140, 255);


const QColor WORLD_SELECTION_BOX_FILL_COLOR = QColor(159, 200, 255, 50);
const QColor WORLD_SELECTION_BOX_OUTLINE_COLOR = QColor(159, 200, 255);
const QColor WORLD_INDICATOR_COLOR = QColor(255, 80, 40);

const QColor MERGE_INDICATOR_STROKE_COLOR = QColor(41, 121, 255);

const QColor WORLD_CONSTRUCTION_ALLOWED = QColor(80, 255, 40);
const QColor WORLD_CONSTRUCTION_PROHIBITED = QColor(255, 20, 40);

const QColor BC_CONTROL_POINT_COLOR = QColor(41, 121, 255);
const QColor BC_POINT_COLOR = QColor(245, 245, 245);

const QColor GRID_STROKE_COLOR = QColor(255, 255, 255, 10);

constexpr int STROKE_WIDTH = 2;

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

    void addVisible(const LockedSelection &selection){ for(const auto &obj : selection) addVisible(obj); }
    void removeVisible(const LockedSelection &selection){ for(const auto &obj : selection) removeVisible(obj); }
    void updateVisibility(const LockedSelection &selection){ for(const auto &obj : selection) updateVisibility(obj); }

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

    void drawLabelText(const QString &LABEL, const int HEIGHT);

    void setupPainterMode(PainterMode type, QPainter &painter);
    void setupDipolePainter(const QColor &color, QPainter &painter);
    void setupDipoleDashedPainter(const QColor &color, QPainter &painter);
    void setupNodePainter(const QColor &color, QPainter &painter);

    void setupPainterMode(PainterMode type){ setupPainterMode(type, painter); };
    void setupDipolePainter(const QColor &color){ setupDipolePainter(color, painter); };
    void setupDipoleDashedPainter(const QColor &color){ setupDipoleDashedPainter(color, painter); };
    void setupNodePainter(const QColor &color){ setupNodePainter(color, painter); };

    // Drawing functions
    void drawBCControlPoint(const QPointF &p, const QColor &strokeColor);
    void drawBCPoint(const QPointF &p, const QColor &strokeColor);
    void drawBCControlLine(const QPointF &point, const QPointF &controlPoint, const QColor &strokeColor);
    void drawBCPathEdits(const BCPath &bcp);
    QPainterPath constructPath(const BCPath &bcp);
    void drawBCPath(const BCPath &bcp, const QColor &color);
    void drawBCPath(const BCPath &bcp, const QColor &color, const QPointF &preview);

    void drawNode(const QPointF &center, const QColor &color, int radius, const QString &label);
    void drawGround(const QPointF &center, const QColor &color, int radius, const QString &label);
    void drawResistor(const QPointF &A, const QPointF &B, const QColor &color, const QString &label);
    void drawCapacitor(const QPointF &A, const QPointF &B, const QColor &color, const QString &label);
    void drawInductor(const QPointF &A, const QPointF &B, const QColor &color, const QString &label);
    void drawBattery(const QPointF &A, const QPointF &B, const QColor &color, const QString &label);
    void drawDC_VoltageGenerator(const QPointF &A, const QPointF &B, const QColor &color, const QString &label);
    void drawDC_CurrentGenerator(const QPointF &A, const QPointF &B, const QColor &color, const QString &label);

    void drawNode(const QPointF &center, const QColor &color, int radius);
    void drawGround(const QPointF &center, const QColor &color, int radius);
    void drawResistor(const QPointF &A, const QPointF &B, const QColor &color);
    void drawCapacitor(const QPointF &A, const QPointF &B, const QColor &color);
    void drawInductor(const QPointF &A, const QPointF &B, const QColor &color);
    void drawBattery(const QPointF &A, const QPointF &B, const QColor &color);
    void drawDC_VoltageGenerator(const QPointF &A, const QPointF &B, const QColor &color);
    void drawDC_CurrentGenerator(const QPointF &A, const QPointF &B, const QColor &color);


    void drawDipole(const ObjectType type, const QPointF &A, const QPointF &B, const QColor &color, const QString &label);
    void drawDipole(const ObjectType type, const QPointF &A, const QPointF &B, const QColor &color);

    void drawObject(const SharedObject &obj, const QColor &color);
};
