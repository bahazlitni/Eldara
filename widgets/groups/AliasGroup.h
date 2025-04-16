#pragma once
#include <QGroupBox>
#include "ObjectGroup.h"
#include "utils/Globals.h"

class IDLabel;
class AddressInput;
class CoordinateInput;
class RadiusInput;
class BooleanInput;
class MainPanel;

class AliasGroup: public ObjectGroup {
    Q_OBJECT

protected:
    AddressInput *Address;
    CoordinateInput *X, *Y;
    RadiusInput *Radius;

public:
    AliasGroup(MainPanel *mainPanel);

    ObjectType type() const override { return ALIAS; }
    ObjectCategory category() const override { return _NODE; }

    void updateSelection(const Selection &selection) override;
    void updateCoordinates() override;
};
