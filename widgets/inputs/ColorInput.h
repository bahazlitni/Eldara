#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QColor>
#include <QColorDialog>
#include <QWheelEvent>

class InputGroup;
class App;

class ColorInput : public QWidget {
    Q_OBJECT
private:
    QLineEdit *hexCodeText;
    QPushButton *previewButton;

    InputGroup *group;
    const QString key;  // The original color key for this row.
    QColor currentColor;

public:
    // Constructor: 'group' is the ColorsGroup container; 'key' is the original color.
    ColorInput(InputGroup* group, const QString &key, QWidget *parent = nullptr);

    // Set a new color and update the UI.
    void setColor(const QColor &newColor);
    void updateData();

    void resizeEvent(QResizeEvent *event) override;

    void wheelEvent(QWheelEvent *event) override {
        event->ignore();
    }

private slots:
    // Called when the user finishes editing the hex text.
    void onHexEditingFinished();
    // Called for live preview updates as the text changes.
    void onHexTextChanged(const QString &newText);
    // Called when the preview button is clicked to open a color picker.
    void onPreviewClicked();
};
