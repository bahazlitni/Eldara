#pragma once
#include "utils/Globals.h"

class VariablesTab;

class RemoveVariablesCommand : public QUndoCommand {
public:
    RemoveVariablesCommand(
        VariablesTab *variablesTab,
        const QVector<QString> &names,
        QUndoCommand *parent = nullptr
    );

    void redo() override;
    void undo() override;

private:
    VariablesTab    *m_variablesTab;

    QHash<QString, QVector<QPair<SharedDipole, QVector<Param>>>> m_dipoles;

    QVector<QString>      m_dbl_names  ;
    QVector<double>       m_dbl_values ;
    QVector<VariableType> m_dbl_types  ;

    QVector<QString>      m_int_names  ;
    QVector<int>          m_int_values ;

    QVector<QString>      m_str_names  ;
    QVector<QString>      m_str_values ;
};
