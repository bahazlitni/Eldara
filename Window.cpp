#include "Window.h"

#include "Scene.h"
#include "widgets/MainPanel.h"
#include "widgets/tabs/SelectionTab.h"
#include "widgets/tabs/QuickSettingsTab.h"
#include "widgets/tabs/VariablesTab.h"
#include "tools/Selector.h"
#include "tools/Pen.h"
#include "dialogs/PreferencesDialog.h"

#include "objects/Alias.h"
#include "objects/Resistor.h"
#include "objects/Capacitor.h"
#include "objects/Inductor.h"
#include "objects/DCV.h"
#include "objects/DCI.h"
#include "objects/Battery.h"

#include <QSettings>
#include <QColor>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QSplitter>
#include <QDockWidget>
#include <QUndoView>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QString>
#include <QVector>
#include <QHash>
#include <QSet>
#include <QPen>
#include <QBrush>
#include <cstring>
#include <QIcon>
#include <QStandardPaths>
#include <QCoreApplication>

#include <QScreen>
#include <QGuiApplication>


Window::Window()
    : QMainWindow(),
    splitter(new QSplitter(Qt::Horizontal, this)),
    scene(splitter),
    mainPanel(&scene, splitter)
{

    QCoreApplication::setOrganizationName("EldaraSoft");
    QCoreApplication::setApplicationName("Eldara");

    setWindowTitle("Eldara");
    setWindowIcon(QIcon(":/Eldara.ico"));

    QSettings settings;

    restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
    restoreState(settings.value("mainWindow/state").toByteArray());

    if (isMaximized() || size().isEmpty() || pos().isNull()) {
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect screenGeometry = screen->geometry();
            int windowWidth = static_cast<int>(screenGeometry.width() * 0.8);
            int windowHeight = static_cast<int>(screenGeometry.height() * 0.8);
            resize(windowWidth, windowHeight);
            move(screenGeometry.center() - rect().center());
        } else {
            resize(1024, 768);
        }
    }


    setupSplitter();
    setupHistoryDock();

    settings.beginGroup("PanelVisibility");
    bool historyDockVisible = settings.value("historyDock", true).toBool();
    bool mainPanelVisible = settings.value("mainPanel", true).toBool();
    settings.endGroup();

    if (historyDock) {
        historyDock->setVisible(historyDockVisible);
    }

    mainPanel.setVisible(mainPanelVisible);

    setupMenuBar();
    applySettings();
    setupConnections();
}


void Window::saveSettings() {
    QSettings settings;

    settings.setValue("mainWindow/geometry", saveGeometry());
    settings.setValue("mainWindow/state", saveState());

    settings.beginGroup("PanelVisibility");
    if (historyDock) {
        settings.setValue("historyDock", historyDock->isVisible());
    }

    settings.setValue("mainPanel", mainPanel.isVisible());

    settings.endGroup();

    settings.sync();
}


void Window::setupMenuBar() {
    QMenuBar *mb = menuBar();
    if (!mb) {
        return;
    }

    fileMenu = mb->addMenu(tr("&File"));
    openAction = fileMenu->addAction(tr("&Open..."), QKeySequence::Open, this, &Window::openFile);
    saveAction = fileMenu->addAction(tr("&Save"),    QKeySequence::Save, this, &Window::saveFile);
    saveAsAction = fileMenu->addAction(tr("Save &As..."), QKeySequence::SaveAs, this, &Window::saveFileAs);

    editMenu = mb->addMenu(tr("&Edit"));

        undoAction = scene.undoStack.createUndoAction(this, tr("&Undo"));
        undoAction->setShortcuts(QKeySequence::Undo);
        editMenu->addAction(undoAction);

        redoAction = scene.undoStack.createRedoAction(this, tr("&Redo"));
        redoAction->setShortcuts(QKeySequence::Redo);
        editMenu->addAction(redoAction);


    editMenu->addSeparator();
    preferencesAction = editMenu->addAction(tr("&Preferences..."), this, &Window::openPreferences);

    viewMenu = mb->addMenu(tr("&View"));

    if (historyDock) {
        historyDockViewAction = historyDock->toggleViewAction();
        historyDockViewAction->setText(tr("History &Dock"));
        viewMenu->addAction(historyDockViewAction);
    }

    mainPanelViewAction = new QAction(tr("&Main Panel"), this);
    mainPanelViewAction->setCheckable(true);
    mainPanelViewAction->setChecked(mainPanel.isVisible());
    viewMenu->addAction(mainPanelViewAction);

    simulationMenu = mb->addMenu(tr("&Simulation"));
    simulationAction = simulationMenu->addAction(tr("&Run"), this, &Window::onSimulationActionTriggered);
    simulationAction->setShortcut(QKeySequence(tr("Ctrl+R")));
}

void Window::setupSplitter() {
    setCentralWidget(splitter);

    splitter->setHandleWidth(1);

    splitter->addWidget(&scene);
    splitter->addWidget(&mainPanel);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);


    int mainPanelIndex = splitter->indexOf(&mainPanel);
    if (mainPanelIndex != -1) {
        splitter->setCollapsible(mainPanelIndex, false);
    }

    int mainPanelWidth = mainPanel.minimumSizeHint().width();
    int sceneWidth = width() - mainPanelWidth - splitter->handleWidth();
    if (sceneWidth < 0) sceneWidth = width() / 2;

    splitter->setSizes({sceneWidth, mainPanelWidth});


    splitter->setStyleSheet(R"(
        QSplitter::handle {
            background-color: #333;
        }
    )");
}

void Window::setupHistoryDock() {
    undoView = new QUndoView(&scene.undoStack, this);
    undoView->setWindowTitle(tr("Undo History"));

    historyDock = new QDockWidget(tr("History"), this);
    historyDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    historyDock->setWidget(undoView);

    addDockWidget(Qt::RightDockWidgetArea, historyDock);
    historyDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
}

void Window::setupConnections() {

    connect(&scene.selector, &Selector::selectionChanged,
            mainPanel.selectionTab, &SelectionTab::updateData);
    connect(&scene.selector, &Selector::selectionDragged,
            mainPanel.selectionTab, &SelectionTab::updateCoordinates);

    connect(&scene.pen, &Pen::dataChanged,
            mainPanel.quickSettingsTab, &QuickSettingsTab::updatePenData);


    connect(&scene.simulator, &Simulator::simulationStarted,
            this, &Window::updateSimulationActionToStop);
    connect(&scene.simulator, &Simulator::simulationEnded,
            this, &Window::updateSimulationActionToRun);

    connect(mainPanelViewAction, &QAction::toggled,
            &mainPanel, &QWidget::setVisible);

}

void Window::applySettings() {
    QSettings settings("EldaraSoft", "Eldara");

    scene.setBackgroundColor(settings.value("scene/background", QColor(33, 33, 33)).value<QColor>());
    scene.setGridStrokeColor(settings.value("scene/gridStroke", QColor(44, 44, 44)).value<QColor>());
    scene.setTileSize(settings.value("scene/tileSize", 50).toInt());
    scene.setSnapPosition(settings.value("scene/snapPosition", true).toBool());
    scene.setAllowMerging(settings.value("scene/allowMerge", true).toBool());
    scene.setShowGrid(settings.value("scene/showGrid", true).toBool());
    scene.setDisplayRawValues(settings.value("scene/displayRaw", false).toBool());

    Pen &pen = scene.pen;
    pen.setRadius(settings.value("pen/radius", 8).toInt());
    pen.setStrokeWidth(settings.value("pen/strokeWidth", 1).toUInt());
    pen.setAliasOutline(settings.value("pen/aliasOutline", 0).toUInt());
    pen.setStrokeColor(settings.value("pen/strokeColor", QColor("#CCC")).value<QColor>());
    pen.setFillColor(settings.value("pen/fillColor", QColor("#FFF")).value<QColor>());
    pen.setShowLabel(settings.value("pen/showLabel", true).toBool());
    pen.setAllowSplitting(settings.value("pen/allowSplit", true).toBool());
    pen.setAllowOnClickColoring(settings.value("pen/allowOnClickColor", true).toBool());
    pen.setDefaultResistance(settings.value("pen/defaultResistance", 1e3).toDouble());
    pen.setDefaultCapacitance(settings.value("pen/defaultCapacitance", 1e-8).toDouble());
    pen.setDefaultInductance(settings.value("pen/defaultInductance", 1e-4).toDouble());
    pen.setDefaultBatteryVoltage(settings.value("pen/defaultBatteryVoltage", 5.0).toDouble());
    pen.setDefaultDCVoltage(settings.value("pen/defaultDCVVoltage", 15.0).toDouble());
    pen.setDefaultIntensity(settings.value("pen/defaultIntensity", 1e-3).toDouble());
    pen.setDefaultQuantity(settings.value("pen/defaultQuantity", 1.0).toDouble());

    mainPanel.updateData();
    scene.update();
}

void Window::openPreferences() {
    if (!preferencesDialogue) {
        preferencesDialogue = new PreferencesDialog(this);
        connect(preferencesDialogue, &PreferencesDialog::settingsApplied, this, &Window::applySettings);
    }

    preferencesDialogue->exec();
}

void Window::openFile() {
    if (scene.hasChanged()) {
        auto reply = QMessageBox::question(
            this, tr("Unsaved Changes"),
            tr("You have unsaved changes. Do you want to save them before opening a new file?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );
        if (reply == QMessageBox::Yes) {
            saveFile();
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    const QString filename = QFileDialog::getOpenFileName(
        this, tr("Open File"), QString(), tr("Dara Files (*.dara);;All Files (*)")
        );
    if (filename.isEmpty()) return;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file for reading."));
        return;
    }

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);

    char header[4];
    if (in.readRawData(header, 4) != 4 || strncmp(header, "DARA", 4) != 0) {
        QMessageBox::critical(this, tr("Invalid File"), tr("Not a valid .dara file."));
        return;
    }

    quint32 version;
    in >> version;
    if (version != 1) {
        QMessageBox::critical(this, tr("Unsupported Version"), tr("Only .dara version 1 is supported."));
        return;
    }

    scene.reset();
    mainPanel.reset();


    auto readBlock = [&](auto &vec) {
        quint32 count;
        in >> count;
        vec.resize(count);
        for (quint32 i = 0; i < count; ++i)
            in.readRawData(reinterpret_cast<char *>(&vec[i]), sizeof(vec[i]));
    };

    // --- Read Aliases ---
    QVector<AliasData> aliasData;
    readBlock(aliasData);
    for (const auto &d : aliasData) {
        QPen pen; pen.setColor(QColor::fromRgb(d.strokeColor));
        pen.setWidth(d.strokeWidth);
        pen.setStyle(static_cast<Qt::PenStyle>(d.penStyle));
        QBrush brush; brush.setColor(QColor::fromRgb(d.fillColor));
        brush.setStyle(static_cast<Qt::BrushStyle>(d.brushStyle));

        auto alias = std::make_shared<Alias>(
            d.id, d.address, d.x, d.y, d.radius, brush, pen, d.showLabel
        );
        scene.addAlias(alias);
    }

    // --- Read Dipole Data Blocks ---
    QVector<ResistorData>    vec_resData;
    QVector<CapacitorData>   vec_capData;
    QVector<InductorData>    vec_indData;
    QVector<DCVData>         vec_dcvData;
    QVector<DCIData>         vec_dciData;
    QVector<BatteryData>     vec_batData;

    readBlock(vec_resData);
    readBlock(vec_capData);
    readBlock(vec_indData);
    readBlock(vec_dcvData);
    readBlock(vec_dciData);
    readBlock(vec_batData);

    QHash<uint64_t, SharedDipole> dipolesMap;

    // --- Helper: build QPen from Data struct ---
    auto makePen = [&](auto &d){
        QPen pen;
        pen.setColor(QColor::fromRgb(d.strokeColor));
        pen.setWidth(d.strokeWidth);
        pen.setStyle(static_cast<Qt::PenStyle>(d.penStyle));
        return pen;
    };

    // --- Generic loader for all two‐terminal elements ---
    auto forEachDipoleBlock = [&](auto &vec, auto ctor){
        for (auto &d : vec) {
            auto A = scene.aliases[d.idA];
            auto B = scene.aliases[d.idB];
            dipolesMap.insert(d.id, ctor(d, A, B, makePen(d)));
        }
    };

    forEachDipoleBlock(vec_resData, [&](auto &d, auto A, auto B, auto pen){
        return std::make_shared<Resistor>(d.id, A, B, pen, d.showLabel, d.R);
    });
    forEachDipoleBlock(vec_capData, [&](auto &d, auto A, auto B, auto pen){
        return std::make_shared<Capacitor>(d.id, A, B, pen, d.showLabel, d.C, d.Q0);
    });
    forEachDipoleBlock(vec_indData, [&](auto &d, auto A, auto B, auto pen){
        return std::make_shared<Inductor>(d.id, A, B, pen, d.showLabel, d.L, d.I0);
    });
    forEachDipoleBlock(vec_dcvData, [&](auto &d, auto A, auto B, auto pen){
        return std::make_shared<DCV>(d.id, A, B, pen, d.showLabel, d.V);
    });
    forEachDipoleBlock(vec_dciData, [&](auto &d, auto A, auto B, auto pen){
        return std::make_shared<DCI>(d.id, A, B, pen, d.showLabel, d.I);
    });
    forEachDipoleBlock(vec_batData, [&](auto &d, auto A, auto B, auto pen){
        return std::make_shared<Battery>(d.id, A, B, pen, d.showLabel, d.V);
    });

    // --- Connect aliases & dipoles ---
    for (auto &dip : dipolesMap) {
        if (auto A = dip->A()) A->connect(dip);
        if (auto B = dip->B()) B->connect(dip);
    }

    // --- Variables: read and stash links ---
    QVector<DoubleVariableData> vec_doubles;
    readBlock(vec_doubles);
    QVector<VariableLink> links_double;
    for (auto &v : vec_doubles)
        for (uint32_t i = 0; i < v.linksCount; ++i) {
            VariableLink link;
            in.readRawData(reinterpret_cast<char *>(&link), sizeof(link));
            links_double.append(link);
        }

    QVector<IntVariableData> vec_ints;
    readBlock(vec_ints);
    for (auto &v : vec_ints)
        for (uint32_t i = 0; i < v.linksCount; ++i) {
            VariableLink link;
            in.readRawData(reinterpret_cast<char *>(&link), sizeof(link));
        }

    QVector<StringVariableData> vec_strs;
    readBlock(vec_strs);
    for (auto &v : vec_strs)
        for (uint32_t i = 0; i < v.linksCount; ++i) {
            VariableLink link;
            in.readRawData(reinterpret_cast<char *>(&link), sizeof(link));
        }

    // --- Populate VariablesTab ---
    QVector<QString> names;
    QVector<QVariant> values;
    QVector<VariableType> types;

    for (auto &v : vec_doubles) {
        QString nm = QString::fromUtf8(v.name).trimmed();
        if (!nm.isEmpty()) {
            names.append(nm);
            values.append(QVariant(v.value));
            types.append((VariableType)v.type);
        }
    }
    for (auto &v : vec_ints) {
        QString nm = QString::fromUtf8(v.name).trimmed();
        if (!nm.isEmpty()) {
            names.append(nm);
            values.append(QVariant(v.value));
            types.append((VariableType)v.type);
        }
    }
    for (auto &v : vec_strs) {
        QString nm = QString::fromUtf8(v.name).trimmed();
        QString val = QString::fromUtf8(v.value).trimmed();
        if (!nm.isEmpty()) {
            names.append(nm);
            values.append(QVariant(val));
            types.append((VariableType)v.type);
        }
    }
    mainPanel.variablesTab->addVariables(names, values, types);

    // --- Apply variable links for double vars ---
    for (auto &v : vec_doubles) {
        QString nm = QString::fromUtf8(v.name).trimmed();
        for (uint32_t i = 0; i < v.linksCount; ++i) {
            VariableLink link;
            in.readRawData(reinterpret_cast<char *>(&link), sizeof(link));
            if (auto dip = dipolesMap.value(link.id)) {
                dip->setParamValue((Param)link.param, v.value);
                dip->setParamVarname((Param)link.param, nm);
            }
        }
    }

    file.close();
    currentSavePath = filename;
    scene.grid.moveTo(QPointF(0.0f, 0.0f));
    scene.update();
}

void Window::saveFile() {
    if (currentSavePath.isEmpty()) {
        saveFileAs();
    } else {
        saveToFile(currentSavePath);
    }
}

void Window::saveFileAs() {
    const QString filename = QFileDialog::getSaveFileName(
        this, tr("Save File As"), QString(), tr("Dara Files (*.dara);;All Files (*)")
        );
    if (!filename.isEmpty() && saveToFile(filename)) {
        currentSavePath = filename;
    }
}

bool Window::saveToFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file for writing."));
        return false;
    }

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);

    // header + version
    out.writeRawData("DARA", 4);
    out << static_cast<quint32>(1);

    // helper to write vectors of structs
    auto writeBlock = [&](auto &vec){
        out << static_cast<quint32>(vec.size());
        for (auto &item : vec)
            out.writeRawData(reinterpret_cast<const char *>(&item), sizeof(item));
    };

    // prepare data vectors & links map
    QVector<AliasData>     vec_alData;
    QVector<ResistorData>  vec_resData;
    QVector<CapacitorData> vec_capData;
    QVector<InductorData>  vec_indData;
    QVector<DCVData>       vec_dcvData;
    QVector<DCIData>       vec_dciData;
    QVector<BatteryData>   vec_batData;
    QHash<QString, QVector<VariableLink>> varLinks;
    QSet<SharedDipole> visitedDipoles;

    // common pack + record helpers
    auto packCommon = [&](auto &dip, auto &d){
        d.id          = dip->id();
        d.idA         = dip->A()->id();
        d.idB         = dip->B()->id();
        d.strokeWidth = (uint8_t)dip->strokeWidth();
        d.penStyle    = (uint8_t)dip->pen().style();
        d.strokeColor = (uint32_t)dip->strokeColor().rgb();
        d.showLabel   = dip->showLabel();
    };
    auto recordLink = [&](uint64_t id, Param p, const QString &varname){
        if (!varname.isEmpty())
            varLinks[varname].append({id, (uint8_t)p});
    };

    // walk aliases → dipoles once
    for (auto &alias : scene.aliases) {
        AliasData a{};
        a.id          = alias->id();
        a.address     = alias->address();
        a.x           = alias->x();
        a.y           = alias->y();
        a.radius      = (uint8_t)alias->radius();
        a.strokeWidth = (uint8_t)alias->strokeWidth();
        a.penStyle    = (uint8_t)alias->pen().style();
        a.brushStyle  = (uint8_t)alias->brush().style();
        a.fillColor   = (uint32_t)alias->fillColor().rgb();
        a.strokeColor = (uint32_t)alias->strokeColor().rgb();
        a.showLabel   = alias->showLabel();
        vec_alData.append(a);

        for (auto &dip : alias->connections()) {
            if (visitedDipoles.contains(dip)) continue;
            visitedDipoles.insert(dip);

            switch (dip->type()) {
            case ObjectType::Resistor: {
                auto r = std::static_pointer_cast<Resistor>(dip);
                ResistorData d{};
                packCommon(r, d);
                d.R = r->R();
                vec_resData.append(d);
                recordLink(d.id, Param::R, r->paramVarname(Param::R));
                break;
            }
            case ObjectType::Capacitor: {
                auto c = std::static_pointer_cast<Capacitor>(dip);
                CapacitorData d{};
                packCommon(c, d);
                d.C  = c->C();
                d.Q0 = c->Q0();
                vec_capData.append(d);
                recordLink(d.id, Param::C,  c->paramVarname(Param::C));
                recordLink(d.id, Param::Q0, c->paramVarname(Param::Q0));
                break;
            }
            case ObjectType::Inductor: {
                auto l = std::static_pointer_cast<Inductor>(dip);
                InductorData d{};
                packCommon(l, d);
                d.L  = l->L();
                d.I0 = l->I0();
                vec_indData.append(d);
                recordLink(d.id, Param::L,  l->paramVarname(Param::L));
                recordLink(d.id, Param::I0, l->paramVarname(Param::I0));
                break;
            }
            case ObjectType::DCV: {
                auto v = std::static_pointer_cast<DCV>(dip);
                DCVData d{};
                packCommon(v, d);
                d.V = v->V();
                vec_dcvData.append(d);
                recordLink(d.id, Param::V, v->paramVarname(Param::V));
                break;
            }
            case ObjectType::DCI: {
                auto i = std::static_pointer_cast<DCI>(dip);
                DCIData d{};
                packCommon(i, d);
                d.I = i->I();
                vec_dciData.append(d);
                recordLink(d.id, Param::I, i->paramVarname(Param::I));
                break;
            }
            case ObjectType::Battery: {
                auto b = std::static_pointer_cast<Battery>(dip);
                BatteryData d{};
                packCommon(b, d);
                d.V = b->V();
                vec_batData.append(d);
                recordLink(d.id, Param::V, b->paramVarname(Param::V));
                break;
            }
            default:
                break;
            }
        }
    }

    // write all blocks
    writeBlock(vec_alData);
    writeBlock(vec_resData);
    writeBlock(vec_capData);
    writeBlock(vec_indData);
    writeBlock(vec_dcvData);
    writeBlock(vec_dciData);
    writeBlock(vec_batData);

    // --- Variables ---
    QVector<DoubleVariableData> vec_varDouble;
    QVector<IntVariableData>    vec_varInt;
    QVector<StringVariableData> vec_varString;

    const auto &names  = mainPanel.variablesTab->names();
    const auto &values = mainPanel.variablesTab->values();
    const auto &types  = mainPanel.variablesTab->types();

    for (int i = 0; i < names.size(); ++i) {
        const auto &nm   = names[i];
        const auto &val  = values[i];
        auto type        = types[i];
        auto links       = varLinks.value(nm);

        if (varIsDouble(type)) {
            DoubleVariableData d{};
            strncpy(d.name, nm.toUtf8().constData(), sizeof(d.name)-1);
            d.value      = val.toDouble();
            d.type       = (uint8_t)type;
            d.linksCount = links.size();
            vec_varDouble.append(d);
        } else if (type == VAR_INT) {
            IntVariableData d{};
            strncpy(d.name, nm.toUtf8().constData(), sizeof(d.name)-1);
            d.value      = val.toInt();
            d.type       = (uint8_t)type;
            d.linksCount = links.size();
            vec_varInt.append(d);
        } else {
            StringVariableData d{};
            strncpy(d.name,  nm.toUtf8().constData(), sizeof(d.name)-1);
            strncpy(d.value, val.toString().toUtf8().constData(), sizeof(d.value)-1);
            d.type       = (uint8_t)type;
            d.linksCount = links.size();
            vec_varString.append(d);
        }
    }

    writeBlock(vec_varDouble);
    for (auto &v : vec_varDouble)
        for (auto &ln : varLinks[v.name])
            out.writeRawData(reinterpret_cast<const char *>(&ln), sizeof(ln));

    writeBlock(vec_varInt);
    for (auto &v : vec_varInt)
        for (auto &ln : varLinks[v.name])
            out.writeRawData(reinterpret_cast<const char *>(&ln), sizeof(ln));

    writeBlock(vec_varString);
    for (auto &v : vec_varString)
        for (auto &ln : varLinks[v.name])
            out.writeRawData(reinterpret_cast<const char *>(&ln), sizeof(ln));

    file.close();
    return true;
}

void Window::closeEvent(QCloseEvent *event) {
    event->accept();
}


void Window::onSimulationActionTriggered() {
    if (scene.simulator.isRunning())
        scene.simulator.stop();
    else
        scene.simulator.run();
}

void Window::updateSimulationActionToRun() {
    simulationAction->setText(tr("&Run"));
    simulationAction->setShortcut(QKeySequence(tr("Ctrl+R")));
}

void Window::updateSimulationActionToStop() {
    simulationAction->setText(tr("&Stop"));
    simulationAction->setShortcut(QKeySequence(tr("Ctrl+W")));
}





