#pragma once
#include "utils/Globals.h"

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QSplitter>
#include <QDockWidget>
#include <QUndoView>
#include <QKeySequence>

class Scene;
class MainPanel;
class SelectionTab;
class QuickSettingsTab;
class PreferencesDialog;
class QString;

class Window : public QMainWindow {
    Q_OBJECT

public:
    explicit Window();

private:
    struct AliasData {
        uint64_t id;
        int64_t address;
        float x, y;
        bool showLabel;
        uint32_t strokeColor, fillColor;
        uint8_t radius, strokeWidth, penStyle, brushStyle;
    };

    struct ResistorData {
        uint64_t id, idA, idB;
        double R;
        bool showLabel;
        uint32_t strokeColor;
        uint8_t strokeWidth, penStyle;
    };

    struct CapacitorData {
        uint64_t id, idA, idB;
        double C, Q0;
        bool showLabel;
        uint32_t strokeColor;
        uint8_t strokeWidth, penStyle;
    };

    struct InductorData {
        uint64_t id, idA, idB;
        double L, I0;
        bool showLabel;
        uint32_t strokeColor;
        uint8_t strokeWidth, penStyle;
    };

    struct DCVData {
        uint64_t id, idA, idB;
        double V;
        bool showLabel;
        uint32_t strokeColor;
        uint8_t strokeWidth, penStyle;
    };

    struct DCIData {
        uint64_t id, idA, idB;
        double I;
        bool showLabel;
        uint32_t strokeColor;
        uint8_t strokeWidth, penStyle;
    };

    struct BatteryData {
        uint64_t id, idA, idB;
        double V;
        bool showLabel;
        uint32_t strokeColor;
        uint8_t strokeWidth, penStyle;
    };

    struct DoubleVariableData {
        char name[VARNAME_MAX_LENGTH];
        double value;
        uint8_t type;
        uint32_t linksCount;
    };

    struct IntVariableData {
        char name[VARNAME_MAX_LENGTH];
        int value;
        uint8_t type;
        uint32_t linksCount;
    };

    struct StringVariableData {
        char name[VARNAME_MAX_LENGTH];
        char value[VARVALUE_STRING_MAX_LENGTH];
        uint8_t type;
        uint32_t linksCount;
    };

    struct VariableLink {
        uint64_t id;
        uint16_t param;
    };

    QSplitter   *splitter;
    QMenu       *fileMenu;
    QMenu       *editMenu;
    QMenu       *simulationMenu;
    QAction     *openAction;
    QAction     *saveAction;
    QAction     *saveAsAction;
    QAction     *preferencesAction;
    QAction     *undoAction;
    QAction     *redoAction;
    QAction     *simulationAction;

    QDockWidget *historyDock;
    QUndoView   *undoView;

    Scene       *scene;
    MainPanel   *mainPanel;

    QString currentSavePath;
    bool saveToFile(const QString &filename);

    void setupMenuBar();
    void setupSplitter();
    void setupConnections();
    void applySettings();
    void setupHistoryDock();

private slots:
    void openFile();
    void saveFile();
    void saveFileAs();
    void openPreferences();

    void onSimulationActionTriggered();
    void updateSimulationActionToRun();
    void updateSimulationActionToStop();

protected:
    void closeEvent(QCloseEvent *event) override;
};
