#pragma once
#include "widgets/groups/InputGroup.h"
#include "App.h"

#include <QWheelEvent>
#include <QComboBox>

#include "Grid.h"
#include "tools\Pen.h"
#include "App.h"
#include "objects/Object.h"

class ObjectTypeInput: public QWidget {
    Q_OBJECT

private:
    InputGroup *group;

public:
    ObjectTypeInput(InputGroup *group, QWidget *parent = nullptr): QWidget(parent), group(group) {
    }

    void wheelEvent(QWheelEvent *event) override {
        event->ignore();
    }

    void updateData(){
        /*
        const QString typeData = group->dataString("type");
        const QString radiusData = group->dataString("radius");

        const ObjectType type = Object::parseType(typeData);
        const int radius = radiusData.toInt();
        */
    }
};
