#include "VariablesManager.h"
#include "App.h"

#include "objects/Alias.h"
#include "objects/Dipole.h"
#include "objects/UnitDipole.h"


// Needs to be updated according to the existing objects' dependencies.
void VariablesManager::detachDoubleFromObjects(const QString &name){
    QSet<SharedDipole> visitedDipoles;
    for(const auto &alias : app->aliases){
        for(const auto &dipole : alias->connections()){
            if(!isUnitDipole(dipole->type())) continue;
            if(visitedDipoles.contains(dipole)) continue;
            auto unitDipole = std::static_pointer_cast<UnitDipole>(dipole);
            if(varname(unitDipole->share()) == name) unitDipole->unshare();
            visitedDipoles.insert(dipole);
        }
    }
    app->update();
}
void VariablesManager::detachIntFromObjects([[maybe_unused]] const QString &name){

}
void VariablesManager::detachStringFromObjects([[maybe_unused]] const QString &name){

}
void VariablesManager::attachDoubleToObjects(
    const QString &name, const QSet<SharedObject> &selection
){
    const auto &v = doubleKey(name);
    if(!v) return;
    QSet<SharedObject> visitedDipoles;
    for(const auto &obj : selection){
        if(!isUnitDipole(obj->type())) continue;
        if(visitedDipoles.contains(obj)) continue;
        std::dynamic_pointer_cast<UnitDipole>(obj)->setValue(v);
        visitedDipoles.insert(obj);
    }
    app->update();
}
void VariablesManager::attachIntToObjects(
    [[maybe_unused]] const QString &name, [[maybe_unused]] const QSet<SharedObject> &selection){

}
void VariablesManager::attachStringToObjects(
    [[maybe_unused]] const QString &name, [[maybe_unused]] const QSet<SharedObject> &selection){

}


SharedDouble VariablesManager::doubleKey(const QString &name) const {
    for(const auto &v : doubleTable.keys()){
        if(doubleTable[v]->name == name) return v;
    }
    return nullptr;
}
SharedInt VariablesManager::intKey(const QString &name) const {
    for(const auto &v : intTable.keys()){
        if(intTable[v]->name == name) return v;
    }
    return nullptr;
}
SharedString VariablesManager::stringKey(const QString &name) const {
    for(const auto &v : stringTable.keys()){
        if(stringTable[v]->name == name) return v;
    }
    return nullptr;
}


void VariablesManager::changeVariables(
    const QVector<QString> &names, const QVector<QVariant> newValues, const QVector<VariableType> types
){
    for(int i = 0; i < names.size(); ++i){
        const QString &name = names[i];
        const QVariant &newValue = newValues[i];
        const VariableType type = types[i];
        if (varIsDouble(type)) {
            SharedDouble v = doubleKey(name);
            if (v) *v = newValue.toDouble();
        }
        else if (type == VAR_STRING) {
            SharedString v = stringKey(name);
            if (v) *v = newValue.toString();
        }
        else if (type == VAR_INT) {
            SharedInt v = intKey(name);
            if (v) *v = newValue.toInt();
        }
    }
    emit variablesChanged(names, newValues, types);
}

void VariablesManager::removeVariables(const QVector<QString> &names, const QVector<VariableType> types) {
    for(int i = 0; i < names.size(); ++i){
        const QString &name = names[i];
        const VariableType type = types[i];
        if (varIsDouble(type)) {
            detachDoubleFromObjects(name);
            doubleTable.remove(doubleKey(name));
        }
        else if (type == VAR_STRING) {
            detachStringFromObjects(name);
            stringTable.remove(stringKey(name));
        }
        else if (type == VAR_INT) {
            detachIntFromObjects(name);
            intTable.remove(intKey(name));
        }
    }
    emit variablesRemoved(names, types);
}

void VariablesManager::addVariables(
    const QVector<QString> &names,
    const QVector<QVariant> values,
    const QVector<VariableType> types
){
    for(int i = 0; i < names.size(); ++i){
        const QString &name = names[i];
        const QVariant &value = values[i];
        const VariableType type = types[i];

        if (varIsDouble(type)) {
            doubleTable.insert(
                std::make_shared<double>(value.toDouble()),
                std::make_shared<VarMetadata>(VarMetadata{name, type})
            );
        }
        else if (type == VAR_INT) {
            intTable.insert(
                std::make_shared<int>(value.toInt()),
                std::make_shared<VarMetadata>(VarMetadata{name, type})
            );
        }
        else if (type == VAR_STRING) {
            stringTable.insert(
                std::make_shared<QString>(value.toString()),
                std::make_shared<VarMetadata>(VarMetadata{name, type})
            );
        }
    }
    emit variablesAdded(names, values, types);
}
void VariablesManager::addVariables(
    const QVector<QString> &names,
    const QVector<QVariant> values,
    const QVector<VariableType> types,
    const QHash<QString, QVector<SharedUnitDipole>> &cache
){
    addVariables(names, values, types);
    for(const QString &name : cache.keys()){
        for(const auto &ud : cache[name]){
            if(const auto sharedDouble = doubleKey(name)) ud->setValue(sharedDouble);
        }
    }
}




QVariant VariablesManager::varvalue(const QString &name, const VariableType type) const {
    if (varIsDouble(type)) {
        for(const auto &shared : doubleTable.keys()){
            if(doubleTable[shared]->name == name) return *shared;
        }
    }
    else if (type == VAR_INT) {
        for(const auto &shared : intTable.keys()){
            if(intTable[shared]->name == name) return *shared;
        }
    }
    else if (type == VAR_STRING) {
        for(const auto &shared : stringTable.keys()){
            if(stringTable[shared]->name == name) return *shared;
        }
    }
    return 0.0;
}
QString VariablesManager::varname(const SharedDouble &var) const {
    return doubleTable.contains(var)? doubleTable[var]->name : "";
}
QString VariablesManager::varname(const SharedInt &var) const {
    return intTable.contains(var)? intTable[var]->name : "";
}
QString VariablesManager::varname(const SharedString &var) const {
    return stringTable.contains(var)? stringTable[var]->name : "";
}
VariableType VariablesManager::vartype(const SharedDouble &var) const {
    return doubleTable.contains(var)? doubleTable[var]->type : VAR_NULL;
}
VariableType VariablesManager::vartype(const SharedInt &var) const {
    return intTable.contains(var)? intTable[var]->type : VAR_NULL;
}
VariableType VariablesManager::vartype(const SharedString &var) const {
    return stringTable.contains(var)? stringTable[var]->type : VAR_NULL;
}
VarMetadata VariablesManager::varmetadata(const SharedDouble &var) const {
    const SharedVarMetadata &metadata = doubleTable[var];
    return doubleTable.contains(var)? VarMetadata{metadata->name, metadata->type} : VarMetadata{"",VAR_NULL};
}
VarMetadata VariablesManager::varmetadata(const SharedInt &var) const {
    const SharedVarMetadata &metadata = intTable[var];
    return intTable.contains(var)? VarMetadata{metadata->name, metadata->type} : VarMetadata{"",VAR_NULL};
}
VarMetadata VariablesManager::varmetadata(const SharedString &var) const {
    const SharedVarMetadata &metadata = stringTable[var];
    return stringTable.contains(var)? VarMetadata{metadata->name, metadata->type} : VarMetadata{"",VAR_NULL};
}

QVector<SharedUnitDipole> VariablesManager::unitDipoles(const QString &name) const {
    QSet<SharedDipole> visitedDipoles;
    QVector<SharedUnitDipole> result;
    for(const auto &alias : app->aliases){
        for(const auto &dipole : alias->connections()){
            if(!isUnitDipole(dipole->type())) continue;
            if(visitedDipoles.contains(dipole)) continue;
            auto unitDipole = std::static_pointer_cast<UnitDipole>(dipole);
            if(varname(unitDipole->share()) == name) result.append(unitDipole);
            visitedDipoles.insert(dipole);
        }
    }
    return result;
}






QStringList VariablesManager::varnames() const {
    QStringList result;
    for(const auto &var : doubleTable) result.append(var->name);
    for(const auto &var : intTable) result.append(var->name);
    for(const auto &var : stringTable) result.append(var->name);
    return result;
}
QStringList VariablesManager::varnames(const VariableType type) const {
    QStringList result;
    if(varIsDouble(type)){
        for(const auto &var : doubleTable){
            if(var->type != type) continue;
            result.append(var->name);
        }
    }
    else if(type == VAR_INT){
        for(const auto &var : intTable)
            result.append(var->name);
    }
    else if(type == VAR_STRING){
        for(const auto &var : stringTable)
            result.append(var->name);
    }
    return result;
}
