#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QLabel>
#include <QComboBox>
#include "utils/Types.h"

class App;

class VariablesTab : public QWidget {
    Q_OBJECT

public:
    explicit VariablesTab(App *app, QWidget *parent = nullptr);
    void updateData();

    QVector<QString> names() const;
    QVector<QString> names(const VariableType type) const;
    QVector<QVariant> values() const;
    QVector<QVariant> values(const VariableType type) const;
    QVector<QPair<QString, QVariant>> filter(const VariableType type) const;

    QString firstName() const;
    QString firstName(const VariableType type) const;
    QVariant firstValue() const;
    QVariant firstValue(const VariableType type) const;
    QPair<QString, QVariant> first() const;
    QPair<QString, QVariant> first(const VariableType type) const;

    bool isEmpty() const;
    bool isEmpty(const VariableType type) const;

    int count() const;
    int count(const VariableType type) const;

public slots:
    void onAddVariable();
    void onRemoveVariable();


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

    void onCellChanged(const int row, int col);
    void onCellDoubleClicked(const int row, int col);

private:
    App *app;
    QTableWidget *table;
    QPushButton *addButton;
    QPushButton *removeButton;
    QLabel *noDataLabel;

    VariableType lastEnteredType;

    // Helper functions working directly with table items.
    int rowOfTypeComboBox(QComboBox *typeCombo);
    bool containsName(const QString &name, int ignoreRow = -1);
    int rowOfName(const QString &name);
    int suffix(const QString &key);
    void removeRow(const int row);
    void buildRow(const int row, const QString &name, const QVariant &value, const VariableType type);

    void validateNewData(
        const QString &originalName,
        const QVariant &originalValue,
        const VariableType type,
        QString *newName,
        QVariant *newValue
    );

    QHash<QString, int> rowsCache;

    inline void exe_changeVariables(
        const QVector<QString> &names,
        const QVector<QVariant> newValues,
        const QVector<VariableType> types
    );
    inline void exe_removeVariables(
        const QVector<QString> &names,
        const QVector<VariableType> types
    );
    inline void exe_addVariables(
        const QVector<QString> &names,
        const QVector<QVariant> values,
        const QVector<VariableType> types
    );
    inline void exe_replaceVariables(
        const QVector<QString> &oldNames,
        const QVector<QString> &newNames,
        const QVector<QVariant> newValues,
        const QVector<VariableType> oldTypes,
        const QVector<VariableType> newTypes
    );
};
