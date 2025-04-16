#include "objects/Alias.h"
#include "objects/Dipole.h"

#include "objects/BCPoint.h"
#include "objects/BCControlPoint.h"
#include "objects/BCPath.h"

#include "Scene.h"
#include "Grid.h"

inline void ProcessAlias(
    QSet<SharedPoint> *set,
    QSet<SharedAlias> *processedAliases,
    const SharedAlias &alias
    ){
    processedAliases->insert(alias);
    set->insert(alias->share());
}

QSet<SharedPoint> GetWorldPointSelection(const Selection &selection){
    QSet<SharedPoint> set;
    QSet<SharedAlias> processedAliases;
    for (const auto &obj : selection){
        if (obj->category() == ObjectCategory::Node){
            SharedAlias alias = std::static_pointer_cast<Alias>(obj);
            if (alias && !processedAliases.contains(alias))
                ProcessAlias(&set, &processedAliases, alias);
        }
        else if (obj->category() == ObjectCategory::Dipole) {
            auto dipole = std::static_pointer_cast<Dipole>(obj);
            if (dipole) {
                auto a = dipole->A();
                auto b = dipole->B();
                if (a && !processedAliases.contains(a))
                    ProcessAlias(&set, &processedAliases, a);
                if (b && !processedAliases.contains(b))
                    ProcessAlias(&set, &processedAliases, b);
            }
        } else if(obj->type() == ObjectType::BCP){
            auto bcpoint = std::static_pointer_cast<BCPoint>(obj);
            set.insert(bcpoint->share());
            if(const auto &ctrlBefore = bcpoint->before())
                set.insert(ctrlBefore->share());
            if(const auto &ctrlAfter = bcpoint->after())
                set.insert(ctrlAfter->share());
        } else if(obj->type() == ObjectType::BCCP){
            auto ctrl = std::static_pointer_cast<BCControlPoint>(obj);
            set.insert(ctrl->share());
        }
    }
    return set;
}

Selection GetDeepSelection(const Selection &selection){
    Selection slc;
    slc.reserve(selection.size());
    for(const auto &obj : selection){
        switch(obj->category()){
        case ObjectCategory::Node: {
            const auto &alias = static_pointer_cast<Alias>(obj);
            for(const auto &dipole : alias->connections()) {
                if(slc.contains(dipole)) continue;
                slc.insert(dipole);
                if(const auto &A = dipole->A()) slc.insert(A);
                if(const auto &B = dipole->B()) slc.insert(B);
            }
            break;
        }
        case ObjectCategory::Dipole: {
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
    Scene *scene,
    Selection &copy,
    QHash<SharedAlias, SharedAlias> &aliasCopyCache,
    const SharedAlias &original,
    const QPointF &dp = QPointF(0.0, 0.0),
    const bool generateNewAddresses = true
){
    if(aliasCopyCache.contains(original)) return aliasCopyCache.value(original);
    const auto &copied = original->clone(scene->id());
    if(generateNewAddresses) copied->setAddress(scene->address());
    copied->translate(dp);
    aliasCopyCache.insert(original, copied);
    copy.insert(copied);
    return copied;
}

Selection copySelection(
    Scene *scene,
    const Selection &selection,
    const QPointF &dp,
    const bool generateNewAddresses
){
    Selection copy;
    QHash<SharedAlias, SharedAlias> aliasCopyCache;
    QSet<SharedDipole> visitedDipoles;
    copy.reserve(selection.size());

    for(const auto &obj : selection){
        switch(obj->category()){
        case ObjectCategory::Node: {
            const auto &alias = static_pointer_cast<Alias>(obj);
            const auto &copied = getAliasCopy(scene, copy, aliasCopyCache, alias, dp, generateNewAddresses);
            for(const auto &dipole : alias->connections()){
                if(!selection.contains(dipole) || visitedDipoles.contains(dipole)) continue;
                const auto &otherCopied = getAliasCopy(scene, copy, aliasCopyCache, dipole->other(alias), dp, generateNewAddresses);
                if(alias == dipole->A()) copy.insert(dipole->clone(copied, otherCopied));
                else copy.insert(dipole->clone(otherCopied, copied));
                visitedDipoles.insert(dipole);
            }
            break;
        }
        case ObjectCategory::Dipole: {
            const auto &dipole = static_pointer_cast<Dipole>(obj);
            if(visitedDipoles.contains(dipole)) continue;
            const auto &copiedA = getAliasCopy(scene, copy, aliasCopyCache, dipole->A(), dp, generateNewAddresses);
            const auto &copiedB = getAliasCopy(scene, copy, aliasCopyCache, dipole->B(), dp, generateNewAddresses);
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
