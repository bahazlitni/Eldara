#include "VariablesTab.h"
#include "commands/ChangeVariablesCommand.h"
#include "commands/RemoveVariablesCommand.h"
#include "commands/AddVariablesCommand.h"
#include "widgets/MainPanel.h"
#include "Scene.h"

#include "SelectionTab.h"

inline void VariablesTab::exe_changeVariables(
    const QVector<QString> &names, const QVector<QVariant> &newValues
){
    mainPanel->scene->execute(new ChangeVariablesCommand(this, names, newValues));
}
inline void VariablesTab::exe_removeVariables(const QVector<QString> &names){
    mainPanel->scene->execute(new RemoveVariablesCommand(this, names));
}
inline void VariablesTab::exe_addVariables(
    const QVector<QString> &names, const QVector<QVariant> &values, const QVector<VariableType> &types
){
    mainPanel->scene->execute(new AddVariablesCommand(this, names, values, types));
}
inline void VariablesTab::exe_replaceVariables(
    const QVector<QString> &oldNames,
    const QVector<QString> &newNames,
    const QVector<QVariant> &newValues,
    const QVector<VariableType> &newTypes
){
    auto macro = new QUndoCommand(tr("Replace Variables"));
    new RemoveVariablesCommand(this, oldNames, macro);
    new AddVariablesCommand(this, newNames, newValues, newTypes, macro);
    mainPanel->scene->execute(macro);
}

// Constructor
VariablesTab::VariablesTab(MainPanel *mainPanel)
    : QWidget(mainPanel), mainPanel(mainPanel), table(new QTableWidget(this)),
    addButton(new QPushButton("Add")), removeButton(new QPushButton("Remove")),
    noDataLabel(new QLabel("No variables are found.", this)),
    lastEnteredType(VAR_RESISTANCE) // default type
{
    // Main layout with scroll area.
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QScrollArea *scrollArea = new QScrollArea(this);
    QWidget *contentWidget = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);

    // Controls layout.
    QHBoxLayout *controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget(addButton);
    controlsLayout->addWidget(removeButton);
    contentLayout->addLayout(controlsLayout);

    // Table and no-data label.
    contentLayout->addWidget(table);
    contentLayout->addWidget(noDataLabel);
    noDataLabel->setAlignment(Qt::AlignCenter);

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // Setup table.
    table->setRowCount(0);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Name", "Value", "Type"});
    table->setEditTriggers(QTableWidget::DoubleClicked);
    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectItems);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    table->setColumnWidth(0, 50);
    table->setColumnWidth(1, 90);
    table->setColumnWidth(2, 90);

    // Connect signals.
    connect(addButton, &QPushButton::clicked, this, &VariablesTab::onAddVariable);
    connect(removeButton, &QPushButton::clicked, this, &VariablesTab::onRemoveVariable);
    connect(table, &QTableWidget::cellChanged, this, &VariablesTab::onCellChanged);
    connect(table, &QTableWidget::cellDoubleClicked, this, &VariablesTab::onCellDoubleClicked);
}

// Insert a new row into the table.
// Also, store the original name in UserRole for later validation.
void VariablesTab::buildRow(const int row, const QString &name, const QVariant &value, const VariableType type){
    if (isEmpty()) {
        table->setVisible(true);
        noDataLabel->setVisible(false);
        removeButton->setEnabled(true);
    }

    QTableWidgetItem *nameItem = new QTableWidgetItem(name);
    // Save the original name
    nameItem->setData(Qt::UserRole, name);
    QTableWidgetItem *valueItem = new QTableWidgetItem(value.toString());
    nameItem->setTextAlignment(Qt::AlignCenter);
    valueItem->setTextAlignment(Qt::AlignCenter);

    QComboBox *typeCombo = new QComboBox(this);
    typeCombo->addItem("Text", VAR_STRING);
    typeCombo->addItem("Number", VAR_DOUBLE);
    typeCombo->addItem("Resistance", VAR_RESISTANCE);
    typeCombo->addItem("Capacitance", VAR_CAPACITANCE);
    typeCombo->addItem("Voltage", VAR_VOLTAGE);
    typeCombo->addItem("Inductance", VAR_INDUCTANCE);
    typeCombo->addItem("Quantity", VAR_QUANTITY);
    typeCombo->addItem("Intensity", VAR_INTENSITY);
    typeCombo->setCurrentIndex(typeCombo->findData(type));
    typeCombo->setFocusPolicy(Qt::NoFocus);

    table->insertRow(row);
    table->setItem(row, 0, nameItem);
    table->setItem(row, 1, valueItem);
    table->setCellWidget(row, 2, typeCombo);
    connect(typeCombo, &QComboBox::currentIndexChanged, this, [this, typeCombo](int index) {
        const int row = rowOfTypeComboBox(typeCombo);
        lastEnteredType = static_cast<VariableType>(typeCombo->itemData(index).toInt());
        QTableWidgetItem *nameItem = table->item(row, 0);
        QTableWidgetItem *valueItem = table->item(row, 1);
        QComboBox *typeCombo = qobject_cast<QComboBox*>(table->cellWidget(row, 2));
        if(!nameItem || !valueItem || !typeCombo) return;
        exe_replaceVariables(
            QVector{nameItem->data(Qt::UserRole).toString()},
            QVector{QString("")},
            QVector{valueItem->data(Qt::UserRole)},
            QVector{static_cast<VariableType>(index)}
        );
    });
}


void VariablesTab::validateNewData(
    const QString &originalName,
    const QVariant &originalValue,
    const VariableType type,
    QString *newName,
    QVariant *newValue
){
    if (newName->isEmpty() || (*newName != originalName && containsName(*newName))){
        if (originalName.isEmpty() || containsName(originalName)){
            const QString key = getVariableKey(lastEnteredType).toUpper();
            *newName = (key + QString::number(suffix(key))).left(VARNAME_MAX_LENGTH);
        }
        else
            *newName = originalName;
    }

    const QPair<double, double> range = rangeOfVariable(type);
    if (varIsDouble(type)) {
        bool ok;
        double parsedValue = newValue->toDouble(&ok);
        if (ok)
            *newValue = QVariant(parsedValue == 0? 0 : qBound(range.first, parsedValue, range.second));
        else {
            parsedValue = originalValue.toDouble(&ok);
            if (ok)
                *newValue = QVariant(parsedValue == 0? 0 : qBound(range.first, parsedValue, range.second));
            else
                *newValue = 0.0;
        }
    }
    else if(type == VAR_INT){
        bool ok;
        int parsedValue = newValue->toInt(&ok);
        if (ok)
            *newValue = QVariant(parsedValue == 0? 0 : qBound((int) range.first, parsedValue, (int) range.second));
        else {
            parsedValue = originalValue.toInt(&ok);
            if (ok)
                *newValue = QVariant(parsedValue == 0? 0 : qBound((int) range.first, parsedValue, (int) range.second));
            else
                *newValue = 0.0;
        }
    }
    else if(type == VAR_STRING)
        *newValue = newValue->toString().trimmed().left(VARVALUE_STRING_MAX_LENGTH);
    else
        *newValue = "";
}

void VariablesTab::onCellChanged(const int row, int) {
    QComboBox *typeCombo = qobject_cast<QComboBox*>(table->cellWidget(row, 2));
    if (!typeCombo) return;
    QTableWidgetItem *nameItem = table->item(row, 0);
    QTableWidgetItem *valueItem = table->item(row, 1);

    const QString originalName = nameItem->data(Qt::UserRole).toString();
    const QVariant originalValue = valueItem->data(Qt::UserRole).toString();
    const VariableType type = static_cast<VariableType>(typeCombo->currentData().toInt());
    QString validatedName = originalName;
    QVariant validatedValue = originalValue;
    validateNewData(validatedName, validatedValue, type, &validatedName, &validatedValue);

    if(originalName != validatedName)
        exe_replaceVariables(
            QVector{originalName},
            QVector{validatedName},
            QVector{validatedValue},
            QVector{type}
        );
    else if(originalValue != validatedValue)
        exe_changeVariables(
            QVector{originalName},
            QVector{validatedValue}
        );
}

void VariablesTab::onCellDoubleClicked(const int row, const int col) {
    QTableWidgetItem *item = table->item(row, col);
    if(!item) return;
}

void VariablesTab::onAddVariable(){
    QString validatedName = "";
    QVariant validatedValue = "";
    validateNewData(validatedName, validatedValue, lastEnteredType, &validatedName, &validatedValue);
    exe_addVariables(
        QVector{validatedName},
        QVector{validatedValue},
        QVector{lastEnteredType}
    );
}

void VariablesTab::onRemoveVariable() {
    QList<QTableWidgetSelectionRange> selectedRanges = table->selectedRanges();
    if (selectedRanges.isEmpty())
        return;

    std::sort(selectedRanges.begin(), selectedRanges.end(),
        [](const QTableWidgetSelectionRange &a, const QTableWidgetSelectionRange &b) {
        return a.bottomRow() > b.bottomRow();
    });

    QVector<QString> names;
    for(const QTableWidgetSelectionRange &range : selectedRanges) {
        for (int row = range.bottomRow(); row >= range.topRow(); --row){
            QTableWidgetItem *nameItem = table->item(row, 0);
            if(nameItem)
                names.append(nameItem->data(Qt::UserRole).toString());
        }
    }

    exe_removeVariables(names);

    if (isEmpty()) {
        table->setVisible(false);
        noDataLabel->setVisible(true);
        removeButton->setEnabled(false);
    }
}

void VariablesTab::addVariables(
    const QVector<QString> &names,
    const QVector<QVariant> &values,
    const QVector<VariableType> &types
){
    for(int i = 0; i < names.size(); ++i){
        QString validatedName = names[i];
        QVariant validatedValue = values[i];
        validateNewData(names[i], values[i], types[i], &validatedName, &validatedValue);
        buildRow(0, validatedName, validatedValue, types[i]);
    }
    mainPanel->selectionTab->onAddVariables(names, values, types);
}
void VariablesTab::changeVariables(
    const QVector<QString> &names,
    const QVector<QVariant> &newValues
){
    for(int i = 0; i < names.size(); ++i){
        const int row = rowOfName(names[i]);
        QTableWidgetItem *valueItem = table->item(row, 1);
        if(!valueItem) continue;
        table->blockSignals(true);
        valueItem->setData(Qt::UserRole, newValues[i]);
        table->blockSignals(false);
    }

    mainPanel->selectionTab->onChangeVariables(names, newValues);
}
void VariablesTab::removeVariables(const QVector<QString> &names){
    for(int i = 0; i < names.size(); ++i)
        table->removeRow(rowOfName(names[i]));

    mainPanel->selectionTab->onRemoveVariables(names);
}



// Returns the row of a given type combo in the Type column.
int VariablesTab::rowOfTypeComboBox(QComboBox *typeCombo) const {
    for (int row = 0; row < table->rowCount(); ++row) {
        QComboBox *combo = qobject_cast<QComboBox*>(table->cellWidget(row, 2));
        if (combo == typeCombo)
            return row;
    }
    return -1;
}

// Checks whether the Name column already contains the given name, ignoring the given row.
bool VariablesTab::containsName(const QString &name, int ignoreRow) {
    for (int row = 0; row < table->rowCount(); ++row) {
        if (row == ignoreRow)
            continue;
        QTableWidgetItem *item = table->item(row, 0);
        if (item && item->text() == name)
            return true;
    }
    return false;
}

// Returns the row index for the given name (if needed).
int VariablesTab::rowOfName(const QString &name) const {
    for (int row = 0; row < table->rowCount(); ++row) {
        QTableWidgetItem *item = table->item(row, 0);
        if (item && item->text() == name)
            return row;
    }
    return -1;
}

// Compute a numeric suffix for names that start with a given key.
int VariablesTab::suffix(const QString &key) {
    QSet<int> existing;
    for (int row = 0; row < table->rowCount(); ++row) {
        QTableWidgetItem *item = table->item(row, 0);
        if (item && item->text().startsWith(key)) {
            bool ok;
            int s = item->text().mid(key.length()).toInt(&ok);
            if (ok)
                existing.insert(s);
        }
    }
    int ret = 0;
    while (existing.contains(ret))
        ret++;
    return ret;
}




QVector<QString> VariablesTab::names() const {
    const int rows = table->rowCount();
    QVector<QString> result;
    result.reserve(rows);
    for (int row = 0; row < rows; ++row) {
        if (auto nameItem = table->item(row, 0))
            result.append(nameItem->data(Qt::UserRole).toString());
    }
    return result;
}

QVector<QString> VariablesTab::names(const VariableType type) const {
    const int rows = table->rowCount();
    QVector<QString> result;
    result.reserve(rows);
    for (int row = 0; row < rows; ++row) {
        if (auto combo = qobject_cast<QComboBox*>(table->cellWidget(row, 2))) {
            const VariableType curType = static_cast<VariableType>(combo->currentData().toInt());
            if (curType != type)
                continue;
            if (auto nameItem = table->item(row, 0))
                result.append(nameItem->data(Qt::UserRole).toString());
        }
    }
    return result;
}

QVector<QVariant> VariablesTab::values() const {
    const int rows = table->rowCount();
    QVector<QVariant> result;
    result.reserve(rows);
    for (int row = 0; row < rows; ++row) {
        if (auto valueItem = table->item(row, 1))
            result.append(valueItem->data(Qt::UserRole));
    }
    return result;
}

QVector<QVariant> VariablesTab::values(const VariableType type) const {
    const int rows = table->rowCount();
    QVector<QVariant> result;
    result.reserve(rows);
    for (int row = 0; row < rows; ++row) {
        if (auto combo = qobject_cast<QComboBox*>(table->cellWidget(row, 2))) {
            const VariableType curType = static_cast<VariableType>(combo->currentData().toInt());
            if (curType != type)
                continue;
            if (auto valueItem = table->item(row, 1))
                result.append(valueItem->data(Qt::UserRole));
        }
    }
    return result;
}

QVector<VariableType> VariablesTab::types() const {
    const int rows = table->rowCount();
    QVector<VariableType> result;
    result.reserve(rows);
    for (int row = 0; row < rows; ++row) {
        if (auto combo = qobject_cast<QComboBox*>(table->cellWidget(row, 2))) {
            result.append(static_cast<VariableType>(combo->currentData().toInt()));
        }
    }
    return result;
}

QVector<QPair<QString, QVariant>> VariablesTab::filter(const VariableType type) const {
    const int rows = table->rowCount();
    QVector<QPair<QString, QVariant>> result;
    result.reserve(rows);
    for (int row = 0; row < rows; ++row) {
        if (auto combo = qobject_cast<QComboBox*>(table->cellWidget(row, 2))) {
            const VariableType curType = static_cast<VariableType>(combo->currentData().toInt());
            if (curType != type)
                continue;
            auto nameItem = table->item(row, 0);
            auto valueItem = table->item(row, 1);
            if (nameItem && valueItem)
                result.append(qMakePair(nameItem->data(Qt::UserRole).toString(), valueItem->data(Qt::UserRole)));
        }
    }
    return result;
}

QString VariablesTab::firstName() const {
    if (isEmpty())
        return "";
    if (auto nameItem = table->item(0, 0))
        return nameItem->data(Qt::UserRole).toString();
    return "";
}

QString VariablesTab::firstName(const VariableType type) const {
    const int rows = table->rowCount();
    for (int row = 0; row < rows; ++row) {
        if (auto combo = qobject_cast<QComboBox*>(table->cellWidget(row, 2))) {
            const VariableType curType = static_cast<VariableType>(combo->currentData().toInt());
            if (curType != type)
                continue;
            if (auto nameItem = table->item(row, 0))
                return nameItem->data(Qt::UserRole).toString();
        }
    }
    return "";
}

QVariant VariablesTab::firstValue() const {
    if (isEmpty())
        return "";
    if (auto valueItem = table->item(0, 1))
        return valueItem->data(Qt::UserRole);
    return "";
}

QVariant VariablesTab::firstValue(const VariableType type) const {
    const int rows = table->rowCount();
    for (int row = 0; row < rows; ++row) {
        if (auto combo = qobject_cast<QComboBox*>(table->cellWidget(row, 2))) {
            const VariableType curType = static_cast<VariableType>(combo->currentData().toInt());
            if (curType != type)
                continue;
            if (auto valueItem = table->item(row, 1))
                return valueItem->data(Qt::UserRole);
        }
    }
    return "";
}

QPair<QString, QVariant> VariablesTab::first() const {
    if (isEmpty())
        return qMakePair(QString(), QVariant());
    auto nameItem = table->item(0, 0);
    auto valueItem = table->item(0, 1);
    if (!nameItem || !valueItem)
        return qMakePair(QString(), QVariant());
    return qMakePair(nameItem->data(Qt::UserRole).toString(), valueItem->data(Qt::UserRole));
}

QPair<QString, QVariant> VariablesTab::first(const VariableType type) const {
    const int rows = table->rowCount();
    for (int row = 0; row < rows; ++row) {
        if (auto combo = qobject_cast<QComboBox*>(table->cellWidget(row, 2))) {
            const VariableType curType = static_cast<VariableType>(combo->currentData().toInt());
            if (curType != type)
                continue;
            auto nameItem = table->item(row, 0);
            auto valueItem = table->item(row, 1);
            if (nameItem && valueItem)
                return qMakePair(nameItem->data(Qt::UserRole).toString(), valueItem->data(Qt::UserRole));
        }
    }
    return qMakePair(QString(), QVariant());
}

bool VariablesTab::isEmpty() const {
    return table->rowCount() == 0;
}

bool VariablesTab::isEmpty(const VariableType type) const {
    const int rows = table->rowCount();
    for (int row = 0; row < rows; ++row) {
        if (auto combo = qobject_cast<QComboBox*>(table->cellWidget(row, 2))) {
            const VariableType curType = static_cast<VariableType>(combo->currentData().toInt());
            if (curType == type)
                return false;
        }
    }
    return true;
}

int VariablesTab::count() const {
    return table->rowCount();
}

int VariablesTab::count(const VariableType type) const {
    int cnt = 0;
    const int rows = table->rowCount();
    for (int row = 0; row < rows; ++row) {
        if (auto combo = qobject_cast<QComboBox*>(table->cellWidget(row, 2))) {
            const VariableType curType = static_cast<VariableType>(combo->currentData().toInt());
            if (curType == type)
                ++cnt;
        }
    }
    return cnt;
}


QVariant VariablesTab::value(const int row) const {
    auto valueItem = table->item(row, 1);
    if(valueItem) return valueItem->data(Qt::UserRole);
    return "";
}
VariableType VariablesTab::vartype(const int row) const {
    auto combo = qobject_cast<QComboBox*>(table->cellWidget(row, 2));
    if(combo) return static_cast<VariableType>(combo->currentData().toInt());
    return VAR_NULL;
}

