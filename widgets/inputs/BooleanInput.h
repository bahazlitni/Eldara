#pragma once
#include <QPushButton>
#include "widgets/groups/InputGroup.h"
#include "Scene.h"
#include "widgets/MainPanel.h"

class BooleanInput : public QPushButton {
    Q_OBJECT

private:
    const Attr attr;
    InputGroup *group;
    QIcon checkedIcon;
    QIcon uncheckedIcon;
    bool hasIcons = false;

    void setCheckInternally(bool status) {
        blockSignals(true);
        setChecked(status);
        updateIcon();
        blockSignals(false);
    }

    void updateIcon() {
        if(hasIcons) setIcon(isChecked() ? checkedIcon : uncheckedIcon);
    }

public:
    // Constructor without icons
    BooleanInput(InputGroup *group, const Attr attr, QWidget *parent = nullptr)
        : QPushButton(parent), attr(attr), group(group) {
        setCheckable(true);
        connect(this, &QPushButton::toggled, this, &BooleanInput::onCheckStateChanged);
        setCursor(Qt::PointingHandCursor);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        setMaximumWidth(32);
    }

    // Constructor with icons
    BooleanInput(
        InputGroup *group,
        const Attr attr,
        const QIcon &checkedIcon,
        const QIcon &uncheckedIcon,
        const QSize &size,
        QWidget *parent = nullptr
    )
        : QPushButton(parent), attr(attr), group(group), checkedIcon(checkedIcon), uncheckedIcon(uncheckedIcon), hasIcons(true) {
        setCheckable(true);
        connect(this, &QPushButton::toggled, this, &BooleanInput::onCheckStateChanged);
        setCursor(Qt::PointingHandCursor);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setFixedSize(size);
        updateIcon();

        setStyleSheet("border: none;");

    }

    void updateData() {
        const bool isEmpty = group->isEmpty();
        setEnabled(!isEmpty);
        setCheckInternally(!isEmpty && group->getAttr(attr).toBool());
    }

private slots:
    void onCheckStateChanged(bool checked) {
        group->setAttr(attr, checked);
        group->mainPanel->scene->update();
        updateIcon();
    }
};
