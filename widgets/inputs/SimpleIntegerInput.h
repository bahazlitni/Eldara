#pragma once
#include "utils/Globals.h"

#include "widgets/custom/CustomSpinBox.h"
#include "widgets/groups/InputGroup.h"
#include "widgets/MainPanel.h"

#include "Scene.h"

#include <QLineEdit>

#include <QWheelEvent>

class SimpleIntegerInput: public CustomSpinBox {
    Q_OBJECT

private:
    const Attr m_attr;
    InputGroup *group;

public:
    SimpleIntegerInput(
        InputGroup *group,
        const Attr attr,
        const int MIN,
        const int MAX,
        QWidget *parent = nullptr
    ): CustomSpinBox(parent), m_attr(attr), group(group) {
        setAlignment(Qt::AlignCenter);
        setRange(MIN, MAX);

        connect(this, &CustomSpinBox::editingFinished, [this](){
            this->group->confirmAttr(m_attr, this->value());
            this->updateData();
            this->group->mainPanel->scene->update();
        });

        connect(this, &CustomSpinBox::valueChanged, [this](int newValue){
            this->group->setAttr(m_attr, newValue);
            this->group->mainPanel->scene->update();
        });
    }

    void wheelEvent(QWheelEvent *event) override {
        event->ignore();
    }

    void updateData(){
        if(group->isMixed(m_attr)){
            lineEdit()->clear();
            lineEdit()->setPlaceholderText("Mixed");
            return;
        }
        setValue(group->getAttr(m_attr).toInt());
    }
};
