#pragma once
#include "utils/Globals.h"

#include "ObjectGroup.h"

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
    BooleanInput *Gnd;

public:
    AliasGroup(MainPanel *mainPanel);

    void updateData() override;

    ObjectType type() const override { return ObjectType::Alias; }
    ObjectCategory category() const override { return ObjectCategory::Node; }

    void updateSelection(const Selection &selection) override;
    void updateCoordinates() override;

private slots:
    void onGndStateChanged();
};
