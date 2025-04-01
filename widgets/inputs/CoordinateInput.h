#pragma once
#include "Grid.h"
#include "widgets/custom/CustomDoubleSpinBox.h"
#include <QLineEdit>
#include "widgets/groups/InputGroup.h"
#include "App.h"
#include <QWheelEvent>


class CoordinateInput: public CustomDoubleSpinBox {
    Q_OBJECT

private:
    double lastValue = 0.0;
    InputGroup *group;
    const QString key;

public:
    CoordinateInput(InputGroup *group, const QString &key, QWidget *parent = nullptr):
        CustomDoubleSpinBox(parent), group(group), key(key) {
        setAlignment(Qt::AlignCenter);
        setRange(-Grid::MAX_COORDINATE_RANGE, Grid::MAX_COORDINATE_RANGE);
        setDecimals(2);

        connect(this, &CustomDoubleSpinBox::editingFinished, [this](){
            this->group->onEditingFinishedApply(this->key, QString::number(this->value(), 'f', 2));
            this->updateData();
            this->group->app->update();
        });
    }

    void wheelEvent(QWheelEvent *event) override {
        event->ignore();
    }

    void updateCrement(){
        const double newValue = value();
        this->group->apply(this->key, QString::number(newValue, 'f', 2));
        lastValue = newValue;
        this->group->app->update();
    }

    void increment() override {
        setValue(group->app->grid.snap(value() + group->app->grid.tileSize()));
        updateCrement();
    }
    void decrement() override {
        setValue(group->app->grid.snap(value() - group->app->grid.tileSize()));
        updateCrement();
    }


    void updateData(){
        const QString data = group->dataString(key);
        if(group->isEmpty() || group->isMixed(key)) {
            lineEdit()->clear();
            lineEdit()->setText(data);
            setEnabled(false);
            return;
        }
        setValue(data.toFloat());
        setEnabled(true);
        lastValue = value();
    }
};
