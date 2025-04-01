#include "App.h"
#include "objects/Alias.h"
#include "objects/Resistor.h"
#include "widgets/tabs/VariablesTab.h"

App::App(QWidget *parent):
    QWidget(parent),
    varManager(VariablesManager(this)),
    grid(Grid(this)),
    pen(Pen(this)),
    selector(Selector(this)),
    grabber(Grabber(this)),
    timeline(Timeline())
{
    mouse = &selector;
    mouse->init();
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

void App::addAlias(const SharedAlias &alias){ aliases.insert(alias->id(), alias); }
void App::removeAlias(const SharedAlias &alias){ aliases.remove(alias->id()); }

void App::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    grid.updateSize();
    update();
}
void App::mouseMoveEvent(QMouseEvent *event){
    _dp = event->pos() - _p;
    _p = event->pos();
    _worldP = grid.toWorld(_p);
    const QPointF prevTarget = mouse->t();
    mouse->updateMovement();
    mouse->setDt(mouse->t() - prevTarget);
    mouse->move();
    update();
}
void App::wheelEvent(QWheelEvent *event){
    grid.setZoom(event->position(), event->angleDelta().y() / 120);
    mouse->move();
    update();
}
void App::mousePressEvent(QMouseEvent *event){
    const Qt::MouseButton btn = event->button();
    mouse->set_tDown(btn, mouse->t());
    set_pDown(btn, _p);
    switch(btn){
    case Qt::LeftButton: mouse->downL(); break;
    case Qt::MiddleButton: mouse->downM(); break;
    case Qt::RightButton: mouse->downR(); break;
    default: break;
    }
    update();
}
void App::mouseReleaseEvent(QMouseEvent *event){
    const Qt::MouseButton btn = event->button();
    mouse->set_tUp(btn, mouse->t());
    set_pUp(btn, _p);
    switch(btn){
    case Qt::LeftButton: mouse->upL(); break;
    case Qt::MiddleButton: mouse->upM(); break;
    case Qt::RightButton: mouse->upR(); break;
    default: break;
    }
    update();
}

void App::keyPressEvent(QKeyEvent *event){
    const Qt::Key key = static_cast<Qt::Key>(event->key());
    keys.insert(key);

    if(event->isAutoRepeat()) mouse->keyPress(key);
    else mouse->keyDown(key);

    if (ctrl()) {
        switch (key) {
        case Qt::Key_Z: undo(); break;
        case Qt::Key_Y: redo(); break;
        default: break;
        }
    }
    update();
}


void App::keyReleaseEvent(QKeyEvent *event){
    const Qt::Key key = (Qt::Key) event->key();
    keys.remove(key);

    if (event->isAutoRepeat()) return;
    if(prevMouse && key == returningKey) {
        mouse = prevMouse;
        mouse->setCursor();
        prevMouse = nullptr;
        return;
    }
    mouse->keyUp(key);
    update();
}
void App::mouseDoubleClickEvent(QMouseEvent *event){
    const Qt::MouseButton btn = event->button();
    mouse->set_tUp(btn, mouse->t());
    set_pUp(btn, mouse->p());
    switch(btn){
    case Qt::LeftButton:
        mouse->downL();
        return mouse->doubleL();
    case Qt::MiddleButton:
        mouse->downM();
        return mouse->doubleM();
    case Qt::RightButton:
        mouse->downR();
        return mouse->doubleR();
    default: return;
    }
    update();
}


// DISPLAY
void App::paintEvent(QPaintEvent *event){ grid.render(event); }

void App::setMouse(MouseTool *m){
    if(m == mouse) return;
    mouse->end();
    m->init();
    mouse = m;
}
void App::setTempMouse(MouseTool *m, const Qt::Key key){
    prevMouse = mouse;
    mouse = m;
    mouse->init();
    returningKey = key;
}

void App::undo(){ timeline.undo(); grid.updateVisibility(); }
void App::redo(){ timeline.redo(); grid.updateVisibility(); }
void App::execute(std::unique_ptr<Command> cmd){
    timeline.execute(std::move(cmd));
    grid.updateVisibility();
}

void App::deepRemoval(const SharedObject &obj){
    selector.deepRemoval(obj);
    pen.deepRemoval(obj);
    grabber.deepRemoval(obj);
    grid.deepRemoval(obj);
    if(obj->category() == _NODE) {
        const auto &alias = std::static_pointer_cast<Alias>(obj);
        removeAlias(alias);
    }
}
