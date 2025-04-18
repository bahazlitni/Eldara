#pragma once
#include "utils/Globals.h"

#include <QString>
#include <QColor>

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

#include <QWheelEvent>

class InputGroup;
class Scene;

class ColorInput : public QWidget {
    Q_OBJECT
private:
    QLineEdit *hexCodeText;
    QPushButton *previewButton;

    InputGroup *group;
    QColor currentColor;

public:
    ColorInput(InputGroup *group, QWidget *parent = nullptr);

    // Set a new color and update the UI.
    void setColor(const QColor &newColor);
    void updateData();

    void resizeEvent(QResizeEvent *event) override;

    void wheelEvent(QWheelEvent *event) override {
        event->ignore();
    }

private slots:
    void onHexEditingFinished();
    void onHexTextChanged(const QString &newText);
    void onPreviewClicked();
};
