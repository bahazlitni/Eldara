#pragma once
#include "utils/Globals.h"

class App;
class QuickSettingsTab;
class SelectionTab;
class VariablesTab;

class MainPanel : public QTabWidget {
    Q_OBJECT

public:
    explicit MainPanel(App *app, QWidget *parent = nullptr);
    App *app;
    QuickSettingsTab *quickSettingsTab;
    SelectionTab *selectionTab;
    VariablesTab *variablesTab;

private:
    const QString qssPath = ":widgets/styles/MainPanel.qss";
    void loadStyleSheet();
};
