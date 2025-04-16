#pragma once
#include <QLineEdit>
#include <QCompleter>
#include "widgets/groups/InputGroup.h"
#include "Scene.h"
#include "objects/Alias.h"

#include "widgets/inputs/BooleanInput.h"

#include <QStringListModel>

#include <QRegularExpressionValidator>
#include <QRegularExpression>

#include <QWheelEvent>


class AddressInput: public QWidget {
    Q_OBJECT
private:
    QLineEdit *input;
    QCompleter *completer;
    InputGroup *group;
    BooleanInput *ShowLabel;
    QHBoxLayout *mainLayout;

public:
    AddressInput(InputGroup *group, QWidget *parent = nullptr):
        QWidget(parent),
        input(new QLineEdit(this)),
        completer(new QCompleter(this)),
        group(group),
        ShowLabel(
            new BooleanInput(
                group, Attr::ShowLabel,
                QIcon(":/assets/icons/eye_opened.png"),
                QIcon(":/assets/icons/eye_shut.png"),
                QSize(16,16),
                parent
            )
        ),
        mainLayout(new QHBoxLayout(this))
    {
        setLayout(mainLayout);
        mainLayout->addWidget(input);
        mainLayout->addWidget(ShowLabel);
        mainLayout->setSpacing(8);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        input->setAlignment(Qt::AlignCenter);

        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setFilterMode(Qt::MatchContains);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        input->setCompleter(completer);

        QRegularExpression regex("^[A-Za-z0-9]+$");
        QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex, this);
        input->setValidator(validator);

        connect(input, &QLineEdit::textChanged, this, &AddressInput::updateSuggestions);

        // Scenely new value when editing finishes
        connect(input, &QLineEdit::editingFinished, [this]() {
            QString newText = this->input->text().trimmed().toUpper();

            if (newText.isEmpty()) {
                this->input->clear();
                return;
            }

            if (!newText.contains(QRegularExpression("^[A-Za-z0-9]+$"))) {
                this->input->clear();
                return;
            }

            this->group->confirmAttr(Attr::Address, newText);
            this->updateData();
        });
    }
    void updateData(){
        if(group->isMixed(Attr::Address)) {
            input->clear();
            input->setPlaceholderText("Mixed");
        }
        else {
            input->setText(group->getAttr(Attr::Address).toString());
        }
        ShowLabel->updateData();
    }

    void wheelEvent(QWheelEvent *event) override {
        event->ignore();
    }

private:
    void updateSuggestions(const QString &input) {
        if (input.isEmpty()) {
            completer->setModel(nullptr);
            return;
        }

        QStringList filteredAddresses;
        for(const auto &alias : group->mainPanel->scene->aliases){
            const QString &address = alias->address();
            if (address.startsWith(input, Qt::CaseInsensitive))
                filteredAddresses.append(address);
        }

        QStringListModel *model = filteredAddresses.isEmpty()? nullptr
        : new QStringListModel(filteredAddresses, completer);
        completer->setModel(model);
    }
};
