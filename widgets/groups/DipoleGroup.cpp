#include "DipoleGroup.h"
#include "AliasGroup.h"

#include "widgets/HSeparator.h"
#include "widgets/inputs/IDLabel.h"
#include "widgets/inputs/DoubleParameterInput.h"

#include "objects/Dipole.h"

#include "widgets/MainPanel.h"
#include "App.h"
#include "widgets/tabs/VariablesTab.h"

DipoleGroup::DipoleGroup(MainPanel *mainPanel, ObjectType type, QWidget *parent):
    ObjectGroup(mainPanel, parent), _type(type),
    ID(new IDLabel(this))
{
    contentLayout->addWidget(new QLabel("ID"), 0, 0);
    contentLayout->addWidget(ID, 0, 1);

    switch(type){
    case RESISTOR:
        addParameter(Param::R, "Resistance");
        break;
    case CAPACITOR:
        addParameter(Param::C, "Capacitance");
        addParameter(Param::Q0, "Initial Quantity");
        break;
    case INDUCTOR:
        addParameter(Param::L, "Inductance");
        addParameter(Param::I0, "Initial Current");
        break;
    case BATTERY:
    case DC_VOLTAGE_GENERATOR:
        addParameter(Param::V, "Voltage");
        break;
    case DC_CURRENT_GENERATOR:
        addParameter(Param::I, "Current");
        break;
    default:
        break;
    }
}

void DipoleGroup::addParameter(const Param param, const QString &label){
    static int i = 1;
    const auto &DoubleInput = new DoubleParameterInput(this, param, label, this);
    DoubleInputs.append(DoubleInput);
    contentLayout->addWidget(DoubleInput, i, 0, 1, 2);
    i++;
}


void DipoleGroup::updateData(){
    ObjectGroup::updateData();
    ID->updateData();
    for(auto &DoubleInput : DoubleInputs)
        DoubleInput->updateData();
}


bool DipoleGroup::isMixedParam(const Param param) const {
    double firstValue;
    bool init = true;
    for(const auto &obj : selection){
        if(const auto &d = dynamic_pointer_cast<Dipole>(obj)){
            if(init) {
                firstValue = d->paramValue(param);
                init = false;
            }
            else if(d->paramValue(param) != firstValue) return true;
        }
    }
    return false;
}
double DipoleGroup::paramValue(const Param param) const {
    for(const auto &obj : selection){
        std::cout << obj;
        if(const auto &d = dynamic_pointer_cast<Dipole>(obj))
            return d->paramValue(param);
    }
    return 0.0;
}
QString DipoleGroup::paramVarname(const Param param) const {
    for(const auto &obj : selection){
        if(const auto &d = dynamic_pointer_cast<Dipole>(obj))
            return d->paramVarname(param);
    }
    return "";
}

void DipoleGroup::setParamValue(const Param param, const double value){
    for(const auto &obj : selection) {
        if(const auto &d = dynamic_pointer_cast<Dipole>(obj)){
            d->setParamVarname(param, "");
            d->setParamValue(param, value);
        }
    }
}
void DipoleGroup::confirmParamValue(const Param param, const double value){
    setParamValue(param, value);
}

void DipoleGroup::setParamVarname(const Param param, const QString &varname){
    const QVariant &v = mainPanel->variablesTab->value(varname);
    for(const auto &obj : selection) {
        if(const auto &d = dynamic_pointer_cast<Dipole>(obj)){
            d->setParamVarname(param, varname);
            d->setParamValue(param, v.toDouble());
        }
    }
}

void DipoleGroup::confirmParamVarname(const Param param, const QString &varname){
    setParamVarname(param, varname);
}


void DipoleGroup::onAddVariables(
    const QVector<QString> &names,
    const QVector<QVariant> &values,
    const QVector<VariableType> &types
){
    for(auto &DoubleInput : DoubleInputs)
        DoubleInput->onAddVariables(names, values, types);
}
void DipoleGroup::onChangeVariables(
    const QVector<QString> &names, const QVector<QVariant> &newValues
){
    for(auto &DoubleInput : DoubleInputs)
        DoubleInput->onChangeVariables(names, newValues);
}
void DipoleGroup::onRemoveVariables(const QVector<QString> &names){
    for(auto &DoubleInput : DoubleInputs)
        DoubleInput->onRemoveVariables(names);
}
