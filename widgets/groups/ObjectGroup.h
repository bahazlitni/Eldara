#pragma once
#include "InputGroup.h"
#include "utils/Globals.h"

class MainPanel;
class ObjectGroup : public InputGroup {
    Q_OBJECT
private:
    void updateBasicData();

public:
    Selection selection;

    ObjectGroup(MainPanel *mainPanel, QWidget *parent = nullptr): InputGroup(mainPanel, parent) {}

    virtual ObjectType type() const = 0;
    virtual ObjectCategory category() const = 0;
    virtual void updateCoordinates(){}

    QString baseTitle() const override;

    bool isEmpty() const override { return selection.isEmpty(); }
    bool isMixed(const Attr attr) const override;

    QVariant getAttr(const Attr attr) const override;
    void setAttr(const Attr attr, const QVariant &v) override;
    void confirmAttr(const Attr attr, const QVariant &v) override;

    void updateData() override;

    virtual void updateSelection(const Selection &selection){
        this->selection = QSet(selection);
        updateBasicData();
    }
};
