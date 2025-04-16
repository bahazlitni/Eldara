#pragma once
#include "Grid.h"
#include "widgets/custom/CustomDoubleSpinBox.h"
#include <QLineEdit>
#include "widgets/groups/InputGroup.h"
#include "App.h"
#include "widgets/MainPanel.h"

class CoordinateInput: public CustomDoubleSpinBox {
    Q_OBJECT

private:
    double lastValue = 0.0;
    InputGroup *group;
    const Attr attr;

public:
    CoordinateInput(InputGroup *group, const Attr attr, QWidget *parent = nullptr):
        CustomDoubleSpinBox(parent), group(group), attr(attr) {
        setAlignment(Qt::AlignCenter);
        setRange(-Grid::MAX_COORDINATE_RANGE, Grid::MAX_COORDINATE_RANGE);
        setDecimals(2);

        connect(this, &CustomDoubleSpinBox::editingFinished, [this](){
            this->group->confirmAttr(this->attr, this->value());
            this->updateData();
            this->group->mainPanel->app->update();
        });
    }

    void wheelEvent(QWheelEvent *event) override {
        event->ignore();
    }

    void updateCrement(){
        const double newValue = value();
        this->group->setAttr(this->attr, newValue);
        lastValue = newValue;
        this->group->mainPanel->app->update();
    }

    void increment() override {
        setValue(group->mainPanel->app->grid.snap(value() + group->mainPanel->app->grid.tileSize()));
        updateCrement();
    }
    void decrement() override {
        setValue(group->mainPanel->app->grid.snap(value() - group->mainPanel->app->grid.tileSize()));
        updateCrement();
    }


    void updateData(){
        if(group->isEmpty() || group->isMixed(attr)) {
            lineEdit()->clear();
            lineEdit()->setText("Mixed");
            setEnabled(false);
            return;
        }
        setValue(group->getAttr(attr).toFloat());
        setEnabled(true);
        lastValue = value();
    }
};
