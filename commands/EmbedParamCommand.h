#pragma once
#include "Command.h"
#include "utils/Globals.h"

class VariablesTab;
class EmbedParamCommand: public Command {
public:
    EmbedParamCommand(
        VariablesTab *variablesTab,
        const Selection &selection,
        const QString &name,
        const Param param
    );
    void execute() override;
    void undo() override;

private:
    struct Cache {
        SharedDipole dipole;
        double value;
        QString name;

        Cache(
            const SharedDipole &d,
            const double v,
            const QString &n
        ): dipole(d), value(v), name(n) {}
    };

    VariablesTab *_variablesTab;
    QVector<Cache> _cache;
    const QString _name;
    const Param _param;
};
