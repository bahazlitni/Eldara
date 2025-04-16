#include "Command.h"
#include "utils/Globals.h"

class Scene;
class MovePointsCommand: public Command {
public:
    explicit MovePointsCommand(
        Scene *scene, const MovementMap &movementMap
    );

    void undo() override;
    void execute() override;

private:
    const MovementMap movementMap;
};
