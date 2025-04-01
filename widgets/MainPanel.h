#pragma once

#include <QGroupBox>
#include <QSet>
#include <QVBoxLayout>
#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include "utils/Types.h"

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

public slots:
    void onAddVariables(
        const QVector<QString> &names,
        const QVector<QVariant> &values,
        const QVector<VariableType> &types
    );
    void onChangeVariables(
        const QVector<QString> &names,
        const QVector<QVariant> &newValues,
        const QVector<VariableType> &types
    );
    void onRemoveVariables(
        const QVector<QString> &names,
        const QVector<VariableType> &types
    );

private:
    const QString qssPath = ":widgets/styles/MainPanel.qss";
    void loadStyleSheet();
};
