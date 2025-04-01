#include "CustomSpinBox.h"
#include <QPainter>
#include <QMouseEvent>
#include <QCursor>

CustomSpinBox::CustomSpinBox(QWidget *parent): QSpinBox(parent),
    pressTimer(QTimer(this)),
    upArrow(QPixmap(":/assets/icons/arrow_up.svg")),
    downArrow(QPixmap(":/assets/icons/arrow_down.svg"))
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setButtonSymbols(QAbstractSpinBox::NoButtons);
    connect(&pressTimer, &QTimer::timeout, this, &CustomSpinBox::crementing);
    setMinimumWidth(50);
}

void CustomSpinBox::paintEvent(QPaintEvent *event) {
    QAbstractSpinBox::paintEvent(event);

    QPainter painter(this);

    upRect = QRect(width() - 2 * buttonWidth, 0, buttonWidth, height());
    downRect = QRect(width() - buttonWidth, 0, buttonWidth, height());

    // Draw up arrow
    if (hoverUp) {
        painter.fillRect(upRect, buttonHoverColor);
    }
    painter.drawPixmap(upRect.center() - QPoint(upArrow.width() / 2, upArrow.height() / 2), upArrow);

    if (hoverDown) {
        painter.fillRect(downRect, buttonHoverColor);
    }
    painter.drawPixmap(downRect.center() - QPoint(downArrow.width() / 2, downArrow.height() / 2), downArrow);
    setMouseTracking(true);
}


void CustomSpinBox::crementing() {
    if(isIncrementing) increment();
    else decrement();
    pressTimer.start(50);
}

void CustomSpinBox::mousePressEvent(QMouseEvent *event) {
    if (upRect.contains(event->pos())) {
        isIncrementing = true;
        increment();
        pressTimer.start(500);
    } else if (downRect.contains(event->pos())) {
        isIncrementing = false;
        decrement();
        pressTimer.start(500);
    }
    QSpinBox::mousePressEvent(event);
}

void CustomSpinBox::mouseReleaseEvent(QMouseEvent *event) {
    pressTimer.stop();
    QSpinBox::mouseReleaseEvent(event);
}


void CustomSpinBox::mouseMoveEvent(QMouseEvent *event) {
    bool newHoverUp = upRect.contains(event->pos());
    bool newHoverDown = downRect.contains(event->pos());

    if (newHoverUp != hoverUp || newHoverDown != hoverDown) {
        hoverUp = newHoverUp;
        hoverDown = newHoverDown;
        update();
    }

    if (hoverUp || hoverDown) setCursor(Qt::PointingHandCursor);
}

void CustomSpinBox::leaveEvent([[maybe_unused]] QEvent *event) {
    hoverUp = hoverDown = false;
    update();
}
