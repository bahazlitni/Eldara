#include "Window.h"

#include "App.h"
#include "widgets/MainPanel.h"
#include "widgets/tabs/SelectionTab.h"
#include "widgets/tabs/QuickSettingsTab.h"
#include "tools/Selector.h"
#include "tools/Pen.h"

#include "widgets/tabs/VariablesTab.h"

#include <QFrame>
#include <QSplitter>

Window::Window(): QMainWindow(),
    splitter(new QSplitter(Qt::Horizontal, this)),
    toolbar(new QToolBar(this)),
    app(new App(splitter)),
    mainPanel(new MainPanel(app, splitter))
{
    setWindowTitle("Circuits Simulator");
    setupSplitter();
    setupToolBar();
    setupConnections();
}

void Window::setupToolBar(){
    addToolBar(toolbar);
}

void Window::setupSplitter(){
    setCentralWidget(splitter);
    splitter->setHandleWidth(1);
    splitter->addWidget(app);
    splitter->addWidget(mainPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    splitter->setCollapsible(1, false);
    splitter->setSizes({app->width(), mainPanel->minimumWidth()});
    splitter->setStyleSheet(R"(
        QSplitter::handle {
            background-color: #333;
        }
    )");
}

void Window::setupConnections(){
    connect(
        &app->selector, &Selector::selectionChanged,
        mainPanel->selectionTab, &SelectionTab::updateData
    );
    connect(
        &app->selector, &Selector::selectionDragged,
        mainPanel->selectionTab, &SelectionTab::updateCoordinates
    );

    connect(
        &app->pen, &Pen::dataChanged,
        mainPanel->quickSettingsTab, &QuickSettingsTab::updatePenData
    );
}
