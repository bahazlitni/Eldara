#pragma once
#include "Command.h"
#include "utils/Types.h"

#include <QSet>


#include <QHash>


class App;
class AddVariablesCommand: public Command {
public:
    AddVariablesCommand(
        App *app,
        const QVector<QString> &names,
        const QVector<QVariant> values,
        const QVector<VariableType> types
    ):
        Command(app),
        _names(names),
        _values(values),
        _types(types)
    {}

    void execute() override;
    void undo() override;

private:
    const QVector<QString> _names;
    const QVector<QVariant> _values;
    const QVector<VariableType> _types;
};
