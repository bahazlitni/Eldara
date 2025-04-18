#pragma once
#include "widgets/custom/CustomDoubleSpinBox.h"
#include "widgets/groups/InputGroup.h"
#include "widgets/MainPanel.h"

#include <QLineEdit>

#include "Grid.h"
#include "Scene.h"

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
            this->group->mainPanel->scene->update();
        });
    }

    void wheelEvent(QWheelEvent *event) override {
        event->ignore();
    }

    void updateCrement(){
        const double newValue = value();
        this->group->setAttr(this->attr, newValue);
        lastValue = newValue;
        this->group->mainPanel->scene->update();
    }

    void increment() override {
        setValue(group->mainPanel->scene->grid.snap(value() + group->mainPanel->scene->tileSize()));
        updateCrement();
    }
    void decrement() override {
        setValue(group->mainPanel->scene->grid.snap(value() - group->mainPanel->scene->tileSize()));
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
