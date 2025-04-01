#pragma once

#include "utils/Types.h"
#include <QGroupBox>
#include <QSet>
#include <QVBoxLayout>
#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QScrollArea>

class App;
class ObjectGroup;
class ColorsGroup;
class AliasGroup;
class UnitDipoleGroup;
class SelectionTab : public QWidget {
    Q_OBJECT

public:
    explicit SelectionTab(App *app, QWidget *parent = nullptr);
    App *app;

public slots:
    void updateData();
    void updateCoordinates();

    void onAddVariables(
        const QVector<QString> &names,
        const QVector<QVariant> &values,
        const QVector<VariableType> &types
    );
    void onChangeVariables(
        const QVector<QString> &names,
        const QVector<QVariant> &newValues,
        [[maybe_unused]] const QVector<VariableType> &types
    );
    void onRemoveVariables(
        const QVector<QString> &names,
        [[maybe_unused]] const QVector<VariableType> &types
    );

private:
    QVBoxLayout *mainLayout;
    QVector<ObjectGroup*> objectGroups;
    QVector<UnitDipoleGroup*> unitDipoleGroups;
    ColorsGroup *colorsGroup;
    QScrollArea *scrollArea;
};
