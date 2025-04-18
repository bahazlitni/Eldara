#pragma once
#include "utils/Globals.h"

#include "InputGroup.h"

#include <QHash>
#include <QVector>
#include <QVariant>
#include <QColor>

#include <QLabel>
#include <QVBoxLayout>

class ColorInput;
class MainPanel;

class ColorsGroup : public InputGroup {
    Q_OBJECT

protected:

    struct ColorSet {
        Selection selection;
        ColorInput *input;
        bool isStroke;

        ColorSet(const Selection &slc, ColorInput *inp, bool s):
            selection(slc), input(inp), isStroke(s){}
    };


    QVector<ColorSet> colorSets;

    // New member variables for the section widgets and layouts.
    QWidget *fillSection;
    QVBoxLayout *fillLayout;
    QLabel *fillHeader;
    QWidget *fillBody;
    QVBoxLayout *fillBodyLayout;

    QWidget *strokeSection;
    QVBoxLayout *strokeLayout;
    QLabel *strokeHeader;
    QWidget *strokeBody;
    QVBoxLayout *strokeBodyLayout;

    void clear();

public:
    ColorsGroup(MainPanel *mainPanel, QWidget *parent = nullptr);
    void updateData() override;
    bool isEmpty() const override { return colorSets.isEmpty(); }
    bool isMixed([[maybe_unused]] const Attr attr) const override { return false; }
    QVariant getAttr([[maybe_unused]] const Attr attr) const override { return ""; }
    void setAttr([[maybe_unused]] const Attr attr, [[maybe_unused]] const QVariant &v) override {}
    void confirmAttr([[maybe_unused]] const Attr attr, [[maybe_unused]] const QVariant &v) override {}

    QColor getColor(ColorInput *colorInput) const override;
    void setColor(ColorInput *colorInput, const QColor &newColor) override;
    void confirmColor(ColorInput *colorInput, const QColor &newColor) override;
};
