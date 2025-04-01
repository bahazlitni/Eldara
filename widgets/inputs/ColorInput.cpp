#include "ColorInput.h"
#include "widgets/groups/InputGroup.h"
#include "App.h"

#include <QRegularExpressionValidator>
#include <QRegularExpression>

#include <QWheelEvent>


ColorInput::ColorInput(InputGroup* group, const QString &key, QWidget *parent)
    : QWidget(parent),
      hexCodeText(new QLineEdit(this)),
      previewButton(new QPushButton(this)),
      group(group),
      key(key),
      currentColor(QColor(key))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(10);
    layout->addWidget(previewButton);
    layout->addWidget(hexCodeText);
    setLayout(layout);

    layout->setContentsMargins(0,0,0,0);

    // Configure previewButton to be a small square showing the current color.
    previewButton->setStyleSheet(QString("background-color: %1; border-radius: 2px; border: none;").arg(currentColor.name()));
    previewButton->setCursor(Qt::PointingHandCursor);

    // Set up the hex input with a simple validator for 6-digit hex (with an optional leading '#').
    QRegularExpression hexRegex("#?[A-Fa-f0-9]{6}");
    hexCodeText->setValidator(new QRegularExpressionValidator(hexRegex, this));
    hexCodeText->setText(currentColor.name().toUpper());

    // Connect signals for both the text input and the preview button.
    connect(hexCodeText, &QLineEdit::editingFinished, this, &ColorInput::onHexEditingFinished);
    connect(hexCodeText, &QLineEdit::textChanged, this, &ColorInput::onHexTextChanged);
    connect(previewButton, &QPushButton::clicked, this, &ColorInput::onPreviewClicked);
}

void ColorInput::setColor(const QColor &newColor){
    currentColor = newColor;
    hexCodeText->setText(currentColor.name().toUpper());
    previewButton->setStyleSheet(QString("background-color: %1; border-radius: 2px; border: none;").arg(currentColor.name()));
}

void ColorInput::updateData(){
    const QColor color = QColor(group->dataString(key));
    if(color.isValid()) currentColor = color;
    setColor(currentColor);
}

void ColorInput::onHexEditingFinished(){
    QColor newColor(hexCodeText->text());
    if(newColor.isValid()){
        setColor(newColor);
        group->onEditingFinishedApply(key, newColor.name().toUpper());
        group->app->update();
    } else {
        hexCodeText->setText(currentColor.name().toUpper());
    }
}

void ColorInput::onHexTextChanged(const QString &newText){
    QColor newColor(newText);
    if(newColor.isValid()){
        // Update the preview in real time without committing the change.
        previewButton->setStyleSheet(QString("background-color: %1; border-radius: 2px; border: none;").arg(newColor.name()));
    }
}

void ColorInput::onPreviewClicked(){
    // Open a color dialog starting with the current color.
    QColor color = QColorDialog::getColor(currentColor, this, "Select Color");
    if(color.isValid()){
        setColor(color);
        group->apply(key, color.name().toUpper());
        if(group->app)
            group->app->update();
    }
}


void ColorInput::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    int size = hexCodeText->height();  // Get the height of the line edit
    previewButton->setFixedSize(size, size);  // Set the button to 1:1 aspect ratio
}
