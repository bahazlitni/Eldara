#include "PenGroup.h"
#include "widgets/HSeparator.h"
#include "widgets/inputs/RadiusInput.h"
#include "widgets/inputs/ColorInput.h"
#include "widgets/inputs/BooleanInput.h"

#include "App.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>

PenGroup::PenGroup(App *app, QWidget *parent):
    InputGroup(app, parent),
    Radius(new RadiusInput(this)),
    StrokeColor(new ColorInput(this, "stroke")),
    FillColor(new ColorInput(this, "fill")),
    ShowLabel(
        new BooleanInput(
            this, "show-label",
            QIcon(":/assets/icons/eye_opened.png"),
            QIcon(":/assets/icons/eye_shut.png"),
            QSize(16,16),
            parent
        )
    )
{
    headerButton->setText("Pen Settings");

    contentLayout->addWidget(new QLabel("Radius"), 0, 0);
    contentLayout->addWidget(Radius, 0, 1);

    contentLayout->addWidget(new QLabel("Fill Color"), 1, 0);
    contentLayout->addWidget(FillColor, 1, 1);

    contentLayout->addWidget(new QLabel("Stroke Color"), 2, 0);
    contentLayout->addWidget(StrokeColor, 2, 1);

    contentLayout->addWidget(new QLabel("Show Label"), 3, 0);
    contentLayout->addWidget(ShowLabel, 3, 1);

    setVisible(true);
    updateData();
}

void PenGroup::apply(const QString &key, const QString &value) { app->pen.setData(key, value); }
void PenGroup::updateData() {
    InputGroup::updateData();
    Radius->updateData();
    FillColor->updateData();
    StrokeColor->updateData();
}

QString PenGroup::dataString(const QString &key) const {
    return app->pen.dataString(key);
}
