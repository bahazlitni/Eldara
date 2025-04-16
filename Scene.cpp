#include "Scene.h"
#include "objects/Alias.h"
#include "objects/Resistor.h"
#include "widgets/tabs/VariablesTab.h"

Scene::Scene(QWidget *parent):
    QWidget(parent),
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

void Scene::addAlias(const SharedAlias &alias){ aliases.insert(alias->id(), alias); }
void Scene::removeAlias(const SharedAlias &alias){ aliases.remove(alias->id()); }

void Scene::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    grid.updateSize();
    update();
}
void Scene::mouseMoveEvent(QMouseEvent *event){
    _dp = event->pos() - _p;
    _p = event->pos();
    _worldP = grid.toWorld(_p);
    const QPointF prevTarget = mouse->t();
    mouse->updateMovement();
    mouse->setDt(mouse->t() - prevTarget);
    mouse->move();
    update();
}
void Scene::wheelEvent(QWheelEvent *event){
    grid.setZoom(event->position(), event->angleDelta().y() / 120);
    mouse->move();
    update();
}
void Scene::mousePressEvent(QMouseEvent *event){
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
void Scene::mouseReleaseEvent(QMouseEvent *event){
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

void Scene::keyPressEvent(QKeyEvent *event){
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


void Scene::keyReleaseEvent(QKeyEvent *event){
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
void Scene::mouseDoubleClickEvent(QMouseEvent *event){
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
void Scene::paintEvent(QPaintEvent *event){ grid.render(event); }

void Scene::setMouse(MouseTool *m){
    if(m == mouse) return;
    mouse->end();
    m->init();
    mouse = m;
}
void Scene::setTempMouse(MouseTool *m, const Qt::Key key){
    prevMouse = mouse;
    mouse = m;
    mouse->init();
    returningKey = key;
}

void Scene::undo(){ timeline.undo(); grid.updateVisibility(); }
void Scene::redo(){ timeline.redo(); grid.updateVisibility(); }
void Scene::execute(std::unique_ptr<Command> cmd){
    timeline.execute(std::move(cmd));
    grid.updateVisibility();
}

void Scene::deepRemoval(const SharedObject &obj){
    selector.deepRemoval(obj);
    pen.deepRemoval(obj);
    grabber.deepRemoval(obj);
    grid.deepRemoval(obj);
    if(obj->category() == ObjectCategory::Node) {
        const auto &alias = std::static_pointer_cast<Alias>(obj);
        removeAlias(alias);
    }
}
