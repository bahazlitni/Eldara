#pragma once
#include "utils/Globals.h"

class VariablesTab;
class EmbedParamCommand : public QUndoCommand {
public:
    EmbedParamCommand(
        VariablesTab *variablesTab,
        const Selection &selection,
        const QString &name,
        Param param,
        QUndoCommand *parent = nullptr
    );

    void redo() override;
    void undo() override;

private:
    struct Cache {
        SharedDipole dipole;
        double       oldValue;
        QString      oldName;
    };

    VariablesTab       *m_variablesTab;
    QVector<Cache>      m_cache;
    QString             m_name;
    Param               m_param;
};
