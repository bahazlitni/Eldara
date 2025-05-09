#pragma once
#include "utils/Globals.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>

class MainPanel;
class PenGroup;

class QuickSettingsTab : public QWidget {
    Q_OBJECT

public:
    explicit QuickSettingsTab(MainPanel *mainPanel);
    MainPanel *mainPanel;

    void reset(){}
    inline void updateData() {
        updateGridData();
        updatePenData();
    }

public slots:
    void updateGridData();
    void updatePenData();

private:
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    PenGroup *penGroup;
};
