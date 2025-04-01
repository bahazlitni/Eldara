#pragma once

#include <QGroupBox>
#include <QSet>
#include <QVBoxLayout>
#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QScrollArea>

class App;
class PenGroup;
class QuickSettingsTab : public QWidget {
    Q_OBJECT

public:
    explicit QuickSettingsTab(App *app, QWidget *parent = nullptr);
    App *app;

public slots:
    void updateGridData();
    void updatePenData();

private:
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    PenGroup *penGroup;
};
