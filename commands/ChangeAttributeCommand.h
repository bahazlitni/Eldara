#include "Command.h"
#include "utils/Types.h"

class App;
class ChangeAttributeCommand: public Command {
public:
    explicit ChangeAttributeCommand(App *app, const LockedSelection &selection, const QString key, const QString value);

    void execute() override;
    void undo() override;

private:
    LockedSelection _selection;
    QHash<SharedObject, QString> oldValues;
    const QString key;
    const QString value;
};
