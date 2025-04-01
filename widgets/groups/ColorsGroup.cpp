#include "ColorsGroup.h"
#include "widgets/inputs/ColorInput.h"
#include "App.h"
#include "tools/Selector.h"
#include "objects/Object.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QColor>

#include "commands/ChangeAttributeCommand.h"

ColorsGroup::ColorsGroup(App *app, QWidget *parent)
    : InputGroup(app, parent)
{
    contentLayout->setSpacing(2);
}

void ColorsGroup::updateData(){
    // Clean up any previous ColorInput widgets.
    qDeleteAll(colorInputs);
    colorInputs.clear();

    // Remove any leftover items from contentLayout.
    QLayoutItem *child;
    while ((child = contentLayout->takeAt(0)) != nullptr) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }

    colorsSelection.clear();

    // Get the current selection of objects from the app.
    LockedSelection selection = app->selector.selection();
    if (selection.isEmpty()){
        setVisible(false);
        return;
    }



    // Group objects by their color value.
    for (auto obj : selection) {
        const QColor color(obj->dataString("color"));
        if (!color.isValid())
            continue;

        // Normalize the color key (using uppercase hex code).
        const QString key = color.name().toUpper();
        if (!colorsSelection.contains(key))
            colorsSelection.insert(key, LockedSelection());
        colorsSelection[key].insert(obj);
    }

    // Create one ColorInput row per unique color.
    for (const QString &key : colorsSelection.keys()){
        ColorInput* input = new ColorInput(this, key, this);
        input->setColor(QColor(key));
        colorInputs.append(input);
        contentLayout->addWidget(input);
    }

    headerButton->setText(colorInputs.size() > 1 ? "Colors Set" : "Color");
    setVisible(true);
}

void ColorsGroup::apply(const QString &key, const QString &value){
    if (!colorsSelection.contains(key)) return;
    LockedSelection selectionForColor = colorsSelection[key];
    for (auto obj : selectionForColor) obj->setData("color", value);
}


void ColorsGroup::onEditingFinishedApply(const QString &key, const QString &value){
    if(lastApplied.contains(key) && lastApplied[key] == value) return;
    app->execute(std::make_unique<ChangeAttributeCommand>(app, colorsSelection[key], key, value));
    lastApplied[key] = value;
}


