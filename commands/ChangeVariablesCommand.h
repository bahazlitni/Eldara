#pragma once
#include "utils/Globals.h"

#include <QUndoCommand>

class VariablesTab;

class ChangeVariablesCommand : public QUndoCommand {
public:
    ChangeVariablesCommand(
        VariablesTab *variablesTab,
        const QVector<QString> &names,
        const QVector<QVariant> &newValues,
        QUndoCommand *parent = nullptr
    );

    void redo() override { changeVariables(true ); }
    void undo() override { changeVariables(false); }

private:
    void changeVariables(const bool intoNewValue);

    VariablesTab    *m_variablesTab      ;

    QVector<QString> m_dbl_names         ;
    QVector<double>  m_dbl_oldValues     ;
    QVector<double>  m_dbl_newValues     ;

    QVector<QString> m_int_names         ;
    QVector<int>     m_int_oldValues     ;
    QVector<int>     m_int_newValues     ;

    QVector<QString> m_str_names         ;
    QVector<QString> m_str_oldValues     ;
    QVector<QString> m_str_newValues     ;
};
