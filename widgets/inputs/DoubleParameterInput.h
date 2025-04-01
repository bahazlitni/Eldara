#pragma once
#include "widgets/groups/InputGroup.h"
#include "App.h"
#include <QWidget>
#include <QStringListModel>


class CustomDoubleSpinBox;
class QComboBox;
class QLineEdit;
class QCheckBox;
class QCompleter;
class QPushButton;
class QVBoxLayout;
class BooleanInput;

class DoubleParameterInput : public QWidget {
    Q_OBJECT

public:
    DoubleParameterInput(InputGroup* group, VariableType type, QWidget* parent = nullptr);
    void updateData();

    // State
    bool isVariable();
    bool isConstant();
    bool isRaw();

    void onAddVariables(
        const QVector<QString> &names,
        [[maybe_unused]]  const QVector<QVariant> &values,
        [[maybe_unused]]  const QVector<VariableType> &types
    );
    void onChangeVariables(
        [[maybe_unused]] const QVector<QString> &names,
        [[maybe_unused]] const QVector<QVariant> &newValues,
        [[maybe_unused]] const QVector<VariableType> &types
    );
    void onRemoveVariables(
        const QVector<QString> &names,
        [[maybe_unused]] const QVector<VariableType> &types
    );

private:
    InputGroup* group;
    const VariableType vartype;
    const SIUnit unit;
    const QPair<int, int> magnitudeRange;
    const double maxValue;
    const double minValue;
    const QString unitSymbol;
    int currentOrder;
    double multiplier;
    double rawValue;


    QStringList variables;
    QString varname;

    QVBoxLayout *mainLayout;

    // Header Widget
    QWidget *headerWidget;
    QPushButton *constantButton;
    QPushButton *variableButton;
    BooleanInput *ShowLabel;

    // Constant Widget
    QWidget *constantWidget;
    CustomDoubleSpinBox *valueSpin;
    QComboBox *magnitudeCombo;
    QPushButton *toggleRawButton;

    // Variable Widget
    QWidget *variableWidget;
    QLineEdit *varnameLineEdit;
    QCompleter *variableCompleter;


    // Helpers
    void setupHeaderUi();
    void setupConstantUi();
    void setupVariableUi();
    void setupMainLayout();
    void setupConnections();

    void updateSpinAndMagnitude();
    void updateCompleterModel();

    void validateVariableName(const QString& name);
    bool validateVarname(const QString &name);

private slots:
    void handleConstantSwitching();
    void handleValueSpinValueChanged(const double value);
    void handleMagnitudeChange(int index);
    void handleValueSpinEditingFinished();
    void toggleRaw();

    void handleVariableSwitching();
    void handleVariableEdit(const QString &text);
    void finalizeVariableEdit();
};
