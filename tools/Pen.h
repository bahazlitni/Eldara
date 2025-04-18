#pragma once
#include "utils/Globals.h"

#include <QCursor>

#include "Grid.h"
#include "MouseTool.h"
#include "objects/BCPath.h"

class Node;
class Dipole;
class EventsManager;
class Alias;
class WorldPoint;

class Pen: public MouseTool {
    Q_OBJECT

protected:
    enum Mode {
       Prohibited,
       AliasAndDipole,
       AliasOnly,
       DipoleOnly,
       AliasSplit,
       NormalSplit,
       OverSplit,
       SwitchPrevious
    };

public:
    Pen(Scene *scene);

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

    inline bool makingAlias() const {
        return type == ObjectType::Alias || type == ObjectType::Ground;
    }
    inline bool onControl() const {
        return _state == INITIAL_CONTROLLING || _state == CONTROLLING;
    }

    void setType(ObjectType type);

    inline bool willDraw(const SharedObject &obj) const override { return isHovered(obj); }
    inline bool willMakeGround() const { return shift() || type == ObjectType::Ground; }

    void deepRemoval(const SharedObject &obj) override;

    inline BCPath path() const { return _path; }

    inline QBrush brush() const {
        QBrush b;
        b.setStyle(Qt::BrushStyle::SolidPattern);
        b.setColor(fillColor());
        return b;
    }

    inline QPen pen() const {
        QPen p;
        p.setStyle(Qt::PenStyle::SolidLine);
        p.setWidth(strokeWidth());
        p.setColor(strokeColor());
        return p;
    }


// Settings
protected:
    struct Settings {
        bool   showLabel                ;
        bool   allowSplitting           ;
        bool   allowOnClickColoring     ;
        double defaultResistance        ;
        double defaultCapacitance       ;
        double defaultInductance        ;
        double defaultBatteryVoltage    ;
        double defaultDCVoltage         ;
        double defaultIntensity         ;
        double defaultQuantity          ;
        int    radius                   ;
        int    strokeWidth              ;
        QColor strokeColor              ;
        QColor fillColor                ;
    };

    Settings settings;

public:
    inline void setShowLabel             ( const bool   v ){ settings.showLabel = v; }
    inline void setAllowSplitting        ( const bool   v ){ settings.allowSplitting = v; }
    inline void setAllowOnClickColoring  ( const bool   v ){ settings.allowOnClickColoring = v; }
    inline void setDefaultResistance     ( const double v ){ settings.defaultResistance = v; }
    inline void setDefaultCapacitance    ( const double v ){ settings.defaultCapacitance = v; }
    inline void setDefaultInductance     ( const double v ){ settings.defaultInductance = v; }
    inline void setDefaultBatteryVoltage ( const double v ){ settings.defaultBatteryVoltage = v; }
    inline void setDefaultDCVoltage      ( const double v ){ settings.defaultDCVoltage = v; }
    inline void setDefaultIntensity      ( const double v ){ settings.defaultIntensity = v; }
    inline void setDefaultQuantity       ( const double v ){ settings.defaultQuantity = v; }
    inline void setRadius                ( const int    v ){ settings.radius = v; }
    inline void setStrokeWidth           ( const int    v ){ settings.strokeWidth = v; }
    inline void setStrokeColor           ( const QColor &v){ settings.strokeColor = v; }
    inline void setFillColor             ( const QColor &v){ settings.fillColor = v; }

    inline bool showLabel               () const { return settings.showLabel; }
    inline bool allowSplitting          () const { return settings.allowSplitting; }
    inline bool allowOnClickColoring    () const { return settings.allowOnClickColoring; }
    inline double defaultResistance     () const { return settings.defaultResistance; }
    inline double defaultCapacitance    () const { return settings.defaultCapacitance; }
    inline double defaultInductance     () const { return settings.defaultInductance; }
    inline double defaultBatteryVoltage () const { return settings.defaultBatteryVoltage; }
    inline double defaultDCVoltage      () const { return settings.defaultDCVoltage; }
    inline double defaultIntensity      () const { return settings.defaultIntensity; }
    inline double defaultQuantity       () const { return settings.defaultQuantity; }
    inline int radius                   () const { return settings.radius; }
    inline int strokeWidth              () const { return settings.strokeWidth; }
    inline QColor strokeColor           () const { return settings.strokeColor; }
    inline QColor fillColor             () const { return settings.fillColor; }

signals:
    void dataChanged();

protected:
    void setState(const ToolState state) override;

    void updateHover();
    void analyze();
    void construct();

    SharedDipole hoveredDipole();
    SharedAlias hoveredAlias();

    Mode mode;
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
