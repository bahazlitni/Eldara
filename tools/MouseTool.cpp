#include "MouseTool.h"
#include "App.h"
#include "objects/Object.h"
#include "Grid.h"

float MouseTool::x() const {return app->x(); }
float MouseTool::y() const {return app->y(); }
float MouseTool::dx() const {return app->dx(); }
float MouseTool::dy() const {return app->dy(); }
QPointF MouseTool::p() const {return app->p(); }
QPointF MouseTool::dp() const {return app->dp(); }

QPointF MouseTool::pDown(const Qt::MouseButton btn) const { return app->pDown(btn); }
QPointF MouseTool::pUp(const Qt::MouseButton btn) const { return app->pUp(btn); }

bool MouseTool::pressed(Qt::Key key) const { return app->pressed(key); }
bool MouseTool::shift() const { return app->shift(); }
bool MouseTool::ctrl() const { return app->ctrl(); }
bool MouseTool::alt() const { return app->alt(); }

QPointF MouseTool::worldP() const {
    return app->worldP();
}
QPointF MouseTool::clientT() const {
    return app->grid.toScreen(t());
}

bool MouseTool::isHovered(const SharedObject &obj) const {
    if(_hoveredObject.expired()) return false;
    return obj == _hoveredObject.lock();
}

void MouseTool::drawIndicators(QPainter *painter){
    app->grid.setupPainterMode(INDICATOR_STROKE, *painter);
    painter->drawPath(_indicators);
}
