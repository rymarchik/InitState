#include "addparamform.h"
#include "ui_addparamform.h"

AddParamForm::AddParamForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddParamForm)
{
    ui->setupUi(this);

    QSqlQuery query;
    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '70.20.*{1,}';";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    ui->paramsBox->addItems(list);
}

AddParamForm::~AddParamForm()
{
    delete ui;
}

void AddParamForm::on_reverseButton_clicked()
{
    QMessageBox::information(0, "OK", "Добавление параметра отменено");
    this->close();
}

void AddParamForm::on_okButton_clicked()
{
    if (!ui->valueLine->text().isEmpty()) {
        emit sendData(ui->paramsBox->itemText(ui->paramsBox->currentIndex()),
                  ui->valueLine->text());
        QMessageBox::information(0, "Отмена", "Параметр добавлен успешно");
        this->close();
    }
    else {
        (new QErrorMessage(this))->showMessage("Ошибка, не заполнено поле");
    }
}
