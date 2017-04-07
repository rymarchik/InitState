#include "receiversDialog.h"
#include "mainwindow.h"

ReceiversDialog::ReceiversDialog(QWidget* parent) :
    QDialog(parent, Qt::WindowCloseButtonHint)
{
    checkBoxList = new QList<QCheckBox*>;
    labelList = new QList<QLabel*>;

    okButton = new QPushButton("Принять");
    cancelButton = new QPushButton("Отменить");

    checkBoxLayout = new QVBoxLayout;
//    labelLayout->setContentsMargins(0, 10, 15, 0);
    labelLayout = new QVBoxLayout;

    QHBoxLayout* infoLayout = new QHBoxLayout;
    infoLayout->addLayout(checkBoxLayout);
    infoLayout->addLayout(labelLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
//    buttonLayout->setSpacing(33);
//    buttonLayout->setContentsMargins(0, 10, 0, 0);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(infoLayout);
    mainLayout->addLayout(buttonLayout);
//    mainLayout->setContentsMargins(25, 10, 25, 15);
    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    setWindowTitle("Получатели");
}

void ReceiversDialog::clearLayout(QLayout* layout) {
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (QWidget* widget = item->widget())
            delete widget;
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout);
        delete item;
    }
}

void ReceiversDialog::setCheckBoxes(int n) {
    clearLayout(checkBoxLayout);
    checkBoxList->clear();

    for (int i = 0; i < n; i++) {
        checkBoxList->append(new QCheckBox);
        checkBoxLayout->addWidget(checkBoxList->at(i));
    }
}

void ReceiversDialog::setLabelNames(QStringList list) {
    clearLayout(labelLayout);
    labelList->clear();

    for (int i = 0; i < list.size(); i++) {
        labelList->append(new QLabel(list.at(i)));
        labelLayout->addWidget(labelList->last());
//        if (!list.at(i).contains("\n")) {
//            labelLayout->addSpacing(12);
//        }
    }
}


