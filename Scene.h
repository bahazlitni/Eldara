#pragma once
#include "utils/Globals.h"

#include "Grid.h"
#include "commands/Timeline.h"
#include "tools/MouseTool.h"
#include "tools/Pen.h"
#include "tools/Selector.h"
#include "tools/Grabber.h"

class VariablesTab;
class Scene : public QWidget {
    Q_OBJECT

public:
    Scene(QWidget *parent = nullptr);
    ~Scene() = default;

    Grid grid;
    Pen pen;
    Selector selector;
    Grabber grabber;
    MouseTool *mouse;

    AliasMap aliases;

    void addAlias(const SharedAlias &alias);
    void removeAlias(const SharedAlias &alias);
    void deepRemoval(const SharedObject &obj);

    void zoom(int degrees);
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    void setMouse(MouseTool *m);
    void setTempMouse(MouseTool *m, const Qt::Key key);

    void undo();
    void redo();
    void execute(std::unique_ptr<Command> cmd);

    bool pressed(Qt::Key key) const { return keys.contains(key); }
    bool shift() const { return pressed(Qt::Key_Shift); };
    bool ctrl() const { return pressed(Qt::Key_Control); };
    bool alt() const { return pressed(Qt::Key_Alt) || pressed(Qt::Key_AltGr); };

    float x() const { return _p.x(); }
    float y() const { return _p.y(); }
    float dx() const { return _dp.x(); }
    float dy() const { return _dp.y(); }
    QPointF p() const { return _p; }
    QPointF dp() const { return _dp; }
    QPointF worldP() const { return _worldP; }
    QPointF pDown(const Qt::MouseButton btn) const {
        int i = 0;
        switch(btn){
        case Qt::MiddleButton: i = 1; break;
        case Qt::RightButton: i = 2; break;
        default: break;
        }
        return _record[0][i];
    }
    QPointF pUp(const Qt::MouseButton btn) const {
        int i = 0;
        switch(btn){
        case Qt::MiddleButton: i = 1; break;
        case Qt::RightButton: i = 2; break;
        default: break;
        }
        return _record[1][i];
    }

    uint64_t id(){ return trackid++; }
    QString address(){
        trackAddress++;
        QString s;
        uint64_t n = trackAddress;
        while(n > 0) {
            uint64_t remainder = (n - 1) % 26;
            s = static_cast<char>('A' + remainder) + s;
            n = (n - 1) / 26;
        }
        return s;
    }


private:
    struct Settings {
        bool   snapPosition             ;
        bool   allowMerging             ;
        bool   showGrid                 ;
        bool   displayRawValues         ;
        int    tileSize                 ;
        QColor backgroundColor          ;
        QColor gridStrokeColor          ;
    };

    Settings settings;

public:
    inline void setSnapPosition     ( const bool v   ) { settings.snapPosition     = v; }
    inline void setAllowMerging     ( const bool v   ) { settings.allowMerging     = v; }
    inline void setShowGrid         ( const bool v   ) { settings.showGrid         = v; }
    inline void setDisplayRawValues ( const bool v   ) { settings.displayRawValues = v; }
    inline void setTileSize         ( const int  v   ) {
        settings.tileSize = v;
        grid.updateGridTile();
    }
    inline void setBackgroundColor  ( const QColor &v) { settings.backgroundColor  = v; }
    inline void setGridStrokeColor  ( const QColor &v) {
        settings.gridStrokeColor = v;
        grid.updateGridTile();
    }

    inline bool   snapPosition     () const { return settings.snapPosition    ; }
    inline bool   allowMerging     () const { return settings.allowMerging    ; }
    inline bool   showGrid         () const { return settings.showGrid        ; }
    inline bool   displayRawValues () const { return settings.displayRawValues; }
    inline int    tileSize         () const { return settings.tileSize        ; }
    inline QColor backgroundColor  () const { return settings.backgroundColor ; }
    inline QColor gridStrokeColor  () const { return settings.gridStrokeColor ; }


private:
    Timeline timeline;

    IDTracker trackid = 0;
    IDTracker trackAddress = 0;

    MouseTool *prevMouse = nullptr;
    Qt::Key returningKey;

    void set_pDown(const Qt::MouseButton btn, const QPointF &p){
        int i = 0;
        switch(btn){
        case Qt::MiddleButton: i = 1; break;
        case Qt::RightButton: i = 2; break;
        default: break;
        }
        _record[0][i] = p;
    }
    void set_pUp(const Qt::MouseButton btn, const QPointF &p){
        int i = 0;
        switch(btn){
        case Qt::MiddleButton: i = 1; break;
        case Qt::RightButton: i = 2; break;
        default: break;
        }
        _record[1][i] = p;
    }

    QSet<Qt::Key> keys;
    QPointF _record[2][3];
    QPointF _p, _dp, _worldP;
};
