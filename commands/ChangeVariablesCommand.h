#pragma once
#include "Command.h"
#include "utils/Types.h"

#include <QSet>


#include <QHash>


class App;
class ChangeVariablesCommand: public Command {
public:
    ChangeVariablesCommand(
        App *app,
        const QVector<QString> &names,
        const QVector<QVariant> newValues,
        const QVector<VariableType> types
    );

    void execute() override;
    void undo() override;

private:
    const QVector<QString> _names;
    const QVector<QVariant> _oldValues;
    const QVector<QVariant> _newValues;
    const QVector<VariableType> _types;
};
