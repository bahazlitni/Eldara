#include "MainPanel.h"
#include "App.h"

#include "widgets/groups/ObjectGroup.h"
#include "widgets/groups/AliasGroup.h"
#include "widgets/groups/ColorsGroup.h"

#include "widgets/tabs/QuickSettingsTab.h"
#include "widgets/tabs/SelectionTab.h"
#include "widgets/tabs/VariablesTab.h"


#include <QVBoxLayout>
#include <QToolButton>
#include <QGroupBox>
#include <QScrollArea>
#include <QTabWidget>
#include <QLabel>
#include <QGuiApplication>

#include <QFile>

MainPanel::MainPanel(App *app, QWidget *parent) :
    QTabWidget(parent),
    app(app),
    quickSettingsTab(new QuickSettingsTab(app, this)),
    selectionTab(new SelectionTab(app, this)),
    variablesTab(new VariablesTab(app, this))
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


void MainPanel::onAddVariables(
    const QVector<QString> &names,
    const QVector<QVariant> &values,
    const QVector<VariableType> &types
){
    selectionTab->onAddVariables(names, values, types);
    variablesTab->onAddVariables(names, values, types);
}
void MainPanel::onChangeVariables(
    const QVector<QString> &names,
    const QVector<QVariant> &newValues,
    const QVector<VariableType> &types
){
    selectionTab->onChangeVariables(names, newValues, types);
    variablesTab->onChangeVariables(names, newValues, types);
}
void MainPanel::onRemoveVariables(
    const QVector<QString> &names,
    const QVector<VariableType> &types
){
    selectionTab->onRemoveVariables(names, types);
    variablesTab->onRemoveVariables(names, types);
}
