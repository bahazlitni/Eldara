#pragma once
#include <QDoubleSpinBox>
#include <QPixmap>
#include <QTimer>

class CustomDoubleSpinBox: public QDoubleSpinBox {
    Q_OBJECT
private:
    QTimer pressTimer;
    void crementing();
    bool isIncrementing = false;

public:
    explicit CustomDoubleSpinBox(QWidget *parent = nullptr);

    const int buttonWidth = 15;
    const QColor buttonHoverColor = QColor(255, 255, 255, 25);
    virtual void increment(){ stepUp(); }
    virtual void decrement(){ stepDown(); }

    QRect upRect, downRect;
    bool hoverUp = false;
    bool hoverDown = false;
    QPixmap upArrow;
    QPixmap downArrow;

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent([[maybe_unused]] QEvent *event) override;

    using QDoubleSpinBox::lineEdit;
};
