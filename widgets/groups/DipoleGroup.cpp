#include "DipoleGroup.h"
#include "AliasGroup.h"

#include "widgets/HSeparator.h"
#include "widgets/inputs/IDLabel.h"

#include "App.h"
#include <QLabel>

DipoleGroup::DipoleGroup(App *app, QWidget *parent):
    ObjectGroup(app, parent),
    ID(new IDLabel(this))
{
    contentLayout->addWidget(new QLabel("ID"), 0, 0);
    contentLayout->addWidget(ID, 0, 1);
}

void DipoleGroup::updateData(){
    ObjectGroup::updateData();
    ID->updateData();
}
