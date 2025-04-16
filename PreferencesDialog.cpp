#include "dialogs/PreferencesDialog.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QSettings>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle(tr("Preferences"));
    tabWidget = new QTabWidget(this);

    // Scene tab
    {
        QWidget *sceneTab = new QWidget;
        QVBoxLayout *sceneLayout = new QVBoxLayout(sceneTab);
        QHBoxLayout *bgLayout = new QHBoxLayout;
        bgLayout->addWidget(new QLabel(tr("Background Color:")));
        bgColorBtn = new QPushButton;
        bgLayout->addWidget(bgColorBtn);
        QHBoxLayout *gridLayout = new QHBoxLayout;
        gridLayout->addWidget(new QLabel(tr("Grid Stroke Color:")));
        gridStrokeBtn = new QPushButton;
        gridLayout->addWidget(gridStrokeBtn);
        sceneLayout->addLayout(bgLayout);
        sceneLayout->addLayout(gridLayout);
        sceneLayout->addWidget(new QLabel(tr("Tile Size:")));
        tileSizeSpin = new QSpinBox;
        tileSizeSpin->setRange(1, 1000);
        sceneLayout->addWidget(tileSizeSpin);
        snapPosCheck = new QCheckBox(tr("Snap Position"));
        showGridCheck = new QCheckBox(tr("Show Grid"));
        displayRawCheck = new QCheckBox(tr("Display Raw Values"));
        sceneLayout->addWidget(snapPosCheck);
        sceneLayout->addWidget(showGridCheck);
        sceneLayout->addWidget(displayRawCheck);
        tabWidget->addTab(sceneTab, tr("Scene"));
    }

    // Pen Tool tab
    {
        QWidget *penTab = new QWidget;
        QVBoxLayout *penLayout = new QVBoxLayout(penTab);
        penLayout->addWidget(new QLabel(tr("Stroke Width:")));
        strokeWidthSpin = new QSpinBox;
        strokeWidthSpin->setRange(1, 5);
        penLayout->addWidget(strokeWidthSpin);
        QHBoxLayout *strokeColorLayout = new QHBoxLayout;
        strokeColorLayout->addWidget(new QLabel(tr("Stroke Color:")));
        strokeColorBtn = new QPushButton;
        strokeColorLayout->addWidget(strokeColorBtn);
        penLayout->addLayout(strokeColorLayout);
        QHBoxLayout *fillColorLayout = new QHBoxLayout;
        fillColorLayout->addWidget(new QLabel(tr("Fill Color:")));
        fillColorBtn = new QPushButton;
        fillColorLayout->addWidget(fillColorBtn);
        penLayout->addLayout(fillColorLayout);
        allowSplitCheck = new QCheckBox(tr("Allow Splitting"));
        allowMergeCheck = new QCheckBox(tr("Allow Merging"));
        allowOnClickCheck = new QCheckBox(tr("Allow On-Click Coloring"));
        penLayout->addWidget(allowSplitCheck);
        penLayout->addWidget(allowMergeCheck);
        penLayout->addWidget(allowOnClickCheck);
        auto addDefaultField = [&](const QString &labelText, QDoubleSpinBox *&spin, int decimals) {
            QHBoxLayout *layout = new QHBoxLayout;
            layout->addWidget(new QLabel(labelText));
            spin = new QDoubleSpinBox;
            spin->setDecimals(decimals);
            layout->addWidget(spin);
            penLayout->addLayout(layout);
        };
        addDefaultField(tr("Default Resistance (Ω):"), defaultResSpin, 6);
        addDefaultField(tr("Default Capacitance (F):"), defaultCapSpin, 10);
        addDefaultField(tr("Default Inductance (H):"), defaultIndSpin, 8);
        addDefaultField(tr("Default Battery Voltage (V):"), defaultBattSpin, 2);
        addDefaultField(tr("Default DC Voltage (V):"), defaultDCVSpin, 2);
        addDefaultField(tr("Default Intensity (A):"), defaultIntSpin, 6);
        addDefaultField(tr("Default Quantity (C):"), defaultQtySpin, 2);
        tabWidget->addTab(penTab, tr("Pen Tool"));
    }

    // Hot Keys tab
    {
        QWidget *hotTab = new QWidget;
        QVBoxLayout *hotLayout = new QVBoxLayout(hotTab);
        hotLayout->addWidget(new QLabel(tr("No hot keys defined yet.")));
        tabWidget->addTab(hotTab, tr("Hot Keys"));
    }

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &PreferencesDialog::reject);
    mainLayout->addWidget(buttonBox);

    // Signals
    connect(bgColorBtn, &QPushButton::clicked, this, &PreferencesDialog::chooseSceneBackground);
    connect(gridStrokeBtn, &QPushButton::clicked, this, &PreferencesDialog::chooseSceneGridStroke);
    connect(strokeColorBtn, &QPushButton::clicked, this, &PreferencesDialog::choosePenStrokeColor);
    connect(fillColorBtn, &QPushButton::clicked, this, &PreferencesDialog::choosePenFillColor);

    loadSettings();
}

void PreferencesDialog::updateColorButton(QPushButton *btn, const QColor &color) {
    btn->setStyleSheet(QString("background-color: %1;").arg(color.name()));
    btn->setProperty("chosenColor", color);
}

void PreferencesDialog::chooseSceneBackground() {
    QColor c = QColorDialog::getColor(bgColorBtn->property("chosenColor").value<QColor>(), this);
    if (c.isValid()) updateColorButton(bgColorBtn, c);
}

void PreferencesDialog::chooseSceneGridStroke() {
    QColor c = QColorDialog::getColor(gridStrokeBtn->property("chosenColor").value<QColor>(), this);
    if (c.isValid()) updateColorButton(gridStrokeBtn, c);
}

void PreferencesDialog::choosePenStrokeColor() {
    QColor c = QColorDialog::getColor(strokeColorBtn->property("chosenColor").value<QColor>(), this);
    if (c.isValid()) updateColorButton(strokeColorBtn, c);
}

void PreferencesDialog::choosePenFillColor() {
    QColor c = QColorDialog::getColor(fillColorBtn->property("chosenColor").value<QColor>(), this);
    if (c.isValid()) updateColorButton(fillColorBtn, c);
}

void PreferencesDialog::loadSettings() {
    QSettings settings;
    // Scene
    updateColorButton(bgColorBtn, settings.value("Scene/BackgroundColor", QColor(33,33,33)).value<QColor>());
    updateColorButton(gridStrokeBtn, settings.value("Scene/GridStrokeColor", QColor(44,44,44)).value<QColor>());
    tileSizeSpin->setValue(settings.value("Scene/TileSize", 50).toInt());
    snapPosCheck->setChecked(settings.value("Scene/SnapPosition", true).toBool());
    showGridCheck->setChecked(settings.value("Scene/ShowGrid", true).toBool());
    displayRawCheck->setChecked(settings.value("Scene/DisplayRawValues", false).toBool());
    // Pen
    strokeWidthSpin->setValue(settings.value("PenTool/StrokeWidth", 1).toInt());
    updateColorButton(strokeColorBtn, settings.value("PenTool/StrokeColor", QColor("#CCC")).value<QColor>());
    updateColorButton(fillColorBtn, settings.value("PenTool/FillColor", QColor("#FFF")).value<QColor>());
    allowSplitCheck->setChecked(settings.value("PenTool/AllowSplitting", true).toBool());
    allowMergeCheck->setChecked(settings.value("PenTool/AllowMerging", true).toBool());
    allowOnClickCheck->setChecked(settings.value("PenTool/AllowOnClickColoring", true).toBool());
    defaultResSpin->setValue(settings.value("PenTool/DefaultResistance", 1e3).toDouble());
    defaultCapSpin->setValue(settings.value("PenTool/DefaultCapacitance", 1e-8).toDouble());
    defaultIndSpin->setValue(settings.value("PenTool/DefaultInductance", 1e-4).toDouble());
    defaultBattSpin->setValue(settings.value("PenTool/DefaultBatteryVoltage", 5.0).toDouble());
    defaultDCVSpin->setValue(settings.value("PenTool/DefaultDCVoltage", 15.0).toDouble());
    defaultIntSpin->setValue(settings.value("PenTool/DefaultIntensity", 1e-3).toDouble());
    defaultQtySpin->setValue(settings.value("PenTool/DefaultQuantity", 1.0).toDouble());
}

void PreferencesDialog::saveSettings() {
    QSettings settings;
    // Scene
    settings.setValue("Scene/BackgroundColor", bgColorBtn->property("chosenColor").value<QColor>());
    settings.setValue("Scene/GridStrokeColor", gridStrokeBtn->property("chosenColor").value<QColor>());
    settings.setValue("Scene/TileSize", tileSizeSpin->value());
    settings.setValue("Scene/SnapPosition", snapPosCheck->isChecked());
    settings.setValue("Scene/ShowGrid", showGridCheck->isChecked());
    settings.setValue("Scene/DisplayRawValues", displayRawCheck->isChecked());
    // Pen Tool
    settings.setValue("PenTool/StrokeWidth", strokeWidthSpin->value());
    settings.setValue("PenTool/StrokeColor", strokeColorBtn->property("chosenColor").value<QColor>());
    settings.setValue("PenTool/FillColor", fillColorBtn->property("chosenColor").value<QColor>());
    settings.setValue("PenTool/AllowSplitting", allowSplitCheck->isChecked());
    settings.setValue("PenTool/AllowMerging", allowMergeCheck->isChecked());
    settings.setValue("PenTool/AllowOnClickColoring", allowOnClickCheck->isChecked());
    settings.setValue("PenTool/DefaultResistance", defaultResSpin->value());
    settings.setValue("PenTool/DefaultCapacitance", defaultCapSpin->value());
    settings.setValue("PenTool/DefaultInductance", defaultIndSpin->value());
    settings.setValue("PenTool/DefaultBatteryVoltage", defaultBattSpin->value());
    settings.setValue("PenTool/DefaultDCVoltage", defaultDCVSpin->value());
    settings.setValue("PenTool/DefaultIntensity", defaultIntSpin->value());
    settings.setValue("PenTool/DefaultQuantity", defaultQtySpin->value());
}

void PreferencesDialog::accept() {
    saveSettings();
    QDialog::accept();
}
