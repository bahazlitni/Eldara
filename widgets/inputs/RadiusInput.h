#pragma once
#include <QLineEdit>
#include "widgets/custom/CustomSpinBox.h"
#include "utils/Types.h"
#include "widgets/groups/InputGroup.h"
#include "App.h"

#include <QWheelEvent>


class RadiusInput: public CustomSpinBox {
    Q_OBJECT

private:
    InputGroup *group;

public:
    RadiusInput(InputGroup *group, QWidget *parent = nullptr): CustomSpinBox(parent), group(group) {
        setAlignment(Qt::AlignCenter);
        setRange(6, 18);

        connect(this, &CustomSpinBox::editingFinished, [this](){
            this->group->onEditingFinishedApply("radius", QString::number(this->value()));
            this->updateData();
            this->group->app->update();
        });

        connect(this, &CustomSpinBox::valueChanged, [this](int newValue){
            this->group->apply("radius", QString::number(newValue));
            this->group->app->update();
        });
    }

    void wheelEvent(QWheelEvent *event) override {
        event->ignore();
    }

    void updateData(){
        const QString data = group->dataString("radius");
        if(data == "Mixed"){
            lineEdit()->clear();
            lineEdit()->setPlaceholderText(data);
            return;
        }
        setValue(data.toInt());
    }
};
