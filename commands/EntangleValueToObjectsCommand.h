#pragma once
#include "Command.h"
#include "utils/Types.h"

#include <QSet>


#include <QHash>


class App;
class EntangleValueToObjectsCommand: public Command {
public:
    EntangleValueToObjectsCommand(App *app, const LockedSelection &selection, const QString &name, const VariableType type);
    void execute() override;
    void undo() override;

private:
    const LockedSelection _selection;
    QVector<QPair<SharedUnitDipole, SharedDouble>> _cache;
    const QString _name;
    const VariableType _type;
};
