#include "DoubleParameterInput.h"
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QCompleter>
#include <QStyle>
#include <cmath>
#include "widgets/custom/CustomDoubleSpinBox.h"
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>

#include "widgets/inputs/BooleanInput.h"

inline QString textLabelOf(const VariableType type){
    switch(type){
    case VAR_RESISTANCE: return "Resistance";
    case VAR_CAPACITANCE: return "Capacitance";
    case VAR_INDUCTANCE: return "Inductance";
    case VAR_INTENSITY: return "Intensity";
    case VAR_QUANTITY: return "Quantity";
    case VAR_VOLTAGE: return "Voltage";
    default: return "";
    }
}

inline int indexToMagnitude(const QPair<int,int> &range, int index){
    return std::max(range.second - index*3, range.first);
}
inline int magnitudeToIndex(const QPair<int,int> &range, int order){
    return std::max((range.second - order)/3, 0);
}

DoubleParameterInput::DoubleParameterInput(InputGroup* group, VariableType type, QWidget* parent):
    QWidget(parent),
    group(group),
    vartype(type),
    unit(getUnitOfVariableType(type)),
    magnitudeRange(unitMagnitudeRange(unit)),
    maxValue(pow(10.0, magnitudeRange.second + 3)),
    minValue(pow(10.0, magnitudeRange.first)),
    unitSymbol(getUnitSymbol(unit)),
    mainLayout(new QVBoxLayout(this)),
    headerWidget(new QWidget(this)),
    constantButton(new QPushButton("Constant", headerWidget)),
    variableButton(new QPushButton("Variable", headerWidget)),
    ShowLabel(
        new BooleanInput(
            group, "show-label",
            QIcon(":/assets/icons/eye_opened.png"),
            QIcon(":/assets/icons/eye_shut.png"),
            QSize(16,16),
            parent
        )
    ),
    constantWidget(new QWidget(this)),
    valueSpin(new CustomDoubleSpinBox(constantWidget)),
    magnitudeCombo(new QComboBox(constantWidget)),
    toggleRawButton(new QPushButton(unitSymbol, constantWidget)),
    variableWidget(new QWidget(this)),
    varnameLineEdit(new QLineEdit(variableWidget)),
    variableCompleter(new QCompleter(this))
{
    setupHeaderUi();
    setupConstantUi();
    setupVariableUi();
    setupMainLayout();
}

// Setup
void DoubleParameterInput::setupHeaderUi(){
    QHBoxLayout *layout = new QHBoxLayout(headerWidget);
    QButtonGroup *modeGroup = new QButtonGroup(headerWidget);
    QLabel *label = new QLabel(textLabelOf(vartype));
    QHBoxLayout *modeLayout = new QHBoxLayout();

    layout->addWidget(ShowLabel);
    layout->addWidget(label);

    layout->addLayout(modeLayout);
    layout->setSpacing(12);
    layout->setContentsMargins(0,0,0,0);

    // Add buttons to group to make them exclusive
    modeGroup->addButton(constantButton, 0);
    modeGroup->addButton(variableButton, 1);
    constantButton->setCheckable(true);
    variableButton->setCheckable(true);

    // Layout to arrange buttons side by side
    modeLayout->setSpacing(0);
    modeLayout->setContentsMargins(0,0,0,0);
    modeLayout->addWidget(constantButton);
    modeLayout->addWidget(variableButton);

    // Set styles
    QString buttonStyle = R"(
        QPushButton {
            border: 1px solid #666;
            background-color: transparent;
            color: #888;
            padding: 3px 6px;
            font-weight: bold;
            border-radius: 0px;
        }
        QPushButton:checked {
            background-color: #444;
            color: white;
        }
    )";

    constantButton->setStyleSheet(buttonStyle);
    variableButton->setStyleSheet(buttonStyle);

    constantButton->setCursor(Qt::PointingHandCursor);
    variableButton->setCursor(Qt::PointingHandCursor);

    // Handle button clicks
    connect(constantButton, &QPushButton::clicked, this, &DoubleParameterInput::handleConstantSwitching);
    connect(variableButton, &QPushButton::clicked, this, &DoubleParameterInput::handleVariableSwitching);
}


void DoubleParameterInput::setupConstantUi(){
    QHBoxLayout* layout = new QHBoxLayout(constantWidget);
    layout->addWidget(valueSpin);
    layout->addWidget(magnitudeCombo);
    layout->addWidget(toggleRawButton);
    layout->setSpacing(6);
    layout->setContentsMargins(0, 0, 0, 0);

    int order = magnitudeRange.second;
    while(order >= magnitudeRange.first){
        magnitudeCombo->addItem(getOrderSymbol(order) + unitSymbol);
        order -= 3;
    }

    // Configure spin box
    valueSpin->setAlignment(Qt::AlignCenter);
    valueSpin->setMinimum(0);
    valueSpin->setDecimals(3);
    valueSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Configure raw mode toggle
    toggleRawButton->setText(unitSymbol);
    toggleRawButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    toggleRawButton->setCursor(Qt::PointingHandCursor);
    toggleRawButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: white;
            border-radius: 2px;
            border: 1px solid #444;
            padding: 3px 6px;
        }

        QPushButton:hover {
            background-color: rgba(255,255,255,40);
        }

        QPushButton:pressed {
            background-color: rgba(255,255,255,60);
        }

        QPushButton:checked {
            background-color: rgba(255,255,255,20);
            color: white;
            border: 1px solid #666;
        }
        QPushButton:unchecked {
            background-color: transparent;
            color: #AAA;
            border: 1px solid #444;
        }
    )");


    toggleRawButton->setCheckable(true);

    connect(valueSpin, &QDoubleSpinBox::valueChanged,
            this, &DoubleParameterInput::handleValueSpinValueChanged
    );
    connect(valueSpin, &QDoubleSpinBox::editingFinished,
            this, &DoubleParameterInput::handleValueSpinEditingFinished
    );
    connect(magnitudeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DoubleParameterInput::handleMagnitudeChange
    );
    connect(toggleRawButton, &QPushButton::toggled,
            this, &DoubleParameterInput::toggleRaw
    );

    toggleRawButton->blockSignals(true);
    toggleRawButton->setChecked(true);
    toggleRawButton->blockSignals(false);
    toggleRawButton->toggle();
}

void DoubleParameterInput::setupVariableUi(){
    QHBoxLayout* layout = new QHBoxLayout(variableWidget);
    layout->addWidget(new QLabel("Pick Variable"));
    layout->addWidget(varnameLineEdit);
    layout->setSpacing(12);
    layout->setContentsMargins(0, 0, 0, 0);

    variableCompleter->setCaseSensitivity(Qt::CaseSensitive);
    variableCompleter->setFilterMode(Qt::MatchStartsWith);
    varnameLineEdit->setCompleter(variableCompleter);

    connect(varnameLineEdit, &QLineEdit::textChanged,
            this, &DoubleParameterInput::handleVariableEdit);
    connect(varnameLineEdit, &QLineEdit::editingFinished,
            this, &DoubleParameterInput::finalizeVariableEdit);
}

void DoubleParameterInput::setupMainLayout(){
    setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(8);
    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(constantWidget);
    mainLayout->addWidget(variableWidget);
}



// State Management
bool DoubleParameterInput::isVariable(){
    return variableButton->isChecked();
}
bool DoubleParameterInput::isConstant(){
    return constantButton->isChecked();
}
bool DoubleParameterInput::isRaw(){
    return toggleRawButton->isChecked();
}

void DoubleParameterInput::handleConstantSwitching(){
    constantButton->setChecked(true);
    constantWidget->setVisible(true);
    variableButton->setChecked(false);
    variableWidget->setVisible(false);
    variableButton->setDisabled(variables.isEmpty());

    valueSpin->blockSignals(true);
    magnitudeCombo->blockSignals(true);
    bool ok;
    const QString &strvalue = group->dataString("value");
    rawValue = strvalue.toDouble(&ok);
    currentOrder = isRaw() ? 0 : magnitude(rawValue);
    updateSpinAndMagnitude();
    if(ok && !strvalue.isEmpty())
        valueSpin->setValue(rawValue / multiplier);
    else {
        valueSpin->lineEdit()->setPlaceholderText(strvalue);
        valueSpin->lineEdit()->setText("");
    }
    magnitudeCombo->blockSignals(false);
    valueSpin->blockSignals(false);
}

void DoubleParameterInput::handleVariableSwitching(){
    constantButton->setChecked(false);
    constantWidget->setVisible(false);
    variableButton->setChecked(true);
    variableWidget->setVisible(true);
    variableButton->setEnabled(true);
}

// Helper function to update spin widget and magnitudeCombo based on rawValue.
void DoubleParameterInput::updateSpinAndMagnitude() {
    valueSpin->blockSignals(true);
    magnitudeCombo->blockSignals(true);
    multiplier = pow(10.0, std::min(currentOrder, magnitudeRange.second));
    valueSpin->setMaximum(maxValue / multiplier);
    valueSpin->setDecimals(qBound(0, currentOrder - magnitudeRange.first, 3));
    valueSpin->setValue(rawValue / multiplier);
    magnitudeCombo->setCurrentIndex(magnitudeToIndex(magnitudeRange, currentOrder));
    magnitudeCombo->blockSignals(false);
    valueSpin->blockSignals(false);
}

void DoubleParameterInput::updateCompleterModel(){
    QStringListModel* completerModel = qobject_cast<QStringListModel*>(variableCompleter->model());
    if (!completerModel) {
        completerModel = new QStringListModel(variableCompleter);
        variableCompleter->setModel(completerModel);
    }
    completerModel->setStringList(variables);
}

void DoubleParameterInput::updateData(){
    ShowLabel->updateData();

    variables = group->app->varManager.varnames(vartype);
    updateCompleterModel();

    if(validateVarname(group->dataString("varname")))
        handleVariableSwitching();
    else
        handleConstantSwitching();
}


bool DoubleParameterInput::validateVarname(const QString &name){
    bool valid = !name.isEmpty() && variables.contains(name);
    varnameLineEdit->blockSignals(true);
    varnameLineEdit->setPlaceholderText("");
    varnameLineEdit->setText("");
    if(valid){
        varname = name;
        varnameLineEdit->setText(name);
    }
    else if (variables.contains(varname)) {
        varnameLineEdit->setText(varname);
    }
    else if(varname == "Mixed") {
        varnameLineEdit->setPlaceholderText(varname);
    }
    else if(!varname.isEmpty()) {
        varname = "";
    }
    varnameLineEdit->blockSignals(false);
    return valid;
}

void DoubleParameterInput::handleVariableEdit(const QString &text) {
    variableCompleter->complete();
    varnameLineEdit->setStyleSheet(
        variables.contains(text.trimmed(), Qt::CaseSensitive)
            ? "color: white;"
            : "color: #FF5555;"
        );
}

void DoubleParameterInput::finalizeVariableEdit() {
    const QString newVarname = varnameLineEdit->text().trimmed();
    if(validateVarname(newVarname)) {
        group->onEditingFinishedApply("varname", newVarname);
        varnameLineEdit->setStyleSheet("color: white;");
        group->app->update();
    }
}

void DoubleParameterInput::handleValueSpinValueChanged(const double value){
    rawValue = value*multiplier;
    group->apply("value", QString::number(rawValue));
    group->app->update();
}

void DoubleParameterInput::handleMagnitudeChange(int index){
    currentOrder = indexToMagnitude(magnitudeRange, index);
    updateSpinAndMagnitude();
    group->apply("value", QString::number(rawValue));
    group->app->update();
}

void DoubleParameterInput::toggleRaw() {
    currentOrder = isRaw() ? 0 : magnitude(rawValue);
    magnitudeCombo->setVisible(!isRaw());
    rawValue = valueSpin->value()*multiplier;
    updateSpinAndMagnitude();
}

void DoubleParameterInput::handleValueSpinEditingFinished(){
    if(valueSpin->lineEdit()->placeholderText() == "Mixed"){
        valueSpin->blockSignals(true);
        valueSpin->lineEdit()->setText("");
        valueSpin->blockSignals(false);
        return;
    }
    rawValue = valueSpin->value()*multiplier;
    currentOrder = isRaw() ? 0 : magnitude(rawValue);
    updateSpinAndMagnitude();
    group->onEditingFinishedApply("value", QString::number(rawValue));
}


void DoubleParameterInput::onAddVariables(
    const QVector<QString> &names,
    [[maybe_unused]] const QVector<QVariant> &values,
    const QVector<VariableType> &types
){
    bool needUpdate = false;
    for(int i = 0; i < names.size(); ++i){
        if(types[i] != vartype) continue;
        needUpdate = true;
        variables.append(names[i]);
    }
    if(!needUpdate) return;
    updateCompleterModel();
    variableButton->setEnabled(true);
    if(isConstant() && validateVarname(varname))
        handleVariableSwitching();
}

void DoubleParameterInput::onChangeVariables(
    [[maybe_unused]] const QVector<QString> &names,
    [[maybe_unused]] const QVector<QVariant> &newValues,
    [[maybe_unused]] const QVector<VariableType> &types
){}

void DoubleParameterInput::onRemoveVariables(
    const QVector<QString> &names,
    [[maybe_unused]] const QVector<VariableType> &types
){
    bool needUpdate = false;
    for(int i = 0; i < names.size(); ++i){
        if(types[i] != vartype) continue;
        needUpdate = true;
        variables.removeOne(names[i]);
    }
    if(!needUpdate) return;
    updateCompleterModel();
    if(isVariable() && !validateVarname(varname))
        handleConstantSwitching();
}
