#include "SelectionTab.h"
#include "App.h"

#include "widgets/groups/ObjectGroup.h"
#include "widgets/groups/AliasGroup.h"
#include "widgets/groups/UnitDipoleGroup.h"
#include "widgets/groups/ColorsGroup.h"

#include <QVBoxLayout>
#include <QToolButton>
#include <QGroupBox>
#include <QScrollArea>
#include <QTabWidget>
#include <QLabel>
#include <QGuiApplication>

SelectionTab::SelectionTab(App *app, QWidget *parent) :
    QWidget(parent),
    app(app),
    mainLayout(new QVBoxLayout(this)),
    colorsGroup(new ColorsGroup(this->app)),
    scrollArea(new QScrollArea(this))
{
    setObjectName("SelectionTab");

    setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignTop);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // Create a container widget to hold all content
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->setAlignment(Qt::AlignTop);

    // Add object groups
    objectGroups.append(new AliasGroup(this->app));
    unitDipoleGroups.append(new UnitDipoleGroup(this->app, RESISTOR));
    unitDipoleGroups.append(new UnitDipoleGroup(this->app, CAPACITOR));
    unitDipoleGroups.append(new UnitDipoleGroup(this->app, INDUCTOR));
    unitDipoleGroups.append(new UnitDipoleGroup(this->app, BATTERY));
    unitDipoleGroups.append(new UnitDipoleGroup(this->app, DC_CURRENT_GENERATOR));
    unitDipoleGroups.append(new UnitDipoleGroup(this->app, DC_VOLTAGE_GENERATOR));

    for (auto unitDipoleGroup : unitDipoleGroups) objectGroups.append(unitDipoleGroup);
    for (auto objectGroup : objectGroups) contentLayout->addWidget(objectGroup);

    contentLayout->addWidget(colorsGroup);

    // Set container widget inside the scroll area
    contentWidget->setLayout(contentLayout);
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add the scroll area to the main layout
    mainLayout->addWidget(scrollArea);
}

void SelectionTab::updateData() {
    for (auto objectGroup : objectGroups)
        objectGroup->updateData();
    colorsGroup->updateData();
}

void SelectionTab::updateCoordinates() {
    for (auto objectGroup : objectGroups)
        objectGroup->updateCoordinates();
}

void SelectionTab::onAddVariables(
    const QVector<QString> &names,
    const QVector<QVariant> &values,
    const QVector<VariableType> &types
){
    for(const auto &unitDipoleGroup : unitDipoleGroups)
        unitDipoleGroup->onAddVariables(names, values, types);
}
void SelectionTab::onChangeVariables(
    const QVector<QString> &names,
    const QVector<QVariant> &newValues,
    [[maybe_unused]] const QVector<VariableType> &types
){
    for(const auto &unitDipoleGroup : unitDipoleGroups)
        unitDipoleGroup->onChangeVariables(names, newValues, types);
}
void SelectionTab::onRemoveVariables(
    const QVector<QString> &names,
    [[maybe_unused]] const QVector<VariableType> &types
){
    for(const auto &unitDipoleGroup : unitDipoleGroups)
        unitDipoleGroup->onRemoveVariables(names, types);
}
