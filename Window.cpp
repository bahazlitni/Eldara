#include "Window.h"
#include "Scene.h"
#include "widgets/MainPanel.h"
#include "widgets/tabs/SelectionTab.h"
#include "widgets/tabs/QuickSettingsTab.h"
#include "tools/Selector.h"
#include "tools/Pen.h"
#include "dialogs/PreferencesDialog.h"
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>

Window::Window()
    : QMainWindow(),
    splitter(new QSplitter(Qt::Horizontal, this)),
    scene(new Scene(splitter)),
    mainPanel(new MainPanel(scene, splitter))
{
    setWindowTitle("Circuits Simulator");
    setupMenuBar();
    setupSplitter();
    setupConnections();
    applySettings();
}

void Window::setupMenuBar() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    openAction = new QAction(tr("&Open..."), this);
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(openAction);
    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    fileMenu->addAction(saveAction);
    saveAsAction = new QAction(tr("Save &As..."), this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    fileMenu->addAction(saveAsAction);
    connect(openAction, &QAction::triggered, this, &Window::openFile);
    connect(saveAction, &QAction::triggered, this, &Window::saveFile);
    connect(saveAsAction, &QAction::triggered, this, &Window::saveFileAs);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    preferencesAction = new QAction(tr("&Preferences..."), this);
    editMenu->addAction(preferencesAction);
    connect(preferencesAction, &QAction::triggered, this, &Window::openPreferences);
}

void Window::setupSplitter() {
    setCentralWidget(splitter);
    splitter->setHandleWidth(1);
    splitter->addWidget(scene);
    splitter->addWidget(mainPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    splitter->setCollapsible(1, false);
    splitter->setSizes({scene->width(), mainPanel->minimumWidth()});
    splitter->setStyleSheet(R"(
        QSplitter::handle {
            background-color: #333;
        }
    )");
}

void Window::setupConnections() {
    connect(&scene->selector, &Selector::selectionChanged,
            mainPanel->selectionTab, &SelectionTab::updateData);
    connect(&scene->selector, &Selector::selectionDragged,
            mainPanel->selectionTab, &SelectionTab::updateCoordinates);
    connect(&scene->pen, &Pen::dataChanged,
            mainPanel->quickSettingsTab, &QuickSettingsTab::updatePenData);
}

void Window::applySettings() {
    QSettings settings("EldaraSoft", "Eldara");

    scene->setBackgroundColor(settings.value("scene/background", QColor(33, 33, 33)).value<QColor>());
    scene->setGridStrokeColor(settings.value("scene/gridStroke", QColor(44, 44, 44)).value<QColor>());
    scene->setTileSize(settings.value("scene/tileSize", 50).toInt());
    scene->setSnapPosition(settings.value("scene/snapPosition", true).toBool());
    scene->setAllowMerging(settings.value("scene/allowMerge", true).toBool());
    scene->setShowGrid(settings.value("scene/showGrid", true).toBool());
    scene->setDisplayRawValues(settings.value("scene/displayRaw", false).toBool());

    Pen &pen = scene->pen;
    pen.setStrokeWidth(settings.value("pen/strokeWidth", 1).toInt());
    pen.setStrokeColor(settings.value("pen/strokeColor", QColor("#CCC")).value<QColor>());
    pen.setFillColor(settings.value("pen/fillColor", QColor("#FFF")).value<QColor>());
    pen.setAllowSplitting(settings.value("pen/allowSplit", true).toBool());
    pen.setAllowOnClickColoring(settings.value("pen/allowOnClickColor", true).toBool());
    pen.setDefaultResistance(settings.value("pen/defaultResistance", 1e3).toDouble());
    pen.setDefaultCapacitance(settings.value("pen/defaultCapacitance", 1e-8).toDouble());
    pen.setDefaultInductance(settings.value("pen/defaultInductance", 1e-4).toDouble());
    pen.setDefaultBatteryVoltage(settings.value("pen/defaultBatteryVoltage", 5.0).toDouble());
    pen.setDefaultDCVoltage(settings.value("pen/defaultDCVVoltage", 15.0).toDouble());
    pen.setDefaultIntensity(settings.value("pen/defaultIntensity", 1e-3).toDouble());
    pen.setDefaultQuantity(settings.value("pen/defaultQuantity", 1.0).toDouble());

    scene->update();
}

void Window::openPreferences() {
    PreferencesDialog dlg(this);
    connect(&dlg, &PreferencesDialog::settingsApplied, this, &Window::applySettings);
    if (dlg.exec() == QDialog::Accepted) {
        applySettings();
    }
}

void Window::openFile() {
    const QString filename = QFileDialog::getOpenFileName(
        this, tr("Open File"), QString(), tr("Dara Files (*.dara);;All Files (*)"));
    if (!filename.isEmpty()) {
        // TODO: implement loading logic
    }
}

void Window::saveFile() {
    saveFileAs(); // TODO: Replace with actual saving
}

void Window::saveFileAs() {
    const QString filename = QFileDialog::getSaveFileName(
        this, tr("Save File As"), QString(), tr("Dara Files (*.dara);;All Files (*)"));
    if (!filename.isEmpty()) {
        // TODO: implement saving logic
    }
}

void Window::closeEvent(QCloseEvent *event) {
    event->accept();
}
