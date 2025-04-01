#include "ObjectGroup.h"
#include "tools/Selector.h"
#include "objects/Object.h"
#include "App.h"

#include <QToolButton>
#include <QVBoxLayout>
#include <QStringList>
#include <QStyle>
#include "commands/ChangeAttributeCommand.h"

void ObjectGroup::updateData(){
    updateSelection(app->selector.filter(type()));
}

QString ObjectGroup::baseTitle() const {
    return selection.size() == 1 ? Object::name(type()) : Object::pluralName(type());
}


void ObjectGroup::updateBasicData(){
    if(isEmpty()){
        setVisible(false);
        return;
    }
    setVisible(true);
    headerButton->setText(title());
}

bool ObjectGroup::isMixed(const QString &key) const {
    if (selection.isEmpty()) return false;
    const auto &firstValue = (*selection.constBegin())->dataString(key).toLower();
    return std::any_of(selection.begin(), selection.end(), [&](const auto &obj) {
        return obj->dataString(key).toLower() != firstValue;
    });
}

void ObjectGroup::apply(const QString &key, const QString &value){
    for(const auto &obj : selection) obj->setData(key, value);
}

void ObjectGroup::onEditingFinishedApply(const QString &key, const QString &value){
    if(lastApplied.contains(key) && lastApplied[key] == value) return;
    app->execute(std::make_unique<ChangeAttributeCommand>(app, selection, key, value));
    lastApplied[key] = value;
}

QString ObjectGroup::dataString(const QString &key) const {
    if(isEmpty()) return "";
    if (key == "id") {
        QStringList ids;
        std::transform(selection.begin(), selection.end(), std::back_inserter(ids),
        [](const auto &obj) { return obj->dataString("id"); });
        if (ids.isEmpty()) return "";
        return ids.size() == 1? ids[0] : QString("{%1}").arg(ids.join(", "));
    }
    return isMixed(key)? "Mixed" : (*selection.constBegin())->dataString(key);
}
