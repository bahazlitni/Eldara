#pragma once
#include "utils/Globals.h"

#include "widgets/custom/CustomSpinBox.h"
#include "widgets/groups/InputGroup.h"
#include "widgets/MainPanel.h"

#include "Scene.h"

#include <QLineEdit>

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
            this->group->confirmAttr(Attr::Radius, this->value());
            this->updateData();
            this->group->mainPanel->scene->update();
        });

        connect(this, &CustomSpinBox::valueChanged, [this](int newValue){
            this->group->setAttr(Attr::Radius, newValue);
            this->group->mainPanel->scene->update();
        });
    }

    void wheelEvent(QWheelEvent *event) override {
        event->ignore();
    }

    void updateData(){
        if(group->isMixed(Attr::Radius)){
            lineEdit()->clear();
            lineEdit()->setPlaceholderText("Mixed");
            return;
        }
        setValue(group->getAttr(Attr::Radius).toInt());
    }
};
