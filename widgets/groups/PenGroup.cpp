#include "PenGroup.h"
#include "widgets/inputs/SimpleIntegerInput.h"
#include "widgets/inputs/ColorInput.h"
#include "widgets/inputs/BooleanInput.h"
#include "widgets/MainPanel.h"
#include "Scene.h"

#include <QSettings>

PenGroup::PenGroup(MainPanel *mainPanel, QWidget *parent):
    InputGroup(mainPanel, parent),
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
    FillColor(new ColorInput(this, this)),
    Radius(
        new SimpleIntegerInput(
            this,
            Attr::Radius,
            Limits::MIN_ALIAS_RADIUS,
            Limits::MAX_ALIAS_RADIUS
            )
        ),
    AliasOutline(
        new SimpleIntegerInput(
            this,
            Attr::AliasOutline,
            Limits::MIN_ALIAS_OUTLINE,
            Limits::MAX_ALIAS_OUTLINE
            )
        ),
    StrokeWidth(
        new SimpleIntegerInput(
            this,
            Attr::StrokeWidth,
            Limits::MIN_STROKE_WIDTH,
            Limits::MAX_STROKE_WIDTH
            )
        )
{
    headerButton->setText("Pen Settings");

    contentLayout->addWidget(new QLabel("Radius"), 0, 0);
    contentLayout->addWidget(Radius, 0, 1);

    contentLayout->addWidget(new QLabel("Fill"), 1, 0);
    contentLayout->addWidget(FillColor, 1, 1);

    contentLayout->addWidget(new QLabel("Stroke"), 2, 0);
    contentLayout->addWidget(StrokeColor, 2, 1);

    contentLayout->addWidget(new QLabel("Outline"), 3, 0);
    contentLayout->addWidget(AliasOutline, 3, 1);

    contentLayout->addWidget(new QLabel("Thickness"), 4, 0);
    contentLayout->addWidget(StrokeWidth, 4, 1);

    contentLayout->addWidget(new QLabel("Show Label"), 5, 0);
    contentLayout->addWidget(ShowLabel, 5, 1);

    setVisible(true);
    updateData();
}

QColor PenGroup::getColor(ColorInput *colorInput) const {
    if(colorInput == StrokeColor) return mainPanel->scene->pen.strokeColor();
    return mainPanel->scene->pen.fillColor();
}
QVariant PenGroup::getAttr([[maybe_unused]] const Attr attr) const {
    return mainPanel->scene->pen.getAttr(attr);
}
void PenGroup::setAttr(const Attr attr, const QVariant &v) {
    mainPanel->scene->pen.setAttr(attr, v);
}
void PenGroup::confirmAttr(const Attr attr, const QVariant &v) {
    mainPanel->scene->pen.setAttr(attr, v);
    QSettings s("EldaraSoft", "Eldara");
    switch(attr){
    case Attr::Radius:
        s.setValue("pen/radius", v.toUInt());
        break;
    case Attr::StrokeWidth:
        s.setValue("pen/strokeWidth", v.toUInt());
        break;
    case Attr::AliasOutline:
        s.setValue("pen/aliasOutline", v.toUInt());
        break;
    default:
        break;
    }
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
    QSettings s("EldaraSoft", "Eldara");
    s.setValue(QString("pen/%1Color").arg(strokeOrFill == Attr::StrokeColor? "stroke" : "fill"), newColor);
}

void PenGroup::confirmColor(ColorInput *colorInput, const QColor &newColor) {
    setColor(colorInput, newColor);
}
