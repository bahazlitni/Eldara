#include "ObjectGroup.h"
#include "tools/Selector.h"
#include "objects/Object.h"

#include <QToolButton>
#include <QVBoxLayout>
#include <QStringList>
#include <QStyle>

#include "widgets/MainPanel.h"
#include "App.h"


void ObjectGroup::updateData(){
    updateSelection(mainPanel->app->selector.filter(type()));
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

bool ObjectGroup::isMixed(const Attr attr) const {
    if (selection.isEmpty()) return false;
    const auto &firstValue = (*selection.constBegin())->getAttr(attr);
    return std::any_of(selection.begin(), selection.end(), [&](const auto &obj) {
        return obj->getAttr(attr) != firstValue;
    });
}

QVariant ObjectGroup::getAttr(const Attr attr) const {
    if(isEmpty()) return "";
    if (attr == Attr::ID) {
        QStringList ids;
        std::transform(selection.begin(), selection.end(), std::back_inserter(ids),
        [](const auto &obj) { return obj->getAttr(Attr::ID).toString(); });
        if (ids.isEmpty()) return "";
        return ids.size() == 1? ids[0] : QString("{%1}").arg(ids.join(", "));
    }
    return (*selection.constBegin())->getAttr(attr);
}
void ObjectGroup::setAttr(const Attr attr, const QVariant &v) {
    for(const auto &obj : selection) obj->setAttr(attr, v);
}
void ObjectGroup::confirmAttr(const Attr attr, const QVariant &v) {
    for(const auto &obj : selection) obj->setAttr(attr, v);
}


