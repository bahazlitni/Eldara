#include "MainPanel.h"
#include "Scene.h"

#include "widgets/groups/ObjectGroup.h"
#include "widgets/groups/AliasGroup.h"
#include "widgets/groups/ColorsGroup.h"

#include "widgets/tabs/QuickSettingsTab.h"
#include "widgets/tabs/SelectionTab.h"
#include "widgets/tabs/VariablesTab.h"

MainPanel::MainPanel(Scene *scene, QWidget *parent) :
    QTabWidget(parent),
    scene(scene),
    quickSettingsTab(new QuickSettingsTab(this)),
    selectionTab(new SelectionTab(this)),
    variablesTab(new VariablesTab(this))
{
    setObjectName("MainPanel");
    setMinimumWidth(300);
    setMaximumWidth(400);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    addTab(quickSettingsTab, "Quick Settings");
    addTab(selectionTab, "Selection");
    addTab(variablesTab, "Variables");

    loadStyleSheet();
}

void MainPanel::loadStyleSheet() {
    QFile file(qssPath);

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        setStyleSheet(styleSheet);
        file.close();
    }
}


void MainPanel::reset(){
    quickSettingsTab->reset();
    selectionTab->reset();
    variablesTab->reset();
}
