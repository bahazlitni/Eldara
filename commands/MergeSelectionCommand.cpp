#include "MergeSelectionCommand.h"
#include "utils/Selection.h"
#include "App.h"

#include "objects/Object.h"
#include "objects/Alias.h"
#include "objects/Dipole.h"

MergeSelectionCommand::MergeSelectionCommand(App *app, const MergeMap &mergeMap):
    Command(app), _mergeMap(mergeMap) {
    for(const auto &merged : _mergeMap.keys()){
        switch(merged->category()){
        case _NODE: {
            const auto &mergedAlias = std::static_pointer_cast<Alias>(merged);
            for(auto dipole : mergedAlias->connections())
                _dipolesConfig.insert(dipole, qMakePair(dipole->A(), dipole->B()));
            break;
        }
        case _DIPOLE: {
            const auto &mergedDipole = std::static_pointer_cast<Dipole>(merged);
            _dipolesConfig.insert(mergedDipole, qMakePair(mergedDipole->A(), mergedDipole->B()));
            break;
        }
        default:
            break;
        }
    }
}


void MergeSelectionCommand::execute(){
    Command::execute();
    for(const auto &merged : _mergeMap.keys()){
        if(!merged) continue;
        switch(merged->category()){
        case _NODE: {
            const auto &mergedAlias = std::static_pointer_cast<Alias>(merged);
            const auto &mergerAlias = std::static_pointer_cast<Alias>(_mergeMap[merged]);
            for(auto dipole : mergedAlias->connections()){
                if(dipole->A() == mergedAlias) dipole->setA(mergerAlias);
                if(dipole->B() == mergedAlias) dipole->setB(mergerAlias);
                mergedAlias->disconnect(dipole);
                mergerAlias->connect(dipole);
            }
            break;
        }
        case _DIPOLE: {
            const auto &mergedDipole = std::static_pointer_cast<Dipole>(merged);
            if(const auto &A = mergedDipole->A()) A->disconnect(mergedDipole);
            if(const auto &B = mergedDipole->B()) B->disconnect(mergedDipole);
            break;
        }
        default:
            break;
        }
        app->deepRemoval(merged);
    }
}

void MergeSelectionCommand::undo(){
    Command::execute();
    for(const auto &merged : _mergeMap.keys()){
        if(!merged || merged->category() != _NODE) continue;
        const auto &mergedAlias = std::static_pointer_cast<Alias>(merged);
        app->aliases.insert(mergedAlias->id(), mergedAlias);
    }
    for(auto dipole : _dipolesConfig.keys()){
        const auto &dipoleConfig = _dipolesConfig[dipole];
        dipole->A()->disconnect(dipole);
        dipole->B()->disconnect(dipole);
        dipole->setA(dipoleConfig.first);
        dipole->setB(dipoleConfig.second);
        if(dipoleConfig.first) dipoleConfig.first->connect(dipole);
        if(dipoleConfig.second) dipoleConfig.second->connect(dipole);
        dipole->dirtyVisibleCheckFlag = app->grid.getDirtyVisibleCheckFlagInitial();
    }
}
