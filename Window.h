#pragma once
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QComboBox>
#include <QPainter>
#include <QTimer>
#include <QSplitter>

class App;
class MainPanel;
class Window : public QMainWindow {
    Q_OBJECT

public:
    explicit Window();

private:
    QSplitter *splitter;
    QToolBar *toolbar;
    App *app;
    MainPanel *mainPanel;

    void setupSplitter();
    void setupToolBar();
    void setupConnections();

protected:
    void closeEvent([[maybe_unused]] QCloseEvent *event) override {};
};
