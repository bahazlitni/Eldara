#pragma once
#include "ColorsGroup.h"

class RadiusInput;
class ColorInput;
class BooleanInput;
class Scene;
class MainPanel;

class PenGroup: public InputGroup {
    Q_OBJECT

protected:
    RadiusInput *Radius;
    BooleanInput *ShowLabel;

    ColorInput *StrokeColor;
    ColorInput *FillColor;

public:
    PenGroup(MainPanel *mainPanel, QWidget *parent = nullptr);

    QVariant getAttr(const Attr attr) const override;
    void setAttr(const Attr attr, const QVariant &v) override;
    void confirmAttr(const Attr attr, const QVariant &v) override;

    bool isMixed([[maybe_unused]] const Attr attr) const override { return false; }
    bool isEmpty() const override { return false; }

    void updateData() override;

    QColor getColor(ColorInput *colorInput) const override;
    void setColor(ColorInput *colorInput, const QColor &newColor) override;
    void confirmColor(ColorInput *colorInput, const QColor &newColor) override;
};
