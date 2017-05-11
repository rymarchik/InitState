#include "commands.h"

Commands::Commands(QSqlDatabase db, QTreeWidget *navigatorTree,
                   QTableWidget *navigatorReciversTable, QTreeWidget *changesTree,
                   QTableWidget *changesReciversTable, QWidget *parent)
    : navigatorTree(navigatorTree),
      changesTree(changesTree),
      BaseToolClass(db, navigatorReciversTable, changesReciversTable, parent)
{
    connect(this->navigatorTree, SIGNAL(itemSelectionChanged()), this, SLOT(showRecivers()));
}

void Commands::fillNavigator() {
    navigatorTree->clear();
    changesTree->clear();
    navigatorTree->clear();
    navigatorTree->setColumnCount(5);
    QStringList UpperTableHeaders;
    UpperTableHeaders << "Название команды/сигнала/\nРегистрационный номер" << "Время формирования/\nДата регистрации"
                        << "Время исполнения/\nТип документа" << "Тема документа" << "Источник информации"<<"код";
    navigatorTree->setHeaderLabels(UpperTableHeaders);
    for (int i=0; i < navigatorTree->columnCount(); i++)
    {
        navigatorTree->resizeColumnToContents(i);
    }
    QTreeWidgetItem *commandsSignals = new QTreeWidgetItem(navigatorTree);
    commandsSignals->setText(0, "Команды и сигналы");
    commandsSignals->setExpanded(true);
    QTreeWidgetItem *documents = new QTreeWidgetItem(navigatorTree);
    documents->setText(0, "Документы");
    documents->setExpanded(true);
    QTreeWidgetItem *out = new QTreeWidgetItem(commandsSignals);
    out->setText(0, "Исходящие");
    QTreeWidgetItem *in = new QTreeWidgetItem(commandsSignals);
    in->setText(0, "Входящие");
    QTreeWidgetItem* root = out;
    QSqlQuery query = QSqlQuery(db);
    QString selectPattern = "SELECT t1.termname, inf.date_add, atr.execution_time, inf.order_id "
            "FROM orders_alerts.orders_alerts_info  inf "
            "JOIN orders_alerts.orders_alerts_attrib atr ON inf.order_id = atr.order_id "
            "JOIN reference_data.terms t1 ON inf.order_tid = t1.termhierarchy;";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    while (query.next()) {
        addCommand(root, query.value(0).toString(), query.value(1).toDateTime(),
                         query.value(2).toDateTime(),query.value(3).toString());
    }
    root->setExpanded(true);
    out = new QTreeWidgetItem(documents);
    out->setText(0, "Исходящие");
    in = new QTreeWidgetItem(documents);
    in->setText(0, "Входящие");
    root = out;
    selectPattern = "SELECT cinf.outgoing_reg_number, cinf.outgoing_reg_datetime, t1.termname, t2.termname, cinf.cmbdid "
            "FROM combatdocs.combatdocs_info cinf "
              "JOIN combatdocs.combatdocs_type ctyp ON cinf.cmbdid = ctyp.cmbdid "
              "JOIN combatdocs.combatdocs_theme cthm ON cinf.cmbdid = cthm.cmbdid "
              "JOIN reference_data.terms t1 ON ctyp.doctype_tid = t1.termhierarchy "
              "JOIN reference_data.terms t2 ON cthm.doctheme_tid = t2.termhierarchy;";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    while (query.next()) {
        addDocument(root, query.value(0).toString(), query.value(1).toDateTime(),
                    query.value(2).toString(), query.value(3).toString(), query.value(4).toString());
    }
    root->setExpanded(true);

    navigatorReciversTable->setColumnCount(3);
    QStringList LowerTableHeaders;
    LowerTableHeaders << "Получатель" << "Отметка" << "Время отметки";
    navigatorReciversTable->setHorizontalHeaderLabels(LowerTableHeaders);
    navigatorReciversTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    navigatorTree->hideColumn(5);
}

void Commands::fillChanges() {
    /*changesTree->clear();
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
    changesLowerTable->setColumnCount(3);
    QStringList LowerTableHeaders;
    LowerTableHeaders << "Получатель2" << "Отметка2" << "Время отметки2";
    changesLowerTable->setHorizontalHeaderLabels(LowerTableHeaders);
    changesLowerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);*/
}

QWidget *Commands::onAdd() {
    //реализация кнопки добавить
    CommandsAddForm *addWidget = new CommandsAddForm(OWN_NAME);
    addWidgetList << addWidget;
    return addWidget;
}

QWidget *Commands::onEdit() {
    //реализация кнопки править
    return 0;
}

bool Commands::onDelete() {
    //реализация кнопки удалить
    QStringList exceptions;
    exceptions << "Команды и сигналы" << "Документы" << "Исходящие" << "Входящие";
    QString s = navigatorTree->currentItem()->text(0);
    for (int i=0;i<exceptions.size();i++) {
        if (s.compare(exceptions.at(i)) == 0) {
            return false;
        }
    }
    QString code = navigatorTree->currentItem()->text(5);
    QString type=navigatorTree->currentItem()->parent()->parent()->text(0);
    if (type.compare("Документы") == 0) {
        return deleteDocument(code);
    }
    else {
        return deleteCommand(code);
    }
}

bool Commands::onSave(int number) {
    //реализация кнопки сохранить
    bool commandOrDoc = addWidgetList.at(number)->getCommandOrDoc();
    QString command = addWidgetList.at(number)->getCommandName();
    QString timeAdd = addWidgetList.at(number)->getTimeAdd();
    QString timeExec = addWidgetList.at(number)->getTimeExec();
    QString AttributeExec = addWidgetList.at(number)->getAttributeExec();
    QStringList parametrs = addWidgetList.at(number)->getParametrList();
    QStringList receivers = addWidgetList.at(number)->getReceiversList();
    if(!commandOrDoc) {
        return saveCommand(OWN_NAME, command, timeAdd);
    }
    //else saveDoc();
    return false;
}

void Commands::addCommand(QTreeWidgetItem *parent,
                  QString name1, QDateTime date, QDateTime date2, QString code)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name1);
    treeItem->setText(1, date.toString("dd.MM.yyyy hh:mm:ss"));
    treeItem->setText(2, date2.toString("dd.MM.yyyy hh:mm:ss"));
    treeItem->setText(5, code);

    parent->addChild(treeItem);
}

void Commands::addDocument(QTreeWidgetItem *parent,
                  QString name1, QDateTime date, QString name3, QString name4, QString code)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name1);
    treeItem->setText(1, date.toString("dd.MM.yyyy hh:mm:ss"));
    treeItem->setText(2, name3);
    treeItem->setText(3, name4);
    treeItem->setText(5, code);

    parent->addChild(treeItem);
}

QString Commands::convertCodeToReferenceName(QString code)
{
    QSqlQuery query = QSqlQuery( db );
    QString s = "";
    QString referenceName;
    s = "SELECT reference_data.terms.termname FROM reference_data.terms WHERE reference_data.terms.termhierarchy ='"+code+"';";
    if (!query.exec(s)) {
        return "";
    }
    else {
            while ( query.next() ) {
                referenceName = query.value( 0 ).toString();
            }
    }
    return referenceName;
}

QString Commands::convertReferenceNameTOCode(QString referenceName)
{
    QSqlQuery query = QSqlQuery( db );
    QString s = "";
    QString code;
    s = "SELECT reference_data.terms.termhierarchy FROM reference_data.terms WHERE reference_data.terms.termname ='"+referenceName+"';";
    if (!query.exec(s)) {
        return "";
    }
    else {
            while ( query.next() ) {
                code = query.value( 0 ).toString();
            }
    }
    return code;
}

void Commands::showRecivers()
{
    QStringList exceptions;
    exceptions << "Команды и сигналы" << "Документы" << "Исходящие" << "Входящие";
    QString s = navigatorTree->currentItem()->text(0);
    for (int i=0;i<exceptions.size();i++) {
        if (s.compare(exceptions.at(i)) == 0) {
            return;
        }
    }
    QString code = navigatorTree->currentItem()->text(5);
    QString type=navigatorTree->currentItem()->parent()->parent()->text(0);
    QSqlQuery query = QSqlQuery(db);
    QString selectPattern = "SELECT combatobjectcode, mark_tid, mark_time ";
    if (type.compare("Документы") == 0) {
        selectPattern = selectPattern+ "FROM combatdocs.combatdocs_acceptors WHERE cmbdid='"+code+"';";
    }
    else {
        selectPattern = selectPattern+ "FROM orders_alerts.orders_alerts_acceptors WHERE order_id='"+code+"';";
    }
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    int i=0;
    navigatorReciversTable->clearContents();
    navigatorReciversTable->setRowCount(query.size());
    while (query.next()) {
        navigatorReciversTable->setItem(i, 0, new QTableWidgetItem(query.value(0).toString()));
        navigatorReciversTable->setItem(i, 1, new QTableWidgetItem(convertCodeToReferenceName(query.value(1).toString())));
        navigatorReciversTable->setItem(i, 2, new QTableWidgetItem(query.value(2).toString()));
        i++;
    }
}

bool Commands::saveCommand(QString object, QString command, QString time)
{
    object=convertReferenceNameTOCode(object);
    command=convertReferenceNameTOCode(command);
    QSqlQuery query = QSqlQuery( db );
    if (!query.exec("SELECT order_id FROM orders_alerts.orders_alerts_info ORDER BY order_id DESC LIMIT 1;")) {
        return false;
    }
    else {
        int id;
        while ( query.next() ) {
            id = query.value( 0 ).toInt();
        }
        id++;
        db.transaction();
        query.prepare( "INSERT INTO orders_alerts.orders_alerts_info( "
                       "combatobjectcode, order_id, training_object, order_tid, date_add, date_edit, date_delete, id_manager) "
                       "VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
        query.addBindValue( object );
        query.addBindValue( id );
        query.addBindValue( "true" );
        query.addBindValue( command );
        query.addBindValue(time);
        query.addBindValue(time);
        query.addBindValue(time/*"null"*/);
        query.addBindValue("1");
        query.exec();
        return db.commit();
    }
    return false;
}

bool Commands::deleteCommand(QString id)
{
    QSqlQuery query = QSqlQuery( db );
    QString s = "";
    s = "DELETE FROM orders_alerts.orders_alerts_info WHERE order_id ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    else {
        return true;
    }
}

bool Commands::deleteDocument(QString id)
{
    QSqlQuery query = QSqlQuery( db );
    QString s = "";
    s = "DELETE FROM combatdocs.combatdocs_info WHERE cmbdid ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    else {
        return true;
    }
}


/*QStringList HitTargetsTabForm::getDataSourceBatteries() {
    QSqlQuery query;
    QString selectQuery = "SELECT object_number, termname FROM own_forces.combatstructure JOIN "
                          "reference_data.terms ON termhierarchy = object_name WHERE "
                          "combat_hierarchy IN (SELECT combat_hierarchy FROM own_forces.combatstructure "
                          "WHERE nlevel(combat_hierarchy) = 1 AND type_army = '22.10' AND type_mode = 0 "
                          "AND date_delete is null) ORDER BY object_number";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(tr("%1").arg(query.value(0).toInt()) + " " + query.value(1).toString());
    }
    return list;
}*/
