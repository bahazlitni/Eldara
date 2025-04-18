#pragma once
#include "utils/Globals.h"

#include <QString>
#include <QIcon>
#include <QColor>

#include <QWidget>
#include <QToolButton>
#include <QGridLayout>

class ColorInput;
class MainPanel;

class InputGroup : public QWidget {
    Q_OBJECT
protected:
    QToolButton *headerButton;
    QWidget *contentWidget;
    QGridLayout *contentLayout;
    const QIcon collapsedIcon;
    const QIcon expandedIcon;

    QString prefix = "";
    QString suffix = "";

public:
    MainPanel *mainPanel;
    InputGroup(MainPanel *mainPanel, QWidget *parent = nullptr);

    void setTitlePrefix(const QString &str){ prefix = str; }
    void setTitleSuffix(const QString &str){ suffix = str; }

    virtual QString baseTitle() const { return ""; }
    QString title() const { return prefix + baseTitle() + suffix; }

    virtual void updateData(){}
    virtual bool isEmpty() const = 0;

    virtual bool isMixed(const Attr attr) const = 0;
    virtual QVariant getAttr(const Attr attr) const = 0;
    virtual void setAttr(const Attr attr, const QVariant &v) = 0;
    virtual void confirmAttr(const Attr attr, const QVariant &v) = 0;


    virtual bool isMixedParam([[maybe_unused]] const Param param) const { return false; }
    virtual double paramValue([[maybe_unused]] const Param param) const {
        return 0.0;
    }
    virtual QString paramVarname([[maybe_unused]] const Param param) const {
        return "";
    }

    virtual void setParamValue(
        [[maybe_unused]] const Param param,
        [[maybe_unused]] const double value
    ) {}
    virtual void confirmParamValue(
        [[maybe_unused]] const Param param,
        [[maybe_unused]] const double value
    ) {}
    virtual void setParamVarname(
        [[maybe_unused]] const Param param,
        [[maybe_unused]] const QString &varname
    ) {}
    virtual void confirmParamVarname(
        [[maybe_unused]] const Param param,
        [[maybe_unused]] const QString &varname
    ) {}

    virtual QColor getColor([[maybe_unused]] ColorInput *colorInput) const {
        return Qt::black;
    }

    virtual void setColor(
        [[maybe_unused]] ColorInput *colorInput,
        [[maybe_unused]] const QColor &newColor
    ) {}
    virtual void confirmColor(
        [[maybe_unused]] ColorInput *colorInput,
        [[maybe_unused]] const QColor &newColor
    ) {}

public slots:
    void onToggled(bool checked);
};
