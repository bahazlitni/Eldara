#pragma once

#include "utils/Globals.h"

class InputGroup;
class DipoleGroup;
class MainPanel;

class SelectionTab : public QWidget {
    Q_OBJECT

public:
    explicit SelectionTab(MainPanel *mainPanel);
    MainPanel *mainPanel;

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
        const QVector<QVariant> &newValues
    );
    void onRemoveVariables(
        const QVector<QString> &names
    );

private:
    QVBoxLayout *mainLayout;

    QVector<InputGroup*> inputGroups;
    QScrollArea *scrollArea;
};
