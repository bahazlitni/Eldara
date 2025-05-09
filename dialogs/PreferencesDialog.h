#pragma once
#include <QDialog>
#include <QColor>
#include <QVariantMap>

class QWidget;
class QTabWidget;
class QSpinBox;
class QCheckBox;
class QDoubleSpinBox;
class QPushButton;
class QDialogButtonBox;

class PreferencesDialog : public QDialog {
    Q_OBJECT
public:
    explicit PreferencesDialog(QWidget *parent = nullptr);

signals:
    void settingsApplied();

private slots:
    void chooseSceneBackground();
    void chooseSceneGridStroke();
    void choosePenStrokeColor();
    void choosePenFillColor();
    void accept() override;
    void apply();
    void reject() override;

private:
    void buildSceneTab();
    void buildPenToolTab();
    void loadSettings();
    void saveSettings();
    void updateColorButton(QPushButton *btn, const QColor &color);
    void backupCurrentSettings();
    void restoreBackupSettings();

    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;
    QVariantMap backup;

    // Scene widgets
    QPushButton *bgColorBtn;
    QPushButton *gridStrokeBtn;
    QSpinBox *tileSizeSpin;
    QCheckBox *snapPosCheck;
    QCheckBox *allowMergeCheck;
    QCheckBox *showGridCheck;
    QCheckBox *displayRawCheck;

    // Pen tool widgets
    QSpinBox *radiusSpin;
    QSpinBox *aliasOutlineSpin;
    QSpinBox *strokeWidthSpin;
    QPushButton *strokeColorBtn;
    QPushButton *fillColorBtn;
    QCheckBox *showLabelCheck;
    QCheckBox *allowSplitCheck;
    QCheckBox *allowOnClickCheck;
    QDoubleSpinBox *defaultResSpin;
    QDoubleSpinBox *defaultCapSpin;
    QDoubleSpinBox *defaultIndSpin;
    QDoubleSpinBox *defaultBattSpin;
    QDoubleSpinBox *defaultDCVSpin;
    QDoubleSpinBox *defaultIntSpin;
    QDoubleSpinBox *defaultQtySpin;
};
