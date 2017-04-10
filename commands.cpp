#include "commands.h"

Commands::Commands(QSqlDatabase db, QWidget *parent) : QToolBox(parent)
{
    this->db=db;
}

void Commands::setTables(QTreeWidget *navigatorUpperTree, QTableWidget *navigatorLowerTable)
{
    this->navigatorUpperTree=navigatorUpperTree;
    this->navigatorLowerTable=navigatorLowerTable;
    connect(this->navigatorUpperTree, SIGNAL(itemSelectionChanged()), this, SLOT(showRecivers()));
}

void Commands::fillNavigator() {
    navigatorUpperTree->clear();
    navigatorLowerTable->clear();
    navigatorUpperTree->clear();

    navigatorUpperTree->setColumnCount(5);
    QStringList UpperTableHeaders;
    UpperTableHeaders << "Название команды/сигнала/\nРегистрационный номер" << "Время формирования/\nДата регистрации"
                        << "Время исполнения/\nТип документа" << "Тема документа" << "Источник информации";
    navigatorUpperTree->setHeaderLabels(UpperTableHeaders);
    for (int i=0; i < navigatorUpperTree->columnCount(); i++)
    {
        navigatorUpperTree->resizeColumnToContents(i);
    }
    QTreeWidgetItem *commandsSignals = new QTreeWidgetItem(navigatorUpperTree);
    commandsSignals->setText(0, "Команды и сигналы");
    commandsSignals->setExpanded(true);
    QTreeWidgetItem *documents = new QTreeWidgetItem(navigatorUpperTree);
    documents->setText(0, "Документы");
    documents->setExpanded(true);
    QTreeWidgetItem *out = new QTreeWidgetItem(commandsSignals);
    out->setText(0, "Исходящее");
    QTreeWidgetItem *in = new QTreeWidgetItem(commandsSignals);
    in->setText(0, "Входящее");
    QTreeWidgetItem* root = out;
    QSqlQuery query = QSqlQuery(db);
    QString selectPattern = "SELECT t1.termname, inf.date_add, atr.execution_time, inf.order_id "
            "FROM orders_alerts.orders_alerts_info  inf "
            "JOIN orders_alerts.orders_alerts_attrib atr ON inf.order_id = atr.order_id "
            "JOIN reference_data.terms t1 ON inf.order_tid = t1.termhierarchy;";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    int i = 0;
    signalsList = new int[query.size()];
    while (query.next()) {
        addCommand(root, query.value(0).toString(), query.value(1).toDateTime(), query.value(2).toDateTime());
        signalsList[i] = query.value(3).toInt();
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
        addDocument(root, query.value(0).toString(), query.value(1).toDateTime(),
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
    /*Utility::closeNewEditTab(this);
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
    }
    changesLowerTable->setColumnCount(3);
    QStringList LowerTableHeaders;
    LowerTableHeaders << "Получатель2" << "Отметка2" << "Время отметки2";
    changesLowerTable->setHorizontalHeaderLabels(LowerTableHeaders);
    changesLowerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);*/
}

void Commands::onAdd() {
    //реализация кнопки добавить
    /*CommandsAddForm *newForm = new CommandsAddForm;
    contentWidget = newForm;
    this->addTab(contentWidget, "Новый");
    this->setCurrentWidget(contentWidget);*/
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

void Commands::addCommand(QTreeWidgetItem *parent,
                  QString name1, QDateTime date, QDateTime date2)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name1);
    treeItem->setText(1, date.toString("dd.MM.yyyy hh:mm:ss"));
    treeItem->setText(2, date2.toString("dd.MM.yyyy hh:mm:ss"));

    parent->addChild(treeItem);
}

void Commands::addDocument(QTreeWidgetItem *parent,
                  QString name1, QDateTime date, QString name3, QString name4)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name1);
    treeItem->setText(1, date.toString("dd.MM.yyyy hh:mm:ss"));
    treeItem->setText(2, name3);
    treeItem->setText(3, name4);

    parent->addChild(treeItem);
}

void Commands::showRecivers()
{
    QString s = navigatorUpperTree->currentItem()->text(0);
    if (s.compare("Команды и сигналы") == 0) { return; }
    if (s.compare("Документы") == 0) { return; }
    QTreeWidgetItem *parent = navigatorUpperTree->currentItem()->parent();
    int numb = parent->indexOfChild(navigatorUpperTree->currentItem());
    QSqlQuery query = QSqlQuery(db);
    QString selectPattern = "SELECT combatobjectcode, mark_tid, mark_time ";
    selectPattern = selectPattern+ "FROM orders_alerts.orders_alerts_acceptors WHERE order_id='"+QString::number(signalsList[numb],10)+"';";
    //qDebug()<<selectPattern;
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    int i=0;
    navigatorLowerTable->clearContents();
    navigatorLowerTable->setRowCount(query.size());
    while (query.next()) {
        navigatorLowerTable->setItem(i, 0, new QTableWidgetItem(query.value(0).toString()));
        navigatorLowerTable->setItem(i, 1, new QTableWidgetItem(query.value(1).toString()));
        navigatorLowerTable->setItem(i, 2, new QTableWidgetItem(query.value(2).toString()));
        i++;
    }
}
