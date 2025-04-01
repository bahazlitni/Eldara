#pragma once
#include <QIcon>
#include <QString>
#include <QToolButton>
#include <QWidget>
#include <QGridLayout>

class App;
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
    App *app;
    InputGroup(App *app, QWidget *parent = nullptr);

    void setTitlePrefix(const QString &str){ prefix = str; }
    void setTitleSuffix(const QString &str){ suffix = str; }

    virtual QString baseTitle() const { return ""; }
    QString title() const { return prefix + baseTitle() + suffix; }

    virtual void updateData(){}

    virtual void apply(const QString &key, const QString &value) = 0;
    virtual void onEditingFinishedApply(const QString &key, const QString &value) = 0;
    virtual QString dataString([[maybe_unused]] const QString &key) const { return ""; }
    virtual bool isEmpty() const { return false; }
    virtual bool isMixed([[maybe_unused]] const QString &key) const { return false; }

public slots:
    void onToggled(bool checked);
};
