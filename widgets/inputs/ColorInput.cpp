#include "ColorInput.h"
#include "widgets/groups/InputGroup.h"
#include "Scene.h"
#include "widgets/MainPanel.h"

#include <QHBoxLayout>
#include <QColorDialog>

#include <QRegularExpressionValidator>

ColorInput::ColorInput(InputGroup *group, QWidget *parent):
    QWidget(parent),
    hexCodeText(new QLineEdit(this)),
    previewButton(new QPushButton(this)),
    group(group)
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
    const QString colorName = newColor.name().toUpper();
    currentColor = newColor;
    hexCodeText->setText(colorName);
    previewButton->setStyleSheet(
        QString("background-color: %1; border-radius: 2px; border: none;").arg(colorName)
    );
}

void ColorInput::updateData(){
    setColor(group->getColor(this));
}

void ColorInput::onHexEditingFinished(){
    QColor newColor(hexCodeText->text());
    if(newColor.isValid()){
        setColor(newColor);
        group->setColor(this, newColor);
        group->mainPanel->scene->update();
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
    QColor color = QColorDialog::getColor(currentColor, this, "Select Color");
    if(color.isValid()){
        setColor(color);
        group->setColor(this, color);
        group->mainPanel->scene->update();
    }
}


void ColorInput::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    int size = hexCodeText->height();
    previewButton->setFixedSize(size, size);
}
