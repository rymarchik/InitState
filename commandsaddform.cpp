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

    addParamForm= new AddParamForm();
    addReciversForm = new AddReciversForm();

    connect(ui->timeExecBox, SIGNAL(clicked(bool)), this, SLOT(changeEnabledTimerExec()));
    connect(ui->menuBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeContent()));
    connect(ui->addParamBut, SIGNAL(clicked()), addParamForm, SLOT(show()));
    connect(addParamForm, SIGNAL(sendData(QString, QString)), this, SLOT(receiveDataParametrs(QString, QString)));
    connect(ui->addReceiverBut, SIGNAL(clicked()), addReciversForm, SLOT(show()));
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

CommandsMessageBox CommandsAddForm::getInformationBox()
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
    return box;
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

void CommandsAddForm::changeEnabledTimerExec()
{
    if ( ui->timeExecBox->isChecked() ) {
        ui->timeExecDTE->setEnabled(true);
    }
    else {
        ui->timeExecDTE->setEnabled(false);
    }
}
