#pragma once
#include "utils/Globals.h"

#include <QUndoCommand>

class Scene;

class SplitDipoleCommand: public QUndoCommand {
private:
    Scene *scene;

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
        const SharedAlias &splitter,
        QUndoCommand *parent = nullptr
    );

    void redo() override;
    void undo() override;
};
