#pragma once
#include "utils/Globals.h"
#include "MouseTool.h"

#include <QMap>
#include <QPointF>
#include <QSet>
#include <QCursor>

class Scene;

class Selector: public MouseTool {
    Q_OBJECT

public:
    Selector(Scene *scene);

    ToolState initState() const override { return SELECTOR; }
    void end() override;
    void move() override;
    void downL() override;
    void upL() override;
    void updateMovement() override;
    void keyDown(Qt::Key key) override;
    void keyPress(Qt::Key key) override;
    void keyUp(Qt::Key key) override;
    void draw(QPainter *painter) override;
    void setCursor(const ToolState state) override;

    void deleteSelection();

    bool isEmpty(){ return _selection.isEmpty(); }
    bool isSolo(){ return _selection.size() == 1; }

    int count() const { return _selection.size(); };
    int count(ObjectType type) const noexcept;
    int count(ObjectCategory cat) const noexcept;
    SharedObject first(ObjectType type) const noexcept;
    SharedObject first(ObjectCategory cat) const noexcept;

    Selection filter(ObjectType type) const noexcept;
    Selection filter(ObjectCategory cat) const noexcept;

    void add(const SharedObject &obj){ _selection.insert(obj); }
    void remove(const SharedObject &obj){ _selection.remove(obj); }
    bool contains(const SharedObject &obj) const { return _selection.contains(obj); };

    bool isSelected(const SharedObject &obj) const { return contains(obj); }
    bool willDraw(const SharedObject &obj) const override { return isHovered(obj) || isSelected(obj); }

    void selectInsideBox();

    void select(const Selection &selection){
        _selection.unite(selection);
        emit selectionChanged();
    }
    void unselect(const Selection &selection){
        _selection.subtract(selection);
        emit selectionChanged();
    }

    void deepRemoval(const SharedObject &obj) override;

    Selection selection() const { return _selection; }

signals:
    void selectionChanged();
    void selectionDragged();

protected:

    void setState(const ToolState state) override;

    void translate(const QPointF &dp);
    void executeDrag();
    void calcMergeMap();
    void endMovementMap();
    void initMovementMap();

    MergeMap mergeMap;
    SharedObject draggingObject;
    Selection deepSelection;
    Selection toCopySelection;
    Selection _selection;
    MovementMap movementMap;

    QRectF box;

    const QCursor selectorCursor, draggingCursor, plusCursor, minusCursor;

    bool initKeyDrag = false;
    QPointF unitDisplacement(const Qt::Key key);


    void drawObject(const SharedObject &obj, const QColor &color);
};
