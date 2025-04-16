#include "AliasGroup.h"
#include "widgets/HSeparator.h"
#include "widgets/inputs/AddressInput.h"
#include "widgets/inputs/CoordinateInput.h"
#include "widgets/inputs/RadiusInput.h"
#include "widgets/MainPanel.h"

AliasGroup::AliasGroup(MainPanel *mainPanel):
    ObjectGroup(mainPanel),
    Address(new AddressInput(this)),
    X(new CoordinateInput(this, Attr::X)),
    Y(new CoordinateInput(this, Attr::Y)),
    Radius(new RadiusInput(this))
{

    QHBoxLayout *positionLayout = new QHBoxLayout();
    QWidget *positionWidget = new QWidget();

    positionWidget->setLayout(positionLayout);
    positionLayout->setSpacing(8);
    positionLayout->setContentsMargins(0, 0, 0, 0);

    contentLayout->addWidget(new QLabel("X, Y"), 0, 0);
    positionLayout->addWidget(X);
    positionLayout->addWidget(Y);
    contentLayout->addWidget(positionWidget, 0, 1);

    contentLayout->addWidget(new QLabel("Size"), 1, 0);
    contentLayout->addWidget(Radius, 1, 1);

    contentLayout->addWidget(new QLabel("Address"), 2, 0);
    contentLayout->addWidget(Address, 2, 1);
}


void AliasGroup::updateSelection(const Selection &selection){
    ObjectGroup::updateSelection(selection);
    Address->updateData();
    X->updateData();
    Y->updateData();
    Radius->updateData();
}


void AliasGroup::updateCoordinates(){
    X->updateData();
    Y->updateData();
}
