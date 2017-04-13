#include "addreciversform.h"
#include "ui_addreciversform.h"

AddReciversForm::AddReciversForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddReciversForm)
{
    ui->setupUi(this);

    checkBoxList = new QList<QCheckBox*>;
}

void AddReciversForm::on_reverseButton_clicked()
{
    QMessageBox::information(0, "Отмена", "Добавление параметра отменено");
    this->close();
}

void AddReciversForm::on_okButton_clicked()
{
    /*if (!ui->valueLine->text().isEmpty()) {
        emit sendData(ui->paramsBox->itemText(ui->paramsBox->currentIndex()),
                  ui->valueLine->text());
        QMessageBox::information(0, "Отмена", "Параметр добавлен успешно");
        this->close();
    }
    else {
        (new QErrorMessage(this))->showMessage("Ошибка, не заполнено поле");
    }*/
}

AddReciversForm::~AddReciversForm()
{
    delete ui;
}
