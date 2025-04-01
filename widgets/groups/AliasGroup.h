#pragma once
#include <QGroupBox>
#include "ObjectGroup.h"
#include "utils/Types.h"

class IDLabel;
class AddressInput;
class CoordinateInput;
class RadiusInput;
class BooleanInput;
class App;

class AliasGroup: public ObjectGroup {
    Q_OBJECT

protected:
    AddressInput *Address;
    CoordinateInput *X, *Y;
    RadiusInput *Radius;

public:
    AliasGroup(App *app);

    ObjectType type() const override { return ALIAS; }
    ObjectCategory category() const override { return _NODE; }

    void updateSelection(const LockedSelection &selection) override;
    void updateCoordinates() override;
};
