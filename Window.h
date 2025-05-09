#pragma once
#include "utils/Globals.h"

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QSplitter>
#include <QDockWidget>
#include <QUndoView>
#include <QKeySequence>
#include <QString>

#include <QSettings>
#include <QVariant>

#include "Scene.h"
#include "widgets/MainPanel.h"


class SelectionTab;
class QuickSettingsTab;
class PreferencesDialog;

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
        char varnameR[VARNAME_MAX_LENGTH];
        bool showLabel;
        uint32_t strokeColor;
        uint8_t strokeWidth, penStyle;
    };

    struct CapacitorData {
        uint64_t id, idA, idB;
        double C, Q0;
        char varnameC[VARNAME_MAX_LENGTH];
        char varnameQ0[VARNAME_MAX_LENGTH];
        bool showLabel;
        uint32_t strokeColor;
        uint8_t strokeWidth, penStyle;
    };

    struct InductorData {
        uint64_t id, idA, idB;
        double L, I0;
        char varnameL[VARNAME_MAX_LENGTH];
        char varnameI0[VARNAME_MAX_LENGTH];
        bool showLabel;
        uint32_t strokeColor;
        uint8_t strokeWidth, penStyle;
    };

    struct DCVData {
        uint64_t id, idA, idB;
        double V;
        char varnameV[VARNAME_MAX_LENGTH];
        bool showLabel;
        uint32_t strokeColor;
        uint8_t strokeWidth, penStyle;
    };

    struct DCIData {
        uint64_t id, idA, idB;
        double I;
        char varnameI[VARNAME_MAX_LENGTH];
        bool showLabel;
        uint32_t strokeColor;
        uint8_t strokeWidth, penStyle;
    };

    struct BatteryData {
        uint64_t id, idA, idB;
        double V;
        char varnameV[VARNAME_MAX_LENGTH];
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


    PreferencesDialog *preferencesDialogue = nullptr;
    QSplitter   *splitter = nullptr;
    QMenu       *fileMenu = nullptr;
    QMenu       *editMenu = nullptr;
    QMenu       *simulationMenu = nullptr;

    QMenu       *viewMenu = nullptr;
    QAction     *historyDockViewAction = nullptr;
    QAction     *mainPanelViewAction = nullptr;

    QAction     *openAction = nullptr;
    QAction     *saveAction = nullptr;
    QAction     *saveAsAction = nullptr;
    QAction     *preferencesAction = nullptr;
    QAction     *undoAction = nullptr;
    QAction     *redoAction = nullptr;
    QAction     *simulationAction = nullptr;

    QDockWidget *historyDock = nullptr;
    QUndoView   *undoView = nullptr;

    Scene scene;
    MainPanel mainPanel;

    QString currentSavePath;
    bool saveToFile(const QString &filename);
    void saveSettings();


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
