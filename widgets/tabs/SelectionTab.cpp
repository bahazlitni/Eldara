#include "SelectionTab.h"
#include "widgets/MainPanel.h"
#include "Scene.h"

#include "widgets/groups/ObjectGroup.h"
#include "widgets/groups/AliasGroup.h"
#include "widgets/groups/DipoleGroup.h"
#include "widgets/groups/ColorsGroup.h"

SelectionTab::SelectionTab(MainPanel *mainPanel) :
    QWidget(mainPanel),
    mainPanel(mainPanel),
    mainLayout(new QVBoxLayout(this)),
    scrollArea(new QScrollArea(this))
{
    setObjectName("SelectionTab");

    setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignTop);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // Create a container widget to hold all content
    QWidget *contentWidget = new QWidget(scrollArea);
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->setAlignment(Qt::AlignTop);

    // Add object groups
    inputGroups.append(new ColorsGroup(mainPanel));
    inputGroups.append(new AliasGroup(mainPanel));
    inputGroups.append(new DipoleGroup(mainPanel, ObjectType::Resistor));
    inputGroups.append(new DipoleGroup(mainPanel, ObjectType::Capacitor));
    inputGroups.append(new DipoleGroup(mainPanel, ObjectType::Inductor));
    inputGroups.append(new DipoleGroup(mainPanel, ObjectType::Battery));
    inputGroups.append(new DipoleGroup(mainPanel, ObjectType::DCI));
    inputGroups.append(new DipoleGroup(mainPanel, ObjectType::DCV));

    for(InputGroup *inputGroup : inputGroups)
        contentLayout->addWidget(inputGroup);

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
    for (auto inputGroup : inputGroups)
        inputGroup->updateData();
}

void SelectionTab::updateCoordinates() {
    for(const auto &inputGroup : inputGroups){
        if(const auto &dipoleGroup = dynamic_cast<DipoleGroup*>(inputGroup))
            dipoleGroup->updateCoordinates();
    }
}

void SelectionTab::onAddVariables(
    const QVector<QString> &names,
    const QVector<QVariant> &values,
    const QVector<VariableType> &types
){
    for(const auto &inputGroup : inputGroups){
        if(const auto &dipoleGroup = dynamic_cast<DipoleGroup*>(inputGroup))
            dipoleGroup->onAddVariables(names, values, types);
    }
}
void SelectionTab::onChangeVariables(
    const QVector<QString> &names,
    const QVector<QVariant> &newValues
){
    for(const auto &inputGroup : inputGroups){
        if(const auto &dipoleGroup = dynamic_cast<DipoleGroup*>(inputGroup))
            dipoleGroup->onChangeVariables(names, newValues);
    }
}
void SelectionTab::onRemoveVariables(
    const QVector<QString> &names
){
    for(const auto &inputGroup : inputGroups){
        if(const auto &dipoleGroup = dynamic_cast<DipoleGroup*>(inputGroup))
            dipoleGroup->onRemoveVariables(names);
    }
}
