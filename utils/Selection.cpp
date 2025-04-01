#include "objects/Alias.h"
#include "objects/Dipole.h"
#include "objects/Wire.h"

#include "objects/BCPoint.h"
#include "objects/BCControlPoint.h"
#include "objects/BCPath.h"

#include "App.h"
#include "Grid.h"

inline void ProcessAlias(
    QSet<SharedPoint> *set,
    QSet<SharedAlias> *processedAliases,
    const SharedAlias &alias
    ){
    processedAliases->insert(alias);
    set->insert(alias->share());

    for (const auto &dipole : alias->connections()){
        if(dipole->type() != WIRE) continue;
        const auto &wire = std::static_pointer_cast<Wire>(dipole);
        const auto &bcp = wire->path();
        if(bcp.isEmpty()) continue;

        const auto &A = wire->A();
        const auto &B = wire->B();

        if(processedAliases->contains(wire->other(alias))){
            auto cur = bcp.first();
            while(cur){
                if(const auto &ctrlBefore = cur->before())
                    set->insert(ctrlBefore->share());
                set->insert(cur->share());
                if(const auto &ctrlAfter = cur->after())
                    set->insert(ctrlAfter->share());
                cur = cur->next();
            }
        } else {
            if(alias == A && bcp.first())
                if(bcp.first()->after()) set->insert(bcp.first()->after()->share());
            if(alias == B && bcp.last())
                if(bcp.last()->before()) set->insert(bcp.last()->before()->share());
        }
    }
}

QSet<SharedPoint> GetWorldPointSelection(const LockedSelection &selection){
    QSet<SharedPoint> set;
    QSet<SharedAlias> processedAliases;
    for (const auto &obj : selection){
        if (obj->category() == _NODE){
            SharedAlias alias = std::static_pointer_cast<Alias>(obj);
            if (alias && !processedAliases.contains(alias))
                ProcessAlias(&set, &processedAliases, alias);
        }
        else if (obj->category() == _DIPOLE) {
            auto dipole = std::static_pointer_cast<Dipole>(obj);
            if (dipole) {
                auto a = dipole->A();
                auto b = dipole->B();
                if (a && !processedAliases.contains(a))
                    ProcessAlias(&set, &processedAliases, a);
                if (b && !processedAliases.contains(b))
                    ProcessAlias(&set, &processedAliases, b);
            }
        } else if(obj->type() == BC_POINT){
            auto bcpoint = std::static_pointer_cast<BCPoint>(obj);
            set.insert(bcpoint->share());
            if(const auto &ctrlBefore = bcpoint->before())
                set.insert(ctrlBefore->share());
            if(const auto &ctrlAfter = bcpoint->after())
                set.insert(ctrlAfter->share());
        } else if(obj->type() == BC_CONTROL_POINT){
            auto ctrl = std::static_pointer_cast<BCControlPoint>(obj);
            set.insert(ctrl->share());
        }
    }
    return set;
}

LockedSelection GetDeepSelection(const LockedSelection &selection){
    LockedSelection slc;
    slc.reserve(selection.size());
    for(const auto &obj : selection){
        switch(obj->category()){
        case _NODE: {
            const auto &alias = static_pointer_cast<Alias>(obj);
            for(const auto &dipole : alias->connections()) {
                if(slc.contains(dipole)) continue;
                slc.insert(dipole);
                if(const auto &A = dipole->A()) slc.insert(A);
                if(const auto &B = dipole->B()) slc.insert(B);
            }
            break;
        }
        case _DIPOLE: {
            const auto &dipole = static_pointer_cast<Dipole>(obj);
            if(slc.contains(dipole)) continue;
            if(const auto &A = dipole->A()) slc.insert(A);
            if(const auto &B = dipole->B()) slc.insert(B);
            break;
        }
        default:
            break;
        }
        slc.insert(obj);
    }
    return slc;
}


inline SharedAlias getAliasCopy(
    App *app,
    LockedSelection &copy,
    QHash<SharedAlias, SharedAlias> &aliasCopyCache,
    const SharedAlias &original,
    const QPointF &dp = QPointF(0.0, 0.0),
    const bool generateNewAddresses = true
){
    if(aliasCopyCache.contains(original)) return aliasCopyCache.value(original);
    const auto &copied = original->clone(app->id());
    if(generateNewAddresses) copied->setAddress(app->address());
    copied->translate(dp);
    aliasCopyCache.insert(original, copied);
    copy.insert(copied);
    return copied;
}

LockedSelection copySelection(
    App *app,
    const LockedSelection &selection,
    const QPointF &dp,
    const bool generateNewAddresses
){
    LockedSelection copy;
    QHash<SharedAlias, SharedAlias> aliasCopyCache;
    QSet<SharedDipole> visitedDipoles;
    copy.reserve(selection.size());

    for(const auto &obj : selection){
        switch(obj->category()){
        case _NODE: {
            const auto &alias = static_pointer_cast<Alias>(obj);
            const auto &copied = getAliasCopy(app, copy, aliasCopyCache, alias, dp, generateNewAddresses);
            for(const auto &dipole : alias->connections()){
                if(!selection.contains(dipole) || visitedDipoles.contains(dipole)) continue;
                const auto &otherCopied = getAliasCopy(app, copy, aliasCopyCache, dipole->other(alias), dp, generateNewAddresses);
                if(alias == dipole->A()) copy.insert(dipole->clone(copied, otherCopied));
                else copy.insert(dipole->clone(otherCopied, copied));
                visitedDipoles.insert(dipole);
            }
            break;
        }
        case _DIPOLE: {
            const auto &dipole = static_pointer_cast<Dipole>(obj);
            if(visitedDipoles.contains(dipole)) continue;
            const auto &copiedA = getAliasCopy(app, copy, aliasCopyCache, dipole->A(), dp, generateNewAddresses);
            const auto &copiedB = getAliasCopy(app, copy, aliasCopyCache, dipole->B(), dp, generateNewAddresses);
            copy.insert(dipole->clone(copiedA, copiedB));
            visitedDipoles.insert(dipole);
            break;
        }
        default:
            break;
        }
    }

    return copy;
}
