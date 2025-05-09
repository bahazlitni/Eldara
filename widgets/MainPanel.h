#pragma once
#include "utils/Globals.h"

#include <QString>

#include <QWidget>
#include <QTabWidget>

class Scene;
class QuickSettingsTab;
class SelectionTab;
class VariablesTab;

class MainPanel : public QTabWidget {
    Q_OBJECT

public:
    explicit MainPanel(Scene *scene, QWidget *parent = nullptr);
    Scene *scene;
    QuickSettingsTab *quickSettingsTab;
    SelectionTab *selectionTab;
    VariablesTab *variablesTab;

    void reset();
    void updateData();

private:
    const QString qssPath = ":widgets/styles/MainPanel.qss";
    void loadStyleSheet();
};
