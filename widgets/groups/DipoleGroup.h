#pragma once
#include <QWidget>
#include "ObjectGroup.h"
#include "utils/Types.h"

class App;
class CoordinateInput;
class IDLabel;
class AliasGroup;

class DipoleGroup: public ObjectGroup {
    Q_OBJECT

protected:
    IDLabel *ID;

public:
    DipoleGroup(App *app, QWidget *parent = nullptr);
    ObjectCategory category() const override { return _DIPOLE; }

    void updateData() override;
};
