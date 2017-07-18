#include "commandsaddform.h"
#include "ui_commandsaddform.h"

CommandsAddForm::CommandsAddForm(QString ownName, QSqlDatabase db, QWidget *parent) :
    db(db),
    QWidget(parent),
    ui(new Ui::CommandsAddForm)
{
    ui->setupUi(this);
    ui->dataSourceLine->setText(ownName);
    ui->docSourceLine->setText(ownName);
    ui->timeCreateDTE->setDateTime(QDateTime::currentDateTime());
    ui->timeExecDTE->setDateTime(QDateTime::currentDateTime());
    ui->docDateRegisterTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->stackedWidget->setCurrentIndex(0);
    ui->tableWidget_4->setRowCount(0);
    setCommandsSignals();
    setAttributeExecution();
    setDocumentsTheme();
    setDocumentsType();
    setDocNumber();

    addParamForm= new AddParamForm();
    addReciversForm = new AddReciversForm();

    connect(ui->timeExecBox, SIGNAL(clicked(bool)), this, SLOT(changeEnabledTimerExec()));
    connect(ui->menuBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeContent()));
    connect(ui->addParamBut, SIGNAL(clicked()), addParamForm, SLOT(show()));
    connect(addParamForm, SIGNAL(sendData(QString, QString)), this, SLOT(receiveDataParametrs(QString, QString)));
    connect(ui->addReceiverBut, SIGNAL(clicked()), addReciversForm, SLOT(show()));
    connect(ui->addDocReceivers, SIGNAL(clicked()), addReciversForm, SLOT(show()));
    connect(addReciversForm, SIGNAL(sendData(QStringList, QString)), this, SLOT(receiveDataReceivers(QStringList, QString)));
}

CommandsAddForm::~CommandsAddForm()
{
    delete ui;
}

bool CommandsAddForm::getCommandOrDoc()
{
    if (ui->menuBox->currentIndex() == 0) return false;
    else return true;
}

CommandsMessageBox CommandsAddForm::getCommandtInformationBox()
{
    CommandsMessageBox box;
    box.setCommandName(ui->commandsSignalsBox->currentText());
    box.setTimeAdd(ui->timeCreateDTE->text());
    if (ui->timeExecBox->isChecked()) {
        box.setTimeExec(ui->timeExecDTE->text());
    }
    else {
        box.setTimeExec("0");
    }
    box.setAttributeExec(ui->attrExecBox->currentText());
    QStringList paramList;
    QStringList paramValueList;
    for (int i=0; i<ui->tableWidget_4->rowCount(); i++) {
        paramList << ui->tableWidget_4->item(i,0)->text();
        paramValueList << ui->tableWidget_4->item(i,1)->text();
    }
    box.setParametrs(paramList);
    box.setParametrsValue(paramValueList);
    QStringList receiversList;
    QStringList receiversMarkList;
    QStringList receiversTimeList;
    for (int i=0; i<ui->tableWidget_5->rowCount(); i++) {
        receiversList << ui->tableWidget_5->item(i,0)->text();
        receiversMarkList << ui->tableWidget_5->item(i,1)->text();
        receiversTimeList << ui->tableWidget_5->item(i,2)->text();
    }
    box.setReceivers(receiversList);
    box.setReceiversMarks(receiversMarkList);
    box.setReceiversTime(receiversTimeList);
    box.setIdCommand(idCommand);
    return box;
}

DocMessageBox CommandsAddForm::getDocumentInformationBox()
{
    DocMessageBox box;
    box.setDocNumber(ui->docNumberlineEdit->text());
    box.setTimeRegister(ui->docDateRegisterTimeEdit->text());
    box.setDocTheme(ui->docThemeBox->currentText());
    box.setDocType(ui->docTypeBox->currentText());
    box.setTimeAdd(ui->docDateRegisterTimeEdit->text());
    QStringList receiversList;
    QStringList receiversMarkList;
    QStringList receiversTimeList;
    for (int i=0; i<ui->tableWidget_6->rowCount(); i++) {
        receiversList << ui->tableWidget_6->item(i,0)->text();
        receiversMarkList << ui->tableWidget_6->item(i,1)->text();
        receiversTimeList << ui->tableWidget_6->item(i,2)->text();
    }
    box.setReceivers(receiversList);
    box.setReceiversMarks(receiversMarkList);
    box.setReceiversTime(receiversTimeList);
    return box;
}

void CommandsAddForm::setDataCommand(QString code)
{
    idCommand = code;
    QSqlQuery query = QSqlQuery(db);
    QString selectPattern = "SELECT order_id, order_tid, date_add "
                "FROM orders_alerts.orders_alerts_info WHERE order_id ='"+code+"'; ";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    while (query.next()) {
        ui->timeCreateDTE->setDateTime(query.value(2).toDateTime());
        ui->timeExecDTE->setDateTime(query.value(2).toDateTime());
    }
    ui->stackedWidget->setCurrentIndex(0);
    ui->menuBox->setCurrentIndex(0);
    ui->menuBox->setEnabled(false);
    ui->tableWidget_4->setRowCount(0);

    query = QSqlQuery(db);
    selectPattern = "SELECT param_tid, param_value "
                "FROM orders_alerts.orders_alerts_param WHERE order_id ='"+code+"';";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    ui->tableWidget_4->setRowCount(0);
    int n = query.size();
    int i=0;
    while (query.next()) {
        ui->tableWidget_4->insertRow(i);
        ui->tableWidget_4->setItem(i, 0, new QTableWidgetItem(Utility::convertCodeToReferenceName(db,query.value(0).toString())));
        ui->tableWidget_4->setItem(i, 1, new QTableWidgetItem(query.value(1).toString()));
        i++;
    }
    query = QSqlQuery(db);
    selectPattern = "SELECT combat_hierarchy, mark_tid, mark_time "
                "FROM orders_alerts.orders_alerts_acceptors WHERE order_id ='"+code+"';";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    ui->tableWidget_5->setRowCount(0);
    n = query.size();
    for (int i=0; i < n; i++)
    while (query.next()) {
        ui->tableWidget_5->insertRow(i);
        ui->tableWidget_5->setItem(i, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableWidget_5->setItem(i, 1, new QTableWidgetItem(Utility::convertCodeToReferenceName(db,query.value(1).toString())));
        ui->tableWidget_5->setItem(i, 2, new QTableWidgetItem(query.value(2).toDateTime().toString("dd.MM.yyyy hh:mm:ss")));
    }
    query = QSqlQuery(db);
    selectPattern = "SELECT order_tid FROM orders_alerts.orders_alerts_info "
                "WHERE order_id ='"+code+"'; ";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    QString commandName;
    while (query.next()) {
        commandName = query.value(0).toString();
    }
    ui->commandsSignalsBox->setCurrentText(Utility::convertCodeToReferenceName(db,commandName));
}

void CommandsAddForm::setDataDocument(QString code)
{
    QSqlQuery query = QSqlQuery(db);
    QString selectPattern = "SELECT cmbdid, outgoing_reg_datetime "
                "FROM combatdocs.combatdocs_info WHERE cmbdid ='"+code+"'; ";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    while (query.next()) {
        ui->docDateRegisterTimeEdit->setDateTime(query.value(1).toDateTime());
        ui->docNumberlineEdit->setText(query.value(0).toString());
    }
    ui->menuBox->setCurrentIndex(1);
    ui->stackedWidget->setCurrentIndex(1);
    ui->menuBox->setEnabled(false);
    query = QSqlQuery(db);
    selectPattern = "SELECT combat_hierarchy, mark_tid, mark_time "
                "FROM combatdocs.combatdocs_acceptors WHERE cmbdid ='"+code+"';";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    ui->tableWidget_6->setRowCount(0);
    int n = query.size();
    for (int i=0; i < n; i++)
    while (query.next()) {
        ui->tableWidget_6->insertRow(i);
        ui->tableWidget_6->setItem(i, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableWidget_6->setItem(i, 1, new QTableWidgetItem(Utility::convertCodeToReferenceName(db,query.value(1).toString())));
        ui->tableWidget_6->setItem(i, 2, new QTableWidgetItem(query.value(2).toDateTime().toString("dd.MM.yyyy hh:mm:ss")));
    }
    query = QSqlQuery(db);
    selectPattern = "SELECT doctheme_tid FROM combatdocs.combatdocs_theme "
                "WHERE cmbdid ='"+code+"';";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    QString commandName;
    while (query.next()) {
        commandName = query.value(0).toString();
    }
    ui->docThemeBox->setCurrentText(Utility::convertCodeToReferenceName(db,commandName));
    query = QSqlQuery(db);
    selectPattern = "SELECT doctype_tid FROM combatdocs.combatdocs_type "
                "WHERE cmbdid ='"+code+"';";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    while (query.next()) {
        commandName = query.value(0).toString();
    }
    ui->docTypeBox->setCurrentText(Utility::convertCodeToReferenceName(db,commandName));
}

int CommandsAddForm::getSaveMode() const
{
    return saveMode;
}

void CommandsAddForm::setSaveMode(int value)
{
    saveMode = value;
}

void CommandsAddForm::changeContent()
{
    if ( ui->stackedWidget->currentIndex() == 0 ) {
        ui->stackedWidget->setCurrentIndex(1);
    }
    else {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void CommandsAddForm::receiveDataParametrs(QString parametr, QString value)
{
    int n = ui->tableWidget_4->rowCount();
    if (n == 0) {
        ui->tableWidget_4->setRowCount(1);
    }
    else {
        ui->tableWidget_4->insertRow(n);
    }
    n = ui->tableWidget_4->rowCount();
    ui->tableWidget_4->setItem(n-1, 0, new QTableWidgetItem(parametr));
    ui->tableWidget_4->setItem(n-1, 1, new QTableWidgetItem(value));
}

void CommandsAddForm::receiveDataReceivers(QStringList receiver, QString mark)
{
    if (!getCommandOrDoc()) {
        for (int i=0; i<receiver.size(); i++) {
            int n = ui->tableWidget_5->rowCount();
            if (n == 0) {
                ui->tableWidget_5->setRowCount(1);
            }
            else {
                ui->tableWidget_5->insertRow(n);
            }
            n = ui->tableWidget_5->rowCount();
            ui->tableWidget_5->setItem(n-1, 0, new QTableWidgetItem(receiver.at(i)));
            ui->tableWidget_5->setItem(n-1, 1, new QTableWidgetItem(Utility::convertCodeToReferenceName(db,mark)));
            ui->tableWidget_5->setItem(n-1, 2, new QTableWidgetItem(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")));
        }
    }
    else {
        for (int i=0; i<receiver.size(); i++) {
            int n = ui->tableWidget_6->rowCount();
            if (n == 0) {
                ui->tableWidget_6->setRowCount(1);
            }
            else {
                ui->tableWidget_6->insertRow(n);
            }
            n = ui->tableWidget_6->rowCount();
            ui->tableWidget_6->setItem(n-1, 0, new QTableWidgetItem(receiver.at(i)));
            ui->tableWidget_6->setItem(n-1, 1, new QTableWidgetItem(Utility::convertCodeToReferenceName(db,mark)));
            ui->tableWidget_6->setItem(n-1, 2, new QTableWidgetItem(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")));
        }
    }
}

void CommandsAddForm::setCommandsSignals()
{
    QSqlQuery query;
    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '70.10.*{1,}';";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    ui->commandsSignalsBox->addItems(list);
}

void CommandsAddForm::setDocumentsType()
{
    QSqlQuery query;
    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '10.02.*{1,}';";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    ui->docTypeBox->addItems(list);
}

void CommandsAddForm::setDocumentsTheme()
{
    QSqlQuery query;
    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '10.01.*{1,}';";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    ui->docThemeBox->addItems(list);
}

void CommandsAddForm::setAttributeExecution()
{
    QSqlQuery query;
    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '70.30.*{1,}';";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    ui->attrExecBox->addItems(list);
}

void CommandsAddForm::setDocNumber() {
    QSqlQuery query = QSqlQuery(db);
    QString s;
    s = "SELECT outgoing_reg_number FROM combatdocs.combatdocs_info ORDER BY outgoing_reg_number DESC LIMIT 1;";
    int id;
    query.exec(s);
    while (query.next()) {
        id = query.value( 0 ).toInt();
    }
    id++;
    ui->docNumberlineEdit->setText(QString::number(id));
}

void CommandsAddForm::changeEnabledTimerExec()
{
    if ( ui->timeExecBox->isChecked() ) {
        ui->timeExecDTE->setEnabled(true);
    }
    else {
        ui->timeExecDTE->setEnabled(false);
    }
}
