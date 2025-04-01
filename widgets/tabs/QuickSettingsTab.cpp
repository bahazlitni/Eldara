#include "QuickSettingsTab.h"
#include "App.h"

#include "widgets/groups/PenGroup.h"

#include <QVBoxLayout>
#include <QToolButton>
#include <QGroupBox>
#include <QScrollArea>
#include <QTabWidget>
#include <QLabel>
#include <QGuiApplication>

QuickSettingsTab::QuickSettingsTab(App *app, QWidget *parent) :
    QWidget(parent),
    app(app),
    mainLayout(new QVBoxLayout(this)),
    scrollArea(new QScrollArea(this)),
    penGroup(new PenGroup(app, this))
{
    setObjectName("QuickSettingsTab");

    setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignTop);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    // Create a container widget to hold all content
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->setAlignment(Qt::AlignTop);

    contentLayout->addWidget(penGroup);

    // Set container widget inside the scroll area
    contentWidget->setLayout(contentLayout);
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add the scroll area to the main layout
    mainLayout->addWidget(scrollArea);
}


void QuickSettingsTab::updatePenData(){
    penGroup->updateData();
}

void QuickSettingsTab::updateGridData(){

}
