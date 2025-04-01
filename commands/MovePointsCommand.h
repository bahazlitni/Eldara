#include "Command.h"
#include "utils/Types.h"

class App;
class MovePointsCommand: public Command {
public:
    explicit MovePointsCommand(
        App *app, const MovementMap &movementMap
    );

    void undo() override;
    void execute() override;

private:
    const MovementMap movementMap;
};
