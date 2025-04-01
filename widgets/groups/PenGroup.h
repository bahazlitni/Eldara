#pragma once
#include <QGroupBox>
#include "InputGroup.h"

class RadiusInput;
class ColorInput;
class BooleanInput;
class App;

class PenGroup: public InputGroup {
    Q_OBJECT

protected:
    RadiusInput *Radius;
    ColorInput *StrokeColor;
    ColorInput *FillColor;
    BooleanInput *ShowLabel;

public:
    PenGroup(App *app, QWidget *parent = nullptr);

    QString dataString(const QString &key) const override;
    void apply(const QString &key, const QString &value) override;
    void onEditingFinishedApply(const QString &key, const QString &value) override { apply(key, value); }
    void updateData() override;
};
