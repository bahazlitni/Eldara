#include "InputGroup.h"
#include "widgets/MainPanel.h"

#include <QSize>

#include <QVBoxLayout>

// Constructor: sets up the header button and content area.
InputGroup::InputGroup(MainPanel *mainPanel, QWidget *parent):
    QWidget(parent),
    headerButton(new QToolButton(this)),
    contentWidget(new QWidget(this)),
    contentLayout(new QGridLayout(contentWidget)),
    collapsedIcon(QIcon(":/assets/icons/arrow_right.svg")),
    expandedIcon(QIcon(":/assets/icons/arrow_down.svg")),
    mainPanel(mainPanel)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    // Create the header button.
    headerButton->setLayoutDirection(Qt::LeftToRight);
    headerButton->setCheckable(true);
    headerButton->setChecked(true);
    headerButton->setText("");
    headerButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    headerButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    headerButton->setIconSize(QSize(16, 16));
    headerButton->setCursor(Qt::PointingHandCursor);
    headerButton->setStyleSheet(
        "QToolButton {"
        "   color: #ddd;"
        "   border-bottom: 1px solid #333;"
        "   border-top: 1px solid #333;"
        "   padding: 6px 6px;"
        "   text-align: left;"
        "   font-size: 14px;"
        "} "
        "QToolButton:hover {"
        "   background-color: #303030;"
        "} "
        "QToolButton:pressed {"
        "   background-color: #333333;"
        "}"
    );

    onToggled(true);

    // Connect the toggled signal to update content visibility.
    connect(headerButton, &QToolButton::toggled, this, &InputGroup::onToggled);

    // Create the content widget and its layout.
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setVerticalSpacing(16);
    contentLayout->setHorizontalSpacing(20);

    // Assemble the layout: header button on top, content widget below.
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(headerButton);
    layout->addWidget(contentWidget);
    setLayout(layout);

    setVisible(false);
}

// Slot called when the header button is toggled.
void InputGroup::onToggled(bool checked) {
    contentWidget->setVisible(checked);
    headerButton->setIcon(checked ? expandedIcon : collapsedIcon);
}
