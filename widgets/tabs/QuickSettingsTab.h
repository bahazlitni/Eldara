#pragma once

#include <QGroupBox>
#include <QSet>
#include <QVBoxLayout>
#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QScrollArea>

class MainPanel;
class PenGroup;
class QuickSettingsTab : public QWidget {
    Q_OBJECT

public:
    explicit QuickSettingsTab(MainPanel *mainPanel);
    MainPanel *mainPanel;

    void reset(){}

public slots:
    void updateGridData();
    void updatePenData();

private:
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    PenGroup *penGroup;
};
