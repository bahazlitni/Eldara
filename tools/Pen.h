#pragma once
#include <QCursor>
#include "Grid.h"
#include "MouseTool.h"
#include "objects/BCPath.h"
#include "utils/Types.h"

enum ConstructionMode {
    PROHIBITED = -1,
    ALIAS_AND_DIPOLE = 0,
    ALIAS_ONLY = 1,
    DIPOLE_ONLY = 2,
    ALIAS_SPLIT = 3,
    NORMAL_SPLIT = 4,
    OVER_SPLIT = 5,
    SWITCH_PREVIOUS = 6
};

const QColor NOT_ALLOWED_CONSTRUCTION_COLOR = QColor(255, 76, 76);

class ComboCommand;

class Command;
class Node;
class Dipole;
class EventsManager;
class Alias;
class WorldPoint;

class Pen: public MouseTool {
    Q_OBJECT

public:
    Pen(App *app);

    SharedAlias previous = nullptr;

    ToolState initState() const override { return PEN; }
    void end() override;
    void move() override;
    void downL() override;
    void downR() override;
    void upL() override;
    void upR() override;
    void updateMovement() override;
    void keyDown([[maybe_unused]] Qt::Key key) override;
    void keyPress([[maybe_unused]] Qt::Key key) override;
    void keyUp([[maybe_unused]] Qt::Key key) override;
    void draw(QPainter *painter) override;
    void setCursor(const ToolState state) override;

    QString dataString(const QString &key) const override;
    void setData(const QString &key, const QString &value) override;

    bool onControl() const { return _state == INITIAL_CONTROLLING || _state == CONTROLLING; };

    QColor fillColor() const { return _fillColor; }
    QColor strokeColor() const { return _strokeColor; }
    int dotRadius() const { return _dotRadius; }

    BCPath path() const { return _path; }
    void setType(ObjectType type);

    bool willDraw(const SharedObject &obj) const override { return isHovered(obj); }

    void deepRemoval(const SharedObject &obj) override;

    bool showLabel() const { return _showLabel; }
    void setShowLabel(const bool b) { _showLabel = b; }



signals:
    void dataChanged();

protected:
    bool _showLabel;

    void setState(const ToolState state) override;

    void updateHover();
    void analyze();
    void construct();

    void drawAliasPreview(const QColor &color);
    void drawDipolePreview(const QColor &color);
    void drawSplittedPreview(const QColor &color);
    void drawResultantPreview(const QColor &color);
    void drawHoveredAlias(){ drawHoveredAlias(WORLD_OBJECT_HOVER_COLOR); }
    void drawHoveredDipole(){ drawHoveredDipole(WORLD_OBJECT_HOVER_COLOR); }
    void drawHoveredAlias(const QColor &color);
    void drawHoveredDipole(const QColor &color);

    SharedDipole hoveredDipole();
    SharedAlias hoveredAlias();

    QColor _fillColor = QColor(222,222,222);
    QColor _strokeColor = QColor(188,188,188);
    int _dotRadius;

    ConstructionMode mode;

    BCPath _path;

    const QCursor penCursor, constructingCursor, constructingPlusCursor;

    QString address();
    SharedAlias MakeAlias(const QPointF &p);
    SharedDipole MakeDipole(const SharedAlias &A, const SharedAlias &B){ return MakeDipole(A, B, type); }
    SharedDipole MakeDipole(const SharedAlias &A, const SharedAlias &B, ObjectType type);

    SharedWorldPoint MakeBCPoint(const QPointF &p);
    SharedWorldPoint MakeBCControlPoint(const QPointF &p);

    void stackBCPoint(const QPointF &p);
    void stackBCPoint(const SharedPoint &p);

    ObjectType type;

    void drawDipole(const ObjectType type, const QPointF &A, const QPointF &B, const QColor &color);
    void drawDipole(const ObjectType type, const QPointF &A, const QPointF &B, const QColor &color, const QString &label);
};
