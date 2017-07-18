#include "commands.h"

Commands::Commands(QSqlDatabase db, QTreeWidget *navigatorTree,
                   QTableWidget *navigatorReciversTable, QTreeWidget *changesTree,
                   QTableWidget *changesReciversTable, QWidget *parent) :
    navigatorTree(navigatorTree),
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
    for (int i = 0; i < navigatorTree->columnCount(); i++)
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
    QString selectPattern = "SELECT t1.termname, inf.date_add, inf.order_id "
                "FROM orders_alerts.orders_alerts_info  inf "
                "JOIN reference_data.terms t1 ON inf.order_tid = t1.termhierarchy;";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    while (query.next()) {
        addCommand(root, query.value(0).toString(), query.value(1).toDateTime(),
                         query.value(1).toDateTime(),query.value(2).toString());
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

    navigatorReceiversTable->setColumnCount(3);
    QStringList LowerTableHeaders;
    LowerTableHeaders << "Получатель" << "Отметка" << "Время отметки";
    navigatorReceiversTable->setHorizontalHeaderLabels(LowerTableHeaders);
    navigatorReceiversTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    navigatorTree->hideColumn(5);
}

void Commands::fillChanges() {
}

QWidget *Commands::onAdd() {
    //реализация кнопки добавить
    CommandsAddForm *addWidget = new CommandsAddForm(OWN_NAME, db);
    addWidget->setSaveMode(0);
    addWidgetList << addWidget;
    return addWidget;
}

QWidget *Commands::onEdit() {
    //реализация кнопки править
    QStringList exceptions;
    exceptions << "Команды и сигналы" << "Документы" << "Исходящие" << "Входящие";
    QString s = navigatorTree->currentItem()->text(0);
    for (int i=0;i<exceptions.size();i++) {
        if (s.compare(exceptions.at(i)) == 0) {
            return false;
        }
    }
    QString code = navigatorTree->currentItem()->text(5);
    QString type = navigatorTree->currentItem()->parent()->parent()->text(0);
    CommandsAddForm *editWidget = new CommandsAddForm(OWN_NAME, db);
    editWidget->setSaveMode(1);
    addWidgetList << editWidget;
    if (type.compare("Документы") == 0) {
        editWidget->setDataDocument(code);
    }
    else {
        editWidget->setDataCommand(code);
    }
    //deleteCommand(code);
    return editWidget;
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
    QString type = navigatorTree->currentItem()->parent()->parent()->text(0);
    if (type.compare("Документы") == 0) {
        return deleteDocument(code);
    }
    else {
        return deleteCommand(code);
    }
}

bool Commands::onSave(int number) {
    //реализация кнопки сохранить
    number-=2;
    bool commandOrDoc = addWidgetList.at(number)->getCommandOrDoc();
    int saveMode = addWidgetList.at(number)->getSaveMode();
    if(!commandOrDoc) {
        if (saveMode == 0) {
            return saveCommand(OWN_NAME, addWidgetList.at(number)->getCommandtInformationBox());
        }
        else {
            return updateCommand(addWidgetList.at(number)->getCommandtInformationBox());
        }
    }
    else  {
        if (saveMode == 0) {
            return saveDocument(addWidgetList.at(number)->getDocumentInformationBox());
        }
        else return updateDocument(addWidgetList.at(number)->getDocumentInformationBox());
    }
    return false;
}

bool Commands::onSend()
{
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
        transmissionModule = new DataTransmissionModule(db);
        transmissionModule->sendDocument(this->navigatorTree->currentItem()->text(5));
        delete transmissionModule;
        return true;
    }
    else {
        transmissionModule = new DataTransmissionModule(db);
        transmissionModule->sendCommand(this->navigatorTree->currentItem()->text(5));
        delete transmissionModule;
        return true;
    }

}

void Commands::removeForm(int index)
{
    addWidgetList.removeAt(index - 2);
}

QString Commands::getCommandName()
{
    QStringList exceptions;
    exceptions << "Команды и сигналы" << "Документы" << "Исходящие" << "Входящие";
    QString s = navigatorTree->currentItem()->text(0);
    for (int i=0;i<exceptions.size();i++) {
        if (s.compare(exceptions.at(i)) == 0) {
            return false;
        }
    }
    QString code = navigatorTree->currentItem()->text(5);
    QSqlQuery query = QSqlQuery(db);
    QString selectPattern = "SELECT order_tid FROM orders_alerts.orders_alerts_info "
                "WHERE order_id ='"+code+"'; ";
    QString type=navigatorTree->currentItem()->parent()->parent()->text(0);
    if (type.compare("Документы") == 0) {
        selectPattern = "SELECT doctheme_tid FROM combatdocs.combatdocs_theme "
                        "WHERE cmbdid ='"+code+"'; ";
    }
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    QString commandName;
    while (query.next()) {
        commandName = query.value(0).toString();
    }
    return Utility::convertCodeToReferenceName(db,commandName);
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
    QString selectPattern = "SELECT combat_hierarchy, mark_tid, mark_time ";
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
    navigatorReceiversTable->clearContents();
    navigatorReceiversTable->setRowCount(query.size());
    while (query.next()) {
        navigatorReceiversTable->setItem(i, 0, new QTableWidgetItem(query.value(0).toString()));
        navigatorReceiversTable->setItem(i, 1, new QTableWidgetItem(query.value(1).toString()));
        navigatorReceiversTable->setItem(i, 2, new QTableWidgetItem(query.value(2).toString()));
        i++;
    }
}

bool Commands::saveCommand(QString object, CommandsMessageBox box)
{
    object=Utility::convertReferenceNameTOCode(db,object);
    QString command = box.getCommandName();
    command=Utility::convertReferenceNameTOCode(db,command);
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
        QString insertQuery = "INSERT INTO orders_alerts.orders_alerts_info( "
                              "combat_hierarchy, order_id, training_object, order_tid, date_add, date_edit, date_delete, id_manager) "
                              "VALUES ('"+object+"', '"
                                         +QString::number(id)+"', '"
                                         +"true"+"', '"
                                         +command+"', '"
                                         +box.getTimeAdd()+"', '"
                                         +box.getTimeAdd()+"', NULL, '"
                                         +QString::number(1)+"');";
        if (!query.exec(insertQuery)) {
            return false;
        }
        for (int i = 0; i < box.getParametrs().size(); i++) {
            insertQuery = "INSERT INTO orders_alerts.orders_alerts_param( "
                          "order_id, param_tid, param_value) "
                          "VALUES ('"+QString::number(id)+"', '"
                                     +Utility::convertReferenceNameTOCode(db,box.getParametrs().at(i))+"', '"
                                     +box.getParametrsValue().at(i)+"');";
            if (!query.exec(insertQuery)) {
                return false;
            }
        }
        for (int i = 0; i < box.getReceivers().size(); i++) {
            insertQuery = "INSERT INTO orders_alerts.orders_alerts_acceptors( "
                          "order_id, combat_hierarchy, mark_tid, mark_time, tid, date_add, date_edit, date_delete, id_manager) "
                          "VALUES ('"+QString::number(id)+"', '"
                                     +box.getReceivers().at(i)+"', '"
                                     +Utility::convertReferenceNameTOCode(db,box.getReceiversMarks().at(i))+"', '"
                                     +box.getReceiversTime().at(i)+"', '"
                                     +"1"+"', '"
                                     +QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"', '"
                                     +QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"', NULL, '"
                                     +"1"+"');";
            if (!query.exec(insertQuery)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool Commands::saveDocument(DocMessageBox box)
{
    QString document = box.getDocNumber();
    QSqlQuery query = QSqlQuery( db );
    QString insertQuery = "INSERT INTO combatdocs.combatdocs_info( "
                          "cmbdid, outgoing_reg_number, outgoing_reg_datetime, holder_coid, date_add, date_edit, date_delete, id_manager) "
                          "VALUES ('"+document+"', '"
                                     +document+"', '"
                                     +box.getTimeRegister()+"', '"
                                     +QString::number(1)+"', '"
                                     +box.getTimeAdd()+"', '"
                                     +box.getTimeAdd()+"', NULL, '"
                                     +QString::number(1)+"');";
    if (!query.exec(insertQuery)) {
        return false;
    }
    query = QSqlQuery( db );
    insertQuery = "INSERT INTO combatdocs.combatdocs_theme( "
                          "cmbdid, doctheme_tid, date_add, date_edit, date_delete, tid, id_manager) "
                          "VALUES ('"+document+"', '"
                                     +Utility::convertReferenceNameTOCode(db,box.getDocTheme())+"', '"
                                     +box.getTimeAdd()+"', '"
                                     +box.getTimeAdd()+"', NULL, '"
                                     +"1"+"', '"
                                     +QString::number(1)+"');";
    if (!query.exec(insertQuery)) {
        return false;
    }
    query = QSqlQuery( db );
    insertQuery = "INSERT INTO combatdocs.combatdocs_type( "
                          "cmbdid, doctype_tid, tid, date_add, date_edit, date_delete, id_manager) "
                          "VALUES ('"+document+"', '"
                                     +Utility::convertReferenceNameTOCode(db,box.getDocType())+"', '"
                                     +"1"+"', '"
                                     +box.getTimeAdd()+"', '"
                                     +box.getTimeAdd()+"', NULL, '"
                                     +QString::number(1)+"');";
    if (!query.exec(insertQuery)) {
        return false;
    }
    for (int i = 0; i < box.getReceivers().size(); i++) {
        insertQuery = "INSERT INTO combatdocs.combatdocs_acceptors( "
                      "cmbdid, combat_hierarchy, mark_tid, mark_time, holder_coid, tid, date_add, date_edit, date_delete, id_manager) "
                      "VALUES ('"+document+"', '"
                                 +box.getReceivers().at(i)+"', '"
                                 +Utility::convertReferenceNameTOCode(db,box.getReceiversMarks().at(i))+"', '"
                                 +box.getReceiversTime().at(i)+"', '"
                                 +"1"+"', '"
                                 +"1"+"', '"
                                 +QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"', '"
                                 +QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"', NULL, '"
                                 +"1"+"');";
        if (!query.exec(insertQuery)) {
            return false;
        }
    }
    return true;
}

bool Commands::deleteCommand(QString id)
{
    QSqlQuery query = QSqlQuery( db );
    QString s = "";
    s = "DELETE FROM orders_alerts.orders_alerts_info WHERE order_id ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    s = "DELETE FROM orders_alerts.orders_alerts_acceptors WHERE order_id ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    s = "DELETE FROM orders_alerts.orders_alerts_param WHERE order_id ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    return true;
}

bool Commands::deleteDocument(QString id)
{
    QSqlQuery query = QSqlQuery( db );
    QString s = "";
    s = "DELETE FROM combatdocs.combatdocs_info WHERE cmbdid ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    query = QSqlQuery( db );
    s = "";
    s = "DELETE FROM combatdocs.combatdocs_theme WHERE cmbdid ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    query = QSqlQuery( db );
    s = "";
    s = "DELETE FROM combatdocs.combatdocs_type WHERE cmbdid ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    query = QSqlQuery( db );
    s = "";
    s = "DELETE FROM combatdocs.combatdocs_acceptors WHERE cmbdid ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    return true;
}

bool Commands::updateDocument(DocMessageBox box) {
    QString document = box.getDocNumber();
    QString id = box.getDocNumber();

    QSqlQuery query = QSqlQuery( db );
    QString insertQuery = "UPDATE combatdocs.combatdocs_info "
                          "SET outgoing_reg_datetime='"
                          +box.getTimeRegister()+"', date_edit='"
                          +box.getTimeAdd()+"' "
                          "WHERE cmbdid='"+id+"';";
    if (!query.exec(insertQuery)) {
        return false;
    }
    query = QSqlQuery( db );
    QString s = "";
    s = "DELETE FROM combatdocs.combatdocs_theme WHERE cmbdid ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    query = QSqlQuery( db );
    s = "";
    s = "DELETE FROM combatdocs.combatdocs_type WHERE cmbdid ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    query = QSqlQuery( db );
    s = "";
    s = "DELETE FROM combatdocs.combatdocs_acceptors WHERE cmbdid ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    query = QSqlQuery( db );
    insertQuery = "INSERT INTO combatdocs.combatdocs_theme( "
                          "cmbdid, doctheme_tid, date_add, date_edit, date_delete, tid, id_manager) "
                          "VALUES ('"+document+"', '"
                                     +Utility::convertReferenceNameTOCode(db,box.getDocTheme())+"', '"
                                     +box.getTimeAdd()+"', '"
                                     +box.getTimeAdd()+"', NULL, '"
                                     +"1"+"', '"
                                     +QString::number(1)+"');";
    if (!query.exec(insertQuery)) {
        return false;
    }
    query = QSqlQuery( db );
    insertQuery = "INSERT INTO combatdocs.combatdocs_type( "
                          "cmbdid, doctype_tid, tid, date_add, date_edit, date_delete, id_manager) "
                          "VALUES ('"+document+"', '"
                                     +Utility::convertReferenceNameTOCode(db,box.getDocType())+"', '"
                                     +"1"+"', '"
                                     +box.getTimeAdd()+"', '"
                                     +box.getTimeAdd()+"', NULL, '"
                                     +QString::number(1)+"');";
    if (!query.exec(insertQuery)) {
        return false;
    }
    for (int i = 0; i < box.getReceivers().size(); i++) {
        insertQuery = "INSERT INTO combatdocs.combatdocs_acceptors( "
                      "cmbdid, combat_hierarchy, mark_tid, mark_time, holder_coid, tid, date_add, date_edit, date_delete, id_manager) "
                      "VALUES ('"+document+"', '"
                                 +box.getReceivers().at(i)+"', '"
                                 +Utility::convertReferenceNameTOCode(db,box.getReceiversMarks().at(i))+"', '"
                                 +box.getReceiversTime().at(i)+"', '"
                                 +"1"+"', '"
                                 +"1"+"', '"
                                 +QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"', '"
                                 +QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"', NULL, '"
                                 +"1"+"');";
        if (!query.exec(insertQuery)) {
            return false;
        }
    }
    return true;
}

bool Commands::updateCommand(CommandsMessageBox box) {
    QSqlQuery query = QSqlQuery( db );
    QString s = "";
    QString command = box.getCommandName();
    QString id = box.getIdCommand();
    command=Utility::convertReferenceNameTOCode(db,command);
    QString insertQuery = "UPDATE orders_alerts.orders_alerts_info "
                          "SET order_tid='"
                          +command+"', date_edit='"
                          +box.getTimeAdd()+"' "
                          "WHERE order_id ='"+id+"';";
    qDebug() << insertQuery;
    if (!query.exec(insertQuery)) {
        return false;
    }
    s = "DELETE FROM orders_alerts.orders_alerts_acceptors WHERE order_id ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    s = "DELETE FROM orders_alerts.orders_alerts_param WHERE order_id ='"+id+"';";
    if (!query.exec(s)) {
        return false;
    }
    for (int i = 0; i < box.getParametrs().size(); i++) {
        insertQuery = "INSERT INTO orders_alerts.orders_alerts_param( "
                      "order_id, param_tid, param_value) "
                      "VALUES ('"+id+"', '"
                                 +Utility::convertReferenceNameTOCode(db,box.getParametrs().at(i))+"', '"
                                 +box.getParametrsValue().at(i)+"');";
        if (!query.exec(insertQuery)) {
            return false;
        }
    }
    for (int i = 0; i < box.getReceivers().size(); i++) {
        insertQuery = "INSERT INTO orders_alerts.orders_alerts_acceptors( "
                      "order_id, combat_hierarchy, mark_tid, mark_time, tid, date_add, date_edit, date_delete, id_manager) "
                      "VALUES ('"+id+"', '"
                                 +box.getReceivers().at(i)+"', '"
                                 +Utility::convertReferenceNameTOCode(db,box.getReceiversMarks().at(i))+"', '"
                                 +box.getReceiversTime().at(i)+"', '"
                                 +"1"+"', '"
                                 +QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"', '"
                                 +QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+"', NULL, '"
                                 +"1"+"');";
        if (!query.exec(insertQuery)) {
            return false;
        }
    }
    return true;
}
