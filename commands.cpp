#include "commands.h"
#include "utility.h"

Commands::Commands(QWidget* parent) : BaseTabWidget(parent)
{
    navigatorTree = new QTreeWidget();
    changesTree = new QTreeWidget();

    navigatorTabLayout = new QVBoxLayout;
    navigatorTabLayout->addWidget(navigatorTree);
    navigatorTabLayout->addWidget(navigatorLowerTable);

    navigatorTabWidget = new QWidget;
    navigatorTabWidget->setLayout(navigatorTabLayout);

    changesTabLayout = new QVBoxLayout;
    changesTabLayout->addWidget(changesTree);
    changesTabLayout->addWidget(changesLowerTable);

    changesTabWidget = new QWidget;
    changesTabWidget->setLayout(changesTabLayout);
}

void Commands::fillNavigator() {
    Utility::closeNewEditTab(this);
    navigatorTree->clear();
    navigatorLowerTable->clear();
    navigatorTree->clear();

    navigatorTree->setColumnCount(5);
    QStringList UpperTableHeaders;
    UpperTableHeaders << "Название команды/сигнала/\nРегистрационный номер" << "Время формирования/\nДата регистрации"
                        << "Время исполнения/\nТип документа" << "Тема документа" << "Источник информации";
    navigatorTree->setHeaderLabels(UpperTableHeaders);
    for (int i=0; i < navigatorTree->columnCount(); i++)
    {
        navigatorTree->resizeColumnToContents(i);
    }
    QTreeWidgetItem *commandsSignals = new QTreeWidgetItem(navigatorTree);
    commandsSignals->setText(0, "Команды и сигналы");
    QTreeWidgetItem *documents = new QTreeWidgetItem(navigatorTree);
    documents->setText(0, "Документы");
    QTreeWidgetItem *out = new QTreeWidgetItem(commandsSignals);
    out->setText(0, "Исходящее");
    QTreeWidgetItem *in = new QTreeWidgetItem(commandsSignals);
    in->setText(0, "Входящее");
    QTreeWidgetItem* root = out;
    QSqlQuery query;
    QString selectPattern = "SELECT t1.termname, inf.date_add, atr.execution_time "
            "FROM orders_alerts.orders_alerts_info  inf "
            "JOIN orders_alerts.orders_alerts_attrib atr ON inf.order_id = atr.order_id "
            "JOIN reference_data.terms t1 ON inf.order_tid = t1.termhierarchy;";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    int i = 0;
    while (query.next()) {
        makeNoteCommand(root, query.value(0).toString(), query.value(1).toDateTime(), query.value(2).toDateTime());
        i++;
    }
    root->setExpanded(true);
    out = new QTreeWidgetItem(documents);
    out->setText(0, "Исходящее");
    in = new QTreeWidgetItem(documents);
    in->setText(0, "Входящее");
    root = out;
    selectPattern = "SELECT cinf.outgoing_reg_number, cinf.outgoing_reg_datetime, t1.termname, t2.termname "
            "FROM combatdocs.combatdocs_info cinf "
              "JOIN combatdocs.combatdocs_type ctyp ON cinf.cmbdid = ctyp.cmbdid "
              "JOIN combatdocs.combatdocs_theme cthm ON cinf.cmbdid = cthm.cmbdid "
              "JOIN reference_data.terms t1 ON ctyp.doctype_tid = t1.termhierarchy "
              "JOIN reference_data.terms t2 ON cthm.doctheme_tid = t2.termhierarchy;";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    i = 0;
    while (query.next()) {
        makeNoteDoc(root, query.value(0).toString(), query.value(1).toDateTime(),
                    query.value(2).toString(), query.value(3).toString());
        i++;
    }
    root->setExpanded(true);

    navigatorLowerTable->setColumnCount(3);
    QStringList LowerTableHeaders;
    LowerTableHeaders << "Получатель" << "Отметка" << "Время отметки";
    navigatorLowerTable->setHorizontalHeaderLabels(LowerTableHeaders);
    navigatorLowerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void Commands::fillChanges() {
    Utility::closeNewEditTab(this);
    changesTree->clear();
    navigatorLowerTable->clear();

    changesTree->setColumnCount(7);
    QStringList UpperTableHeaders;
    UpperTableHeaders << "Команда/сигнал/\nРегистрационный номер" << "Время исполнения\nДата регистрации"
                      << "Источник информации" << "Тип" << "Тема" << "Событие" << "Время события";
    changesTree->setHeaderLabels(UpperTableHeaders);
    for (int i=0; i < changesTree->columnCount(); i++)
    {
        changesTree->resizeColumnToContents(i);
    }
    QTreeWidgetItem *root = changesTree->topLevelItem(0);
    /*QSqlQuery query;
    QString selectPattern = "SELECT orders_alerts.orders_alerts_info.order_tid, "
            "orders_alerts.orders_alerts_info.date_add, orders_alerts.orders_alerts_attrib.execution_time "
            "FROM orders_alerts.orders_alerts_info JOIN orders_alerts.orders_alerts_attrib "
            "ON orders_alerts.orders_alerts_info.order_id = orders_alerts.orders_alerts_attrib.order_id;";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    int i = 0;
    while (query.next()) {
        makeNote(root, query.value(0).toString(), query.value(1).toString(), query.value(2).toString());
        i++;
    }*/
    changesLowerTable->setColumnCount(3);
    QStringList LowerTableHeaders;
    LowerTableHeaders << "Получатель2" << "Отметка2" << "Время отметки2";
    changesLowerTable->setHorizontalHeaderLabels(LowerTableHeaders);
    changesLowerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void Commands::onAdd() {
    //реализация кнопки добавить
}

void Commands::onEdit() {
    //реализация кнопки править
}

bool Commands::onDelete() {
    //реализация кнопки удалить
    return false;
}

bool Commands::onSave() {
    //реализация кнопки сохранить
    return false;
}

void Commands::makeNoteCommand(QTreeWidgetItem *parent,
                  QString name1, QDateTime date, QDateTime date2)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name1);
    treeItem->setText(1, date.toString("dd.MM.yyyy hh:mm:ss"));
    treeItem->setText(2, date2.toString("dd.MM.yyyy hh:mm:ss"));

    parent->addChild(treeItem);
}

void Commands::makeNoteDoc(QTreeWidgetItem *parent,
                  QString name1, QDateTime date, QString name3, QString name4)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name1);
    treeItem->setText(1, date.toString("dd.MM.yyyy hh:mm:ss"));
    treeItem->setText(2, name3);
    treeItem->setText(3, name4);

    parent->addChild(treeItem);
}
