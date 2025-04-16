#pragma once
#include "utils/Globals.h"

class MainPanel;
class Scene;
class VariablesTab : public QWidget {
    Q_OBJECT

private:
    // Helper functions working directly with table items.
    int rowOfTypeComboBox(QComboBox *typeCombo) const;
    bool containsName(const QString &name, int ignoreRow = -1);
    int rowOfName(const QString &name) const;
    int suffix(const QString &key);
    void removeRow(const int row);
    void buildRow(const int row, const QString &name, const QVariant &value, const VariableType type);

public:
    MainPanel *mainPanel;

    explicit VariablesTab(MainPanel *mainPanel);
    void updateData();

    QVector<QString> names() const;
    QVector<QString> names(const VariableType type) const;
    QVector<QVariant> values() const;
    QVector<QVariant> values(const VariableType type) const;
    QVector<QPair<QString, QVariant>> filter(const VariableType type) const;

    QVariant value(const QString &name) const { return value(rowOfName(name)); }
    VariableType vartype(const QString &name) const { return vartype(rowOfName(name)); }
    QVariant value(const int row) const;
    VariableType vartype(const int row) const;

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

    void addVariables(
        const QVector<QString> &names,
        const QVector<QVariant> &values,
        const QVector<VariableType> &types
    );

    void removeVariables(
        const QVector<QString> &names
    );

    void changeVariables(
        const QVector<QString> &names,
        const QVector<QVariant> &newValues
    );

private slots:
    void onCellChanged(const int row, int col);
    void onCellDoubleClicked(const int row, int col);

    void onAddVariable();
    void onRemoveVariable();

private:
    QTableWidget *table;
    QPushButton *addButton;
    QPushButton *removeButton;
    QLabel *noDataLabel;

    VariableType lastEnteredType;

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
        const QVector<QVariant> &newValues
    );
    inline void exe_removeVariables(const QVector<QString> &names);
    inline void exe_addVariables(
        const QVector<QString> &names,
        const QVector<QVariant> &values,
        const QVector<VariableType> &types
    );
    inline void exe_replaceVariables(
        const QVector<QString> &oldNames,
        const QVector<QString> &newNames,
        const QVector<QVariant> &newValues,
        const QVector<VariableType> &newTypes
    );
};
