#include "CustomDoubleSpinBox.h"
#include <QPainter>
#include <QMouseEvent>
#include <QCursor>

CustomDoubleSpinBox::CustomDoubleSpinBox(QWidget *parent): QDoubleSpinBox(parent),
    pressTimer(QTimer(this)),
    upArrow(QPixmap(":/assets/icons/arrow_up.svg")),
    downArrow(QPixmap(":/assets/icons/arrow_down.svg"))
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setButtonSymbols(QAbstractSpinBox::NoButtons);
    connect(&pressTimer, &QTimer::timeout, this, &CustomDoubleSpinBox::crementing);
    setMinimumWidth(50);
}

void CustomDoubleSpinBox::paintEvent(QPaintEvent *event) {
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


void CustomDoubleSpinBox::crementing() {
    if(isIncrementing) increment();
    else decrement();
    pressTimer.start(50);
}

void CustomDoubleSpinBox::mousePressEvent(QMouseEvent *event) {
    if (upRect.contains(event->pos())) {
        isIncrementing = true;
        increment();
        pressTimer.start(500);
    } else if (downRect.contains(event->pos())) {
        isIncrementing = false;
        decrement();
        pressTimer.start(500);
    }
    QDoubleSpinBox::mousePressEvent(event);
}

void CustomDoubleSpinBox::mouseReleaseEvent(QMouseEvent *event) {
    pressTimer.stop();
    QDoubleSpinBox::mouseReleaseEvent(event);
}


void CustomDoubleSpinBox::mouseMoveEvent(QMouseEvent *event) {
    bool newHoverUp = upRect.contains(event->pos());
    bool newHoverDown = downRect.contains(event->pos());

    if (newHoverUp != hoverUp || newHoverDown != hoverDown) {
        hoverUp = newHoverUp;
        hoverDown = newHoverDown;
        update();
    }

    if (hoverUp || hoverDown) setCursor(Qt::PointingHandCursor);
}

void CustomDoubleSpinBox::leaveEvent([[maybe_unused]] QEvent *event) {
    hoverUp = hoverDown = false;
    update();
}
