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
                     "WHERE c.date_delete is NULL and t1.termhierarchy ~ '*{2}';")) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    QVBoxLayout* layout = new QVBoxLayout;
    ui->scrollArea->widget()->setLayout(layout);
    while (query.next()) {
        QCheckBox* thisLine = new QCheckBox;
        thisLine->setChecked(false);
        thisLine->setText(query.value(1).toString());
        checkBoxList << thisLine;
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
    QStringList receiversList;
    for (int i = 0; i < checkBoxList.size(); i++)
    {
        if (checkBoxList.at(i)->isChecked()) {
            //скрипт на добавление получателя
            receiversList << checkBoxList.at(i)->text();
        }
    }
    emit sendData(receiversList,
                  "80.10.05");
    QMessageBox::information(0, "OK", "Получатели добавлены успешно");
    this->close();
}

AddReciversForm::~AddReciversForm()
{
    delete ui;
}
