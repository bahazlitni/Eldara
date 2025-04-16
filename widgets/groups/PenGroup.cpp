#include "PenGroup.h"
#include "widgets/HSeparator.h"
#include "widgets/inputs/RadiusInput.h"
#include "widgets/inputs/ColorInput.h"
#include "widgets/inputs/BooleanInput.h"
#include "widgets/MainPanel.h"
#include "App.h"


PenGroup::PenGroup(MainPanel *mainPanel, QWidget *parent):
    InputGroup(mainPanel, parent),
    Radius(new RadiusInput(this)),
    ShowLabel(
        new BooleanInput(
            this, Attr::ShowLabel,
            QIcon(":/assets/icons/eye_opened.png"),
            QIcon(":/assets/icons/eye_shut.png"),
            QSize(16,16),
            parent
            )
        ),
    StrokeColor(new ColorInput(this, this)),
    FillColor(new ColorInput(this, this))
{
    headerButton->setText("Pen Settings");

    contentLayout->addWidget(new QLabel("Radius"), 0, 0);
    contentLayout->addWidget(Radius, 0, 1);

    contentLayout->addWidget(new QLabel("Fill"), 1, 0);
    contentLayout->addWidget(FillColor, 1, 1);

    contentLayout->addWidget(new QLabel("Stroke"), 2, 0);
    contentLayout->addWidget(StrokeColor, 2, 1);

    contentLayout->addWidget(new QLabel("Show Label"), 3, 0);
    contentLayout->addWidget(ShowLabel, 3, 1);

    setVisible(true);
    updateData();
}

QColor PenGroup::getColor(ColorInput *colorInput) const {
    if(colorInput == StrokeColor) return mainPanel->app->pen.strokeColor();
    return mainPanel->app->pen.fillColor();
}
QVariant PenGroup::getAttr([[maybe_unused]] const Attr attr) const {
    return mainPanel->app->pen.getAttr(attr);
}
void PenGroup::setAttr(const Attr attr, const QVariant &v) {
    mainPanel->app->pen.setAttr(attr, v);
}
void PenGroup::confirmAttr(const Attr attr, const QVariant &v) {
    mainPanel->app->pen.setAttr(attr, v);
}



void PenGroup::updateData(){
    InputGroup::updateData();
    Radius->updateData();
    FillColor->updateData();
    StrokeColor->updateData();
}

void PenGroup::setColor(ColorInput *colorInput, const QColor &newColor) {
    const Attr strokeOrFill = colorInput == StrokeColor? Attr::StrokeColor : Attr::FillColor;
    setAttr(strokeOrFill, newColor);
}

void PenGroup::confirmColor(ColorInput *colorInput, const QColor &newColor) {
    setColor(colorInput, newColor);
}
