#include "UnitDipoleGroup.h"

#include "widgets/inputs/BooleanInput.h"
#include "widgets/inputs/DoubleParameterInput.h"
#include "App.h"
#include <QLabel>

#include "objects/UnitDipole.h"
#include "VariablesManager.h"

#include "commands/EntangleValueToObjectsCommand.h"


UnitDipoleGroup::UnitDipoleGroup(App *app, const ObjectType type, QWidget *parent):
    DipoleGroup(app, parent),
    _type(type),
    _vartype(varTypeOf(type)),
    Value(new DoubleParameterInput(this, _vartype))
{
    contentLayout->addWidget(Value, 1, 0, 1, 2);
}


void UnitDipoleGroup::updateData(){
    DipoleGroup::updateData();
    Value->updateData();
}


QStringList UnitDipoleGroup::varnames() const {
    return app->varManager.varnames(_vartype);
}

void UnitDipoleGroup::apply(const QString &key, const QString &value){
    if(key != "varname") return DipoleGroup::apply(key, value);
}

void UnitDipoleGroup::onEditingFinishedApply(const QString &key, const QString &value){
    if(key != "varname") return DipoleGroup::onEditingFinishedApply(key, value);
    if (isEmpty()) return;
    app->execute(std::make_unique<EntangleValueToObjectsCommand>(app, selection, value, _vartype));
}

bool UnitDipoleGroup::isMixed(const QString &key) const {
    if (key != "varname") return DipoleGroup::isMixed(key);
    if (isEmpty()) return false;

    auto firstShare = std::static_pointer_cast<UnitDipole>(*selection.constBegin())->share();
    return std::any_of(selection.begin(), selection.end(), [&](const auto &obj) {
        return std::static_pointer_cast<UnitDipole>(obj)->share() != firstShare;
    });
}

QString UnitDipoleGroup::dataString(const QString &key) const {
    if (key != "varname") return DipoleGroup::dataString(key);
    if(isEmpty()) return "";
    if (isMixed(key)) return "Mixed";
    auto firstShare = std::static_pointer_cast<UnitDipole>(*selection.constBegin())->share();
    return app->varManager.varname(firstShare);
}

void UnitDipoleGroup::onAddVariables(
    const QVector<QString> &names,
    const QVector<QVariant> &values,
    const QVector<VariableType> &types
){
    Value->onAddVariables(names, values, types);
}

void UnitDipoleGroup::onChangeVariables(
    const QVector<QString> &names,
    const QVector<QVariant> &newValues,
    const QVector<VariableType> &types
){
    Value->onChangeVariables(names, newValues, types);
}
void UnitDipoleGroup::onRemoveVariables(
    const QVector<QString> &names,
    const QVector<VariableType> &types
){
    Value->onRemoveVariables(names, types);
}

