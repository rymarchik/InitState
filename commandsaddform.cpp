#include "commandsaddform.h"
#include "ui_commandsaddform.h"

CommandsAddForm::CommandsAddForm(QString ownName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandsAddForm)
{
    ui->setupUi(this);
    ui->dataSourceLine->setText(ownName);
    ui->timeCreateDTE->setDateTime(QDateTime::currentDateTime());
    ui->timeExecDTE->setDateTime(QDateTime::currentDateTime());
    ui->timeDocRegister->setDateTime(QDateTime::currentDateTime());
    ui->stackedWidget->setCurrentIndex(0);
    ui->tableWidget_4->setRowCount(0);
    setCommandsSignals();
    setAttributeExecution();

    addParamForm= new AddParamForm();
    addReciversForm = new AddReciversForm();

    connect(ui->timeExecBox, SIGNAL(clicked(bool)), this, SLOT(changeEnabledTimerExec()));
    connect(ui->menuBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeContent()));
    connect(ui->addParamBut, SIGNAL(clicked()), addParamForm, SLOT(show()));
    connect(addParamForm, SIGNAL(sendData(QString, QString)), this, SLOT(recieveData(QString, QString)));
    connect(ui->addReciverBut, SIGNAL(clicked()), addReciversForm, SLOT(show()));
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

QString CommandsAddForm::getCommandName()
{
    return ui->commandsSignalsBox->currentText();
}

QString CommandsAddForm::getTimeAdd()
{
    return ui->timeCreateDTE->text();
}

QString CommandsAddForm::getTimeExec()
{
    if (ui->timeExecBox->isChecked()) return ui->timeExecDTE->text();
    else return "0";
}

QString CommandsAddForm::getAttributeExec()
{
    return ui->attrExecBox->currentText();
}

QStringList CommandsAddForm::getParametrList()
{
    QStringList paramList;
    return paramList;
}

QStringList CommandsAddForm::getReceiversList()
{
    QStringList receiverList;
    return receiverList;
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

void CommandsAddForm::addRecivers()
{
}

void CommandsAddForm::recieveData(QString parametr, QString value)
{
    int n = ui->tableWidget_4->rowCount();
    if (n == 0) {
        ui->tableWidget_4->setRowCount(1);
    }
    else {
        ui->tableWidget_4->insertRow(1);
    }
    n = ui->tableWidget_4->rowCount();
    ui->tableWidget_4->setItem(n-1, 0, new QTableWidgetItem(parametr));
    ui->tableWidget_4->setItem(n-1, 1, new QTableWidgetItem(value));

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
