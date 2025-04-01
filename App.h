#pragma once
#include <QVector>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QStack>
#include <atomic>
#include <QMap>
#include <cstdint>
#include "utils/Types.h"

#include "Grid.h"
#include "commands/Timeline.h"
#include "tools/MouseTool.h"
#include "tools/Pen.h"
#include "tools/Selector.h"
#include "tools/Grabber.h"

#include "VariablesManager.h"

class VariablesTab;

class App : public QWidget {
    Q_OBJECT

public:
    App(QWidget *parent = nullptr);
    ~App() = default;

    VariablesManager varManager;
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
    Timeline timeline;

    std::atomic<uint64_t> trackid = 0;
    std::atomic<uint64_t> trackAddress = 0;

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
