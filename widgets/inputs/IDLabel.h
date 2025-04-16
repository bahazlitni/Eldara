#pragma once
#include <QLabel>
#include <QLineEdit>
#include <QFontMetrics>
#include "App.h"
#include "widgets/groups/InputGroup.h"
#include "objects/Object.h"

class IDLabel: public QLabel {
private:
    InputGroup *group;
    bool clipText = true;

public:
    IDLabel(InputGroup *group, QWidget *parent = nullptr): QLabel(parent), group(group) {
        setAlignment(Qt::AlignCenter);
        setTextFormat(Qt::PlainText);
        setWordWrap(false);
    }

    void updateData(){
        const QString text = group->getAttr(Attr::ID).toString();

        if (clipText && !text.isEmpty()) {
            // Get available width for the label
            int availableWidth = width() - 10;  // Subtract some padding
            QFontMetrics metrics(font());

            // Check if text needs to be clipped
            if (metrics.horizontalAdvance(text) > availableWidth) {
                setText(metrics.elidedText(text, Qt::ElideMiddle, availableWidth));
                setToolTip(text);  // Full text available on hover
            } else {
                setText(text);
                setToolTip("");  // No tooltip needed
            }
        } else {
            setText(text);
            setToolTip("");
        }
    }

    // Toggle text clipping
    void setClipText(bool clip) {
        clipText = clip;
        updateData();
    }

    // Override resizeEvent to update clipping when the label is resized
    void resizeEvent(QResizeEvent *event) override {
        QLabel::resizeEvent(event);
        updateData();  // Re-evaluate clipping after resize
    }
};
