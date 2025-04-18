#include "MouseTool.h"
#include "Scene.h"
#include "Grid.h"

float MouseTool::x() const {return scene->x(); }
float MouseTool::y() const {return scene->y(); }
float MouseTool::dx() const {return scene->dx(); }
float MouseTool::dy() const {return scene->dy(); }
QPointF MouseTool::p() const {return scene->p(); }
QPointF MouseTool::dp() const {return scene->dp(); }

QPointF MouseTool::pDown(const Qt::MouseButton btn) const { return scene->pDown(btn); }
QPointF MouseTool::pUp(const Qt::MouseButton btn) const { return scene->pUp(btn); }

bool MouseTool::pressed(Qt::Key key) const { return scene->pressed(key); }
bool MouseTool::shift() const { return scene->shift(); }
bool MouseTool::ctrl() const { return scene->ctrl(); }
bool MouseTool::alt() const { return scene->alt(); }

QPointF MouseTool::worldP() const {
    return scene->worldP();
}
QPointF MouseTool::clientT() const {
    return scene->grid.toScreen(t());
}

bool MouseTool::isHovered(const SharedObject &obj) const {
    if(_hoveredObject.expired()) return false;
    return obj == _hoveredObject.lock();
}

void MouseTool::drawIndicators(QPainter *painter){
    scene->grid.setupPainterMode(Grid::INDICATOR_STROKE, *painter);
    painter->drawPath(_indicators);
}
