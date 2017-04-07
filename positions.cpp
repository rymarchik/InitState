#include "positions.h"
#include "utility.h"

Positions::Positions(QWidget* parent) : BaseTabWidget(parent)
{
    navigatorUpperTree = new QTreeWidget;

    navigatorTabLayout = new QVBoxLayout;
    navigatorTabLayout->addWidget(navigatorUpperTree);
    navigatorTabLayout->addWidget(navigatorLowerTable);

    navigatorTabWidget = new QWidget;
    navigatorTabWidget->setLayout(navigatorTabLayout);
}

void Positions::fillNavigator() {
    Utility::closeNewEditTab(this);
    navigatorLowerTable->clear();

    navigatorUpperTree->setColumnCount(1);
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < 10; ++i)
        items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("item: %1").arg(i))));
    navigatorUpperTree->insertTopLevelItems(0, items);
//    navigatorUpperTable->setColumnCount(1);
//    QStringList navigatorUpperTableHeaders;
//    navigatorUpperTableHeaders << "№ Наименование";
//    navigatorUpperTable->setHorizontalHeaderLabels(navigatorUpperTableHeaders);
//    navigatorUpperTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    navigatorLowerTable->setColumnCount(4);
    QStringList navigatorLowerTableHeaders;
    navigatorLowerTableHeaders << "Получатель" << "Время данных" << "Состояние отправки" << "Состояние передачи";
    navigatorLowerTable->setHorizontalHeaderLabels(navigatorLowerTableHeaders);
    navigatorLowerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void Positions::fillChanges() {
    Utility::closeNewEditTab(this);
    changesUpperTable->clear();
    changesLowerTable->clear();

    changesUpperTable->setColumnCount(3);
    QStringList changesUpperTableHeaders;
    changesUpperTableHeaders << "№ Наименование" << "Событие" << "Время события";
    changesUpperTable->setHorizontalHeaderLabels(changesUpperTableHeaders);
    changesUpperTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    changesLowerTable->setColumnCount(4);
    QStringList changesLowerTableHeaders;
    changesLowerTableHeaders << "Получатель" << "Время данных" << "Состояние отправки" << "Состояние передачи";
    changesLowerTable->setHorizontalHeaderLabels(changesLowerTableHeaders);
    changesLowerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void Positions::onAdd() {
    //реализация кнопки добавить
}

void Positions::onEdit() {
    //реализация кнопки править
}

bool Positions::onDelete() {
    //реализация кнопки удалить
    return false;
}

bool Positions::onSave() {
    //реализация кнопки сохранить
    return false;
}
