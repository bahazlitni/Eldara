#pragma once
#include "MouseTool.h"

#include <QPointF>
#include <QCursor>

class Scene;
class Grabber: public MouseTool {
    Q_OBJECT

public:
    Grabber(Scene *scene): MouseTool(scene),
        grabberCursor(QCursor(QPixmap(":assets/cursor/grab.png"))),
        grabbingCursor(QCursor(QPixmap(":assets/cursor/grabbing.png")))
    {}

    ToolState initState() const override { return GRABBER; }
    void move() override;
    void downL() override;
    void upL() override;

protected:
    void setState(const ToolState state) override;

    const QCursor grabberCursor, grabbingCursor;
    QPointF origin;
};
