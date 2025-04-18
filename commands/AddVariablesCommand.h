#pragma once
#include "utils/Globals.h"

#include <QVector>
#include <QString>
#include <QVariant>

#include <QUndoCommand>

class VariablesTab;

class AddVariablesCommand : public QUndoCommand {
public:
    AddVariablesCommand(
        VariablesTab *variablesTab,
        const QVector<QString> &names,
        const QVector<QVariant> &values,
        const QVector<VariableType> &types,
        QUndoCommand *parent = nullptr
    );

    void redo() override;
    void undo() override;

private:
    VariablesTab         *m_variablesTab;
    QVector<QString>      m_names;
    QVector<QVariant>     m_values;
    QVector<VariableType> m_types;
};
