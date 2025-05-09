#include "dialogs/PreferencesDialog.h"
#include "utils/Globals.h"

#include <QSettings>
#include <QTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QColorDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QPalette>
#include <QString>
#include <QVariant>

#include <cstring>

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("Preferences"));
    resize(500, 400);

    tabWidget = new QTabWidget(this);
    buildSceneTab();
    buildPenToolTab();
    tabWidget->addTab(new QWidget(), tr("Hot Keys")); // Placeholder

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &PreferencesDialog::accept);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &PreferencesDialog::apply);
    connect(buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &PreferencesDialog::reject);
    mainLayout->addWidget(buttonBox);

    loadSettings();
    backupCurrentSettings();
}

void PreferencesDialog::buildSceneTab() {
    QWidget *tab = new QWidget;
    QFormLayout *layout = new QFormLayout(tab);

    bgColorBtn = new QPushButton;
    gridStrokeBtn = new QPushButton;
    tileSizeSpin = new QSpinBox;
    tileSizeSpin->setRange(Limits::MIN_TILESIZE, Limits::MAX_TILESIZE);
    tileSizeSpin->setSingleStep(10);
    snapPosCheck = new QCheckBox;
    allowMergeCheck = new QCheckBox;
    showGridCheck = new QCheckBox;
    displayRawCheck = new QCheckBox;

    layout->addRow(tr("Background Color"), bgColorBtn);
    layout->addRow(tr("Grid Stroke Color"), gridStrokeBtn);
    layout->addRow(tr("Tile Size"), tileSizeSpin);
    layout->addRow(tr("Snap Position"), snapPosCheck);
    layout->addRow(tr("Allow Merging"), allowMergeCheck);
    layout->addRow(tr("Show Grid"), showGridCheck);
    layout->addRow(tr("Display Raw Values"), displayRawCheck);

    tabWidget->addTab(tab, tr("Scene"));

    connect(bgColorBtn, &QPushButton::clicked, this, &PreferencesDialog::chooseSceneBackground);
    connect(gridStrokeBtn, &QPushButton::clicked, this, &PreferencesDialog::chooseSceneGridStroke);
}

void PreferencesDialog::buildPenToolTab() {
    QWidget *tab = new QWidget;
    QFormLayout *layout = new QFormLayout(tab);

    radiusSpin = new QSpinBox;
    aliasOutlineSpin = new QSpinBox;
    strokeWidthSpin = new QSpinBox;
    radiusSpin->setRange(Limits::MIN_ALIAS_RADIUS, Limits::MAX_ALIAS_RADIUS);
    aliasOutlineSpin->setRange(Limits::MIN_ALIAS_OUTLINE, Limits::MAX_ALIAS_OUTLINE);
    strokeWidthSpin->setRange(Limits::MIN_STROKE_WIDTH, Limits::MAX_STROKE_WIDTH);

    strokeColorBtn = new QPushButton;
    fillColorBtn = new QPushButton;
    showLabelCheck = new QCheckBox;
    allowSplitCheck = new QCheckBox;
    allowOnClickCheck = new QCheckBox;

    defaultResSpin = new QDoubleSpinBox;
    defaultResSpin->setDecimals(3);
    defaultResSpin->setRange(Limits::MIN_RESISTANCE_VALUE, Limits::MAX_RESISTANCE_VALUE);
    defaultResSpin->setSuffix(" Ω");

    defaultCapSpin = new QDoubleSpinBox;
    defaultCapSpin->setDecimals(12);
    defaultCapSpin->setRange(Limits::MIN_CAPACITANCE_VALUE, Limits::MAX_CAPACITANCE_VALUE);
    defaultCapSpin->setSuffix(" F");

    defaultIndSpin = new QDoubleSpinBox;
    defaultIndSpin->setDecimals(12);
    defaultIndSpin->setRange(Limits::MIN_INDUCTANCE_VALUE, Limits::MAX_INDUCTANCE_VALUE);
    defaultIndSpin->setSuffix(" H");

    defaultBattSpin = new QDoubleSpinBox;
    defaultBattSpin->setDecimals(3);
    defaultBattSpin->setRange(Limits::MIN_VOLTAGE_VALUE, Limits::MAX_VOLTAGE_VALUE);
    defaultBattSpin->setSuffix(" V");

    defaultDCVSpin = new QDoubleSpinBox;
    defaultDCVSpin->setDecimals(3);
    defaultDCVSpin->setRange(Limits::MIN_VOLTAGE_VALUE, Limits::MAX_VOLTAGE_VALUE);
    defaultDCVSpin->setSuffix(" V");

    defaultIntSpin = new QDoubleSpinBox;
    defaultIntSpin->setDecimals(9);
    defaultIntSpin->setRange(Limits::MIN_CURRENT_VALUE, Limits::MAX_CURRENT_VALUE);
    defaultIntSpin->setSuffix(" A");

    defaultQtySpin = new QDoubleSpinBox;
    defaultQtySpin->setDecimals(6);
    defaultQtySpin->setRange(Limits::MIN_QUANTITY_VALUE, Limits::MAX_QUANTITY_VALUE);
    defaultQtySpin->setSuffix(" C");

    layout->addRow(tr("Radius"), radiusSpin);
    layout->addRow(tr("Fill Color"), fillColorBtn);
    layout->addRow(tr("Stroke Color"), strokeColorBtn);
    layout->addRow(tr("Outline"), aliasOutlineSpin);
    layout->addRow(tr("Thickness"), strokeWidthSpin);
    layout->addRow(tr("Show Label"), showLabelCheck);
    layout->addRow(tr("Allow Splitting"), allowSplitCheck);
    layout->addRow(tr("Allow On-Click Coloring"), allowOnClickCheck);
    layout->addRow(tr("Default Resistance"), defaultResSpin);
    layout->addRow(tr("Default Capacitance"), defaultCapSpin);
    layout->addRow(tr("Default Inductance"), defaultIndSpin);
    layout->addRow(tr("Default Battery Voltage"), defaultBattSpin);
    layout->addRow(tr("Default DCV Voltage"), defaultDCVSpin);
    layout->addRow(tr("Default Intensity"), defaultIntSpin);
    layout->addRow(tr("Default Quantity"), defaultQtySpin);

    tabWidget->addTab(tab, tr("Pen Tool"));

    connect(strokeColorBtn, &QPushButton::clicked, this, &PreferencesDialog::choosePenStrokeColor);
    connect(fillColorBtn, &QPushButton::clicked, this, &PreferencesDialog::choosePenFillColor);
}

void PreferencesDialog::apply() {
    saveSettings();
    emit settingsApplied();
}

void PreferencesDialog::accept() {
    apply();
    QDialog::accept();
}

void PreferencesDialog::reject() {
    restoreBackupSettings();
    QDialog::reject();
}

void PreferencesDialog::loadSettings() {
    QSettings s("EldaraSoft", "Eldara");

    updateColorButton(bgColorBtn, s.value("scene/background", QColor(33, 33, 33)).value<QColor>());
    updateColorButton(gridStrokeBtn, s.value("scene/gridStroke", QColor(44, 44, 44)).value<QColor>());
    tileSizeSpin->setValue(s.value("scene/tileSize", 50).toInt());
    snapPosCheck->setChecked(s.value("scene/snapPosition", true).toBool());
    allowMergeCheck->setChecked(s.value("scene/allowMerge", true).toBool());
    showGridCheck->setChecked(s.value("scene/showGrid", true).toBool());
    displayRawCheck->setChecked(s.value("scene/displayRaw", false).toBool());

    radiusSpin->setValue(s.value("pen/radius", 8).toInt());
    strokeWidthSpin->setValue(s.value("pen/strokeWidth", 1).toInt());
    aliasOutlineSpin->setValue(s.value("pen/aliasOutline", 0).toInt());
    updateColorButton(strokeColorBtn, s.value("pen/strokeColor", QColor("#CCC")).value<QColor>());
    updateColorButton(fillColorBtn, s.value("pen/fillColor", QColor("#FFF")).value<QColor>());
    showLabelCheck->setChecked(s.value("pen/showLabel", true).toBool());
    allowSplitCheck->setChecked(s.value("pen/allowSplit", true).toBool());
    allowOnClickCheck->setChecked(s.value("pen/allowOnClickColor", true).toBool());
    defaultResSpin->setValue(s.value("pen/defaultResistance", 1e3).toDouble());
    defaultCapSpin->setValue(s.value("pen/defaultCapacitance", 1e-8).toDouble());
    defaultIndSpin->setValue(s.value("pen/defaultInductance", 1e-4).toDouble());
    defaultBattSpin->setValue(s.value("pen/defaultBatteryVoltage", 5.0).toDouble());
    defaultDCVSpin->setValue(s.value("pen/defaultDCVVoltage", 15.0).toDouble());
    defaultIntSpin->setValue(s.value("pen/defaultIntensity", 1e-3).toDouble());
    defaultQtySpin->setValue(s.value("pen/defaultQuantity", 1.0).toDouble());
}

void PreferencesDialog::saveSettings() {
    QSettings s("EldaraSoft", "Eldara");

    s.setValue("scene/background", bgColorBtn->palette().button().color());
    s.setValue("scene/gridStroke", gridStrokeBtn->palette().button().color());
    s.setValue("scene/tileSize", tileSizeSpin->value());
    s.setValue("scene/snapPosition", snapPosCheck->isChecked());
    s.setValue("scene/allowMerge", allowMergeCheck->isChecked());
    s.setValue("scene/showGrid", showGridCheck->isChecked());
    s.setValue("scene/displayRaw", displayRawCheck->isChecked());

    s.setValue("pen/radius", radiusSpin->value());
    s.setValue("pen/strokeWidth", strokeWidthSpin->value());
    s.setValue("pen/aliasOutline", aliasOutlineSpin->value());
    s.setValue("pen/strokeColor", strokeColorBtn->palette().button().color());
    s.setValue("pen/fillColor", fillColorBtn->palette().button().color());
    s.setValue("pen/showLabel", showLabelCheck->isChecked());
    s.setValue("pen/allowSplit", allowSplitCheck->isChecked());
    s.setValue("pen/allowOnClickColor", allowOnClickCheck->isChecked());
    s.setValue("pen/defaultResistance", defaultResSpin->value());
    s.setValue("pen/defaultCapacitance", defaultCapSpin->value());
    s.setValue("pen/defaultInductance", defaultIndSpin->value());
    s.setValue("pen/defaultBatteryVoltage", defaultBattSpin->value());
    s.setValue("pen/defaultDCVVoltage", defaultDCVSpin->value());
    s.setValue("pen/defaultIntensity", defaultIntSpin->value());
    s.setValue("pen/defaultQuantity", defaultQtySpin->value());
}

void PreferencesDialog::backupCurrentSettings() {
    QSettings s("EldaraSoft", "Eldara");
    for (const QString &key : s.allKeys()) {
        backup[key] = s.value(key);
    }
}

void PreferencesDialog::restoreBackupSettings() {
    QSettings s("EldaraSoft", "Eldara");
    s.clear();
    for (auto it = backup.begin(); it != backup.end(); ++it) {
        s.setValue(it.key(), it.value());
    }
}

void PreferencesDialog::updateColorButton(QPushButton *btn, const QColor &color) {
    QPalette pal = btn->palette();
    pal.setColor(QPalette::Button, color);
    btn->setAutoFillBackground(true);
    btn->setPalette(pal);
    btn->update();
}

void PreferencesDialog::chooseSceneBackground() {
    QColor color = QColorDialog::getColor(bgColorBtn->palette().button().color(), this);
    if (color.isValid()) updateColorButton(bgColorBtn, color);
}

void PreferencesDialog::chooseSceneGridStroke() {
    QColor color = QColorDialog::getColor(gridStrokeBtn->palette().button().color(), this);
    if (color.isValid()) updateColorButton(gridStrokeBtn, color);
}

void PreferencesDialog::choosePenStrokeColor() {
    QColor color = QColorDialog::getColor(strokeColorBtn->palette().button().color(), this);
    if (color.isValid()) updateColorButton(strokeColorBtn, color);
}

void PreferencesDialog::choosePenFillColor() {
    QColor color = QColorDialog::getColor(fillColorBtn->palette().button().color(), this);
    if (color.isValid()) updateColorButton(fillColorBtn, color);
}
