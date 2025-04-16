#pragma once
#include "utils/Globals.h"

#include "Grid.h"
#include "MouseTool.h"
#include "objects/BCPath.h"

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

    QVariant getAttr(const Attr attr) const override;
    void setAttr(const Attr attr, const QVariant &v) override;

    bool onControl() const { return _state == INITIAL_CONTROLLING || _state == CONTROLLING; };

    inline QPen pen() const { return _pen; }
    inline QBrush brush() const { return _brush; }
    inline QColor fillColor() const { return _brush.color(); }
    inline QColor strokeColor() const { return _pen.color(); }
    inline int strokeWidth() const { return _pen.width(); }

    inline void setPen(const QPen &pen) { _pen = pen; }
    inline void setBrush(const QBrush &brush) { _brush = brush; }
    inline void setFillColor(const QColor &color) { _brush.setColor(color); }
    inline void setStrokeColor(const QColor &color) { _pen.setColor(color); }
    inline void setStrokeWidth(const int w) { _pen.setWidth(w); }

    inline int radius() const { return _radius; }
    inline void setRadius(const int r) { _radius = r; }

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

    SharedDipole hoveredDipole();
    SharedAlias hoveredAlias();

    QPen _pen;
    QBrush _brush;
    int _radius;

    ConstructionMode mode;

    BCPath _path;

    const QCursor penCursor, constructingCursor, constructingPlusCursor;

    SharedAlias MakeAlias(const QPointF &p);
    SharedDipole MakeDipole(const SharedAlias &A, const SharedAlias &B){ return MakeDipole(A, B, type); }
    SharedDipole MakeDipole(const SharedAlias &A, const SharedAlias &B, ObjectType type);

    SharedWorldPoint MakeBCPoint(const QPointF &p);
    SharedWorldPoint MakeBCControlPoint(const QPointF &p);

    void stackBCPoint(const QPointF &p);
    void stackBCPoint(const SharedPoint &p);

    ObjectType type;

    void drawDipole(
        const ObjectType type,
        const QPointF &A,
        const QPointF &B,
        const QPen &pen,
        const QString &label = ""
    );

    void drawAliasPreview(const QColor &color);
    void drawDipolePreview(const QColor &color);
    void drawSplittedPreview(const QColor &color);
    void drawResultantPreview(const QColor &color);
    void drawHoveredAlias(){ drawHoveredAlias(Palette::HOVER); }
    void drawHoveredDipole(){ drawHoveredDipole(Palette::HOVER); }
    void drawHoveredAlias(const QColor &color);
    void drawHoveredDipole(const QColor &color);
};
