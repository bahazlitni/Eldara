#include "Command.h"
#include "utils/Globals.h"

class Scene;
class SplitDipoleCommand: public Command {
private:
    SharedDipole splitted;
    SharedDipole resultant;
    SharedAlias splitter;
    SharedAlias resultantA;
    SharedAlias resultantB;
    SharedAlias splittedA;
    SharedAlias splittedB;

public:
    explicit SplitDipoleCommand(
        Scene *scene,
        const SharedDipole &splitted,
        const SharedDipole &resultant,
        const SharedAlias &splitter
    );

    ~SplitDipoleCommand() override;

    void execute() override;
    void undo() override;
};
