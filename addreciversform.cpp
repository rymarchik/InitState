#include "addreciversform.h"
#include "ui_addreciversform.h"

AddReciversForm::AddReciversForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddReciversForm)
{
    ui->setupUi(this);

    QSqlQuery query;
    if (!query.exec("SELECT c.combat_hierarchy, c.object_number||' '||t1.termname "
                     "FROM own_forces.combatstructure c "
                     "LEFT JOIN reference_data.terms t1 ON object_name = t1.termhierarchy "
                     "LEFT JOIN reference_data.terms t2 ON type_army   = t2.termhierarchy "
                     "WHERE c.date_delete is NULL ")) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    QVBoxLayout* layout = new QVBoxLayout;
    ui->scrollArea->widget()->setLayout(layout);
    while (query.next()) {
        QCheckBox* thisLine = new QCheckBox;
        thisLine->setChecked(false);
        thisLine->setText(query.value(1).toString());
        layout->addWidget(thisLine);
    }
}

void AddReciversForm::on_reverseButton_clicked()
{
    QMessageBox::information(0, "Отмена", "Добавление параметра отменено");
    this->close();
}

void AddReciversForm::on_okButton_clicked()
{
    qDebug() << ui->scrollArea->widget()->layout()->count();

    for (int i = 0; i < ui->scrollArea->widget()->layout()->count(); i++)
    {
        //QCheckBox* box = (QCheckBox*) ui->scrollArea->widget()->layout()->itemAt(i);
        //if (box->isChecked()) qDebug() << box->text();
    }
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
