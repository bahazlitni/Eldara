#pragma once
#include <QPushButton>
#include "widgets/groups/InputGroup.h"
#include "App.h"

class BooleanInput : public QPushButton {
    Q_OBJECT

private:
    const QString key;
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
    BooleanInput(InputGroup *group, const QString &key, QWidget *parent = nullptr)
        : QPushButton(parent), key(key), group(group) {
        setCheckable(true);
        connect(this, &QPushButton::toggled, this, &BooleanInput::onCheckStateChanged);
        setCursor(Qt::PointingHandCursor);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        setMaximumWidth(32);
    }

    // Constructor with icons
    BooleanInput(
        InputGroup *group,
        const QString &key,
        const QIcon &checkedIcon,
        const QIcon &uncheckedIcon,
        const QSize &size,
        QWidget *parent = nullptr
    )
        : QPushButton(parent), key(key), group(group), checkedIcon(checkedIcon), uncheckedIcon(uncheckedIcon), hasIcons(true) {
        setCheckable(true);
        connect(this, &QPushButton::toggled, this, &BooleanInput::onCheckStateChanged);
        setCursor(Qt::PointingHandCursor);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setFixedSize(size);
        updateIcon();

        setStyleSheet("border: none;");

    }

    void updateData() {
        if (group->isEmpty()) {
            setCheckInternally(false);
            setEnabled(false);
        } else {
            const QString data = group->dataString(key);
            setEnabled(true);
            setCheckInternally(data == "1");
        }
    }

private slots:
    void onCheckStateChanged(bool checked) {
        group->apply(key, checked ? "1" : "0");
        group->app->update();
        updateIcon();
    }
};
