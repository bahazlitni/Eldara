#pragma once
#include "InputGroup.h"
#include "utils/Types.h"

class App;
class ObjectGroup : public InputGroup {
    Q_OBJECT
private:
    QHash<QString, QString> lastApplied;
    void updateBasicData();

public:
    LockedSelection selection;

    ObjectGroup(App *app, QWidget *parent = nullptr): InputGroup(app, parent) {}

    virtual ObjectType type() const { return VOID; }
    virtual ObjectCategory category() const { return _VOID; }
    virtual void updateCoordinates(){}

    QString baseTitle() const override;

    bool isEmpty() const override { return selection.isEmpty(); }
    bool isMixed(const QString &key) const override;
    QString dataString(const QString &key) const override;

    void apply(const QString &key, const QString &value) override;
    void onEditingFinishedApply(const QString &key, const QString &value) override;

    void updateData() override;

    virtual void updateSelection(const LockedSelection &selection){
        this->selection = QSet(selection);
        updateBasicData();
    }
};
