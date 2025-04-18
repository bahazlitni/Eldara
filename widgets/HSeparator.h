#pragma once
#include <QHBoxLayout>
#include <QFrame>
#include <QWidget>

class HSeparator : public QWidget {
public:
    explicit HSeparator(int margin = 10, QWidget *parent = nullptr) : QWidget(parent) {
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, margin, 0, margin);

        QFrame *line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setStyleSheet("background-color: #333; max-height: 1px; border: none;");

        layout->addWidget(line);
        setLayout(layout);
    }
};
