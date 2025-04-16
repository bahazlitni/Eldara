#pragma once
#include <QMainWindow>
#include <QSplitter>
#include <QMenu>
#include <QAction>

class Scene;
class MainPanel;

class Window : public QMainWindow {
    Q_OBJECT

public:
    explicit Window();

private:
    QSplitter *splitter;
    QMenu *fileMenu;
    QMenu *editMenu;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *preferencesAction;
    Scene *scene;
    MainPanel *mainPanel;

    void setupMenuBar();
    void setupSplitter();
    void setupConnections();

private slots:
    void openFile();
    void saveFile();
    void saveFileAs();
    void openPreferences();
    void applySettings();

protected:
    void closeEvent(QCloseEvent *event) override;
};
