#include "ColorsGroup.h"
#include "widgets/inputs/ColorInput.h"
#include "tools/Selector.h"
#include "objects/Object.h"
#include "widgets/MainPanel.h"
#include "App.h"

#include <QLabel>
#include <QVBoxLayout>

// Constructor: creates the section widgets once.
ColorsGroup::ColorsGroup(MainPanel *mainPanel, QWidget *parent):
    InputGroup(mainPanel, parent),
    // New member variables for the section widgets and layouts.
    fillSection(new QWidget(contentWidget)),
    fillLayout(new QVBoxLayout(fillSection)),
    fillHeader(new QLabel("Fill")),
    fillBody(new QWidget(fillSection)),
    fillBodyLayout(new QVBoxLayout(fillBody)),
    strokeSection(new QWidget(contentWidget)),
    strokeLayout(new QVBoxLayout(strokeSection)),
    strokeHeader(new QLabel("Stroke")),
    strokeBody(new QWidget(strokeSection)),
    strokeBodyLayout(new QVBoxLayout(strokeBody))
{
    contentLayout->setSpacing(16);
    headerButton->setText("Color Set");

    fillLayout->addWidget(fillHeader);
    fillLayout->addWidget(fillBody);
    fillSection->setLayout(fillLayout);
    fillBody->setLayout(fillBodyLayout);
    fillLayout->setContentsMargins(0,0,0,0);
    fillBodyLayout->setContentsMargins(0,0,0,0);
    fillBodyLayout->setSpacing(5);
    fillLayout->setSpacing(10);

    strokeLayout->addWidget(strokeHeader);
    strokeLayout->addWidget(strokeBody);
    strokeSection->setLayout(strokeLayout);
    strokeBody->setLayout(strokeBodyLayout);
    strokeLayout->setContentsMargins(0,0,0,0);
    strokeBodyLayout->setContentsMargins(0,0,0,0);
    strokeBodyLayout->setSpacing(5);
    strokeLayout->setSpacing(10);

    // Add the section widgets to the main content layout.
    contentLayout->addWidget(fillSection, 0, 0);
    contentLayout->addWidget(strokeSection, 1, 0);
}

void ColorsGroup::clear() {
    // Delete all ColorInput widgets held by each ColorSet.
    for (const ColorSet &cs : colorSets) {
        if (cs.input)
            cs.input->deleteLater();
    }
    colorSets.clear();

    // Clear the fill section layout except the first widget.
    // This loop removes all items while leaving the item at index 0.
    while (fillBodyLayout->count() > 0) {
        // Always remove the widget at position 1; position 0 remains intact.
        QLayoutItem *item = fillBodyLayout->takeAt(0);
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    // Clear the stroke section layout except the first widget.
    while (strokeBodyLayout->count() > 0) {
        QLayoutItem *item = strokeBodyLayout->takeAt(0);
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }
}



void ColorsGroup::updateData() {
    clear();

    Selection selection = mainPanel->app->selector.selection();
    if (selection.isEmpty()) {
        setVisible(false);
        return;
    }

    QHash<QRgb, Selection> fillMap;
    QHash<QRgb, Selection> strokeMap;

    for (const auto &obj : selection) {
        if(obj->hasBrush()){
            QRgb rgb(obj->fillColor().rgb());
            if(fillMap.contains(rgb))
                fillMap[rgb].insert(obj);
            else
                fillMap.insert(rgb, Selection{obj});
        }
        if(obj->hasPen()){
            QRgb rgb(obj->strokeColor().rgb());
            if(strokeMap.contains(rgb))
                strokeMap[rgb].insert(obj);
            else
                strokeMap.insert(rgb, Selection{obj});
        }
    }

    for(const Selection &selection : fillMap) {
        ColorSet cs {selection, new ColorInput(this, fillBody), false};
        fillBodyLayout->addWidget(cs.input);
        colorSets.append(cs);
        cs.input->updateData();
    }
    for(const Selection &selection : strokeMap) {
        ColorSet cs {selection, new ColorInput(this, strokeBody), true};
        strokeBodyLayout->addWidget(cs.input);
        colorSets.append(cs);
        cs.input->updateData();
    }

    fillSection->setVisible(!fillMap.isEmpty());
    strokeSection->setVisible(!strokeMap.isEmpty());
    setVisible(!isEmpty());
}


QColor ColorsGroup::getColor(ColorInput *colorInput) const {
    for(const auto &cs : colorSets){
        if(cs.input == colorInput){
            if(cs.selection.isEmpty()) return Qt::black;
            return cs.isStroke
                ? (*cs.selection.constBegin())->strokeColor()
                : (*cs.selection.constBegin())->fillColor();
        }
    }
    return Qt::black;
}

// Applies the new color to the objects associated with the provided ColorInput.
void ColorsGroup::setColor(ColorInput *colorInput, const QColor &newColor) {
    if (!colorInput)
        return;

    // Locate the ColorSet with the matching input.
    for (const ColorSet &cs : colorSets) {
        if (cs.input == colorInput) {
            if (cs.isStroke) {
                for (auto obj : cs.selection)
                    obj->setStrokeColor(newColor);
            }
            else {
                for (auto obj : cs.selection)
                    obj->setFillColor(newColor);
            }
            break;
        }
    }
}

// Confirmation handler for the color change.
void ColorsGroup::confirmColor(ColorInput *colorInput, const QColor &newColor) {
    setColor(colorInput, newColor);
}
