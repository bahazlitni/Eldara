#pragma once
#include "InputGroup.h"
#include <QHash>
#include <QVector>
#include "utils/Types.h"

class App;
class ColorInput;

class ColorsGroup : public InputGroup {
    Q_OBJECT

private:
    QHash<QString, QString> lastApplied;

protected:
    // Maps a color key (e.g. "#FF00AA") to the selection of objects having that color.
    QHash<QString, LockedSelection> colorsSelection;
    QVector<ColorInput*> colorInputs;

public:
    ColorsGroup(App *app, QWidget *parent = nullptr);
    void updateData() override;
    bool isEmpty() const override { return colorsSelection.isEmpty(); }
    void apply(const QString &key, const QString &value) override;
    void onEditingFinishedApply(const QString &key, const QString &value) override;
};
