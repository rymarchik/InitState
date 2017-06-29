#include "battleOrderDialog.h"
#include "ui_battleOrderDialog.h"

battleOrderDialog::battleOrderDialog(QSqlDatabase db, QString combatHierarchy, QString munitionName, int numDialog, QWidget *parent) : //QDialog(parent),
    db(db),
    combatHierarchy(combatHierarchy),
    munition(munitionName),
    numDlg(numDialog),
    QDialog(parent),
    ui(new Ui::battleOrderDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox->button( QDialogButtonBox::Ok     ), SIGNAL(clicked()), this, SLOT(slotAccept())    );
    connect(ui->buttonBox->button( QDialogButtonBox::Ok     ), SIGNAL(clicked()), this, SLOT(slotSendSignal()));
    connect(ui->buttonBox->button( QDialogButtonBox::Cancel ), SIGNAL(clicked()), this, SLOT(reject())        );
}

void battleOrderDialog::getCountPlace(int numDialog)
{
    getComboBoxName(munition);
    //qDebug()<<"munitionName: "<<munition;

    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    query.prepare( "SELECT mu.count, mu.place "
                   "FROM own_forces.combatstructure c "
                   "LEFT JOIN own_forces.munition  mu  ON mu.combat_hierarchy  = c.combat_hierarchy "
                   "LEFT JOIN reference_data.terms t   ON mu.name_tid   = t.termhierarchy "
                   "WHERE mu.date_delete IS NULL "
                   "      AND c.combat_hierarchy = ? "
                   "      AND t.termname = ? ");
    query.addBindValue( combatHierarchy );
    query.addBindValue( munition );
    query.exec();

    switch (numDialog) {
    case 1:
        ui->lineEditCount->setText("");
        ui->lineEditPlace->setText("");
        break;
    case 2:
        while(query.next())
        {
            ui->lineEditCount->setText(query.value(0).toString());
            ui->lineEditPlace->setText(query.value(1).toString());
        }
        break;
    case 3:
        while(query.next())
        {
            ui->lineEditCount->setText(query.value(0).toString());
            ui->lineEditPlace->setText(query.value(1).toString());
        }
        break;
    }
    db.commit();
}

void battleOrderDialog::getComboBoxName(QString str)
{
    ui->comboBoxName->clear();

    int index = 0;
    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    query.prepare( "SELECT termhierarchy, termname FROM reference_data.terms WHERE termhierarchy ~ '56.10.*{1}' OR termhierarchy ~ '56.20.*{1}'" );
    query.exec();

    while(query.next())
    {
        ui->comboBoxName->addItem( query.value(1).toString(), query.value(0).toString() );
        index = ui->comboBoxName->findText(str);
        ui->comboBoxName->setCurrentIndex(index);
    }
    db.commit();
}

QString battleOrderDialog::getMunitionName()
{
    qDebug() << ui->comboBoxName->itemData(ui->comboBoxName->currentIndex()).toString();;
    return ui->comboBoxName->itemData(ui->comboBoxName->currentIndex()).toString();
}

int battleOrderDialog::getCount()
{
    return ui->lineEditCount->text().toInt();
}

QString battleOrderDialog::getPlace()
{
    return ui->lineEditPlace->text();
}

void battleOrderDialog::setDisabledTextEdit(bool x, bool y)
{
    ui->comboBoxName->setDisabled( x );
    ui->lineEditCount->setDisabled( y );
    ui->lineEditPlace->setDisabled( y );
}

void battleOrderDialog::slotAccept()
{
    accept();
    QSqlQuery query = QSqlQuery(db);

    /*
    qDebug()<< "numDialog: " << numDlg <<
               "combatHierarchy: "<< combatHierarchy <<
               "getMunitionName: "<< getMunitionName() <<
               "getCount: "<< getCount() <<
               "getPlace: "<< getPlace();
    */

    switch (numDlg) {
    case 1:
        db.transaction();
        query.prepare( "SELECT * FROM own_forces.set_munition(?, ?, ?, ?, ?)" );
        query.addBindValue( combatHierarchy );
        query.addBindValue( getMunitionName() );
        query.addBindValue( getCount() );
        query.addBindValue( getPlace() );
        query.addBindValue( 1 );

        if (!query.exec())
        {
            QSqlError err = query.lastError().text();
            QMessageBox::critical(0,  ("Соединение не установлено "), err.text(), QMessageBox::Ok);
            db.rollback();
        }

        while (query.next())
        {
            int priznak = query.value(0).toInt();

            if (priznak == 0)
            {
                QMessageBox::critical(0,  ("Ошибка"), ("Такая запись уже существует!"), QMessageBox::Ok);
                //qDebug() << query.exec();
            }
            else
            {
                QMessageBox::information(0,  ("Информация"), ("Добавление прошло успешно!"), QMessageBox::Ok);
                //qDebug() << query.exec();
            }
        }
        db.commit();
        break;
    case 2:
        db.transaction();
        query.prepare( "SELECT * FROM own_forces.set_munition(?, ?, ?, ?, ?)" );
        query.addBindValue( combatHierarchy );
        query.addBindValue( getMunitionName() );
        query.addBindValue( getCount() );
        query.addBindValue( getPlace() );
        query.addBindValue( 2 );

        if (!query.exec())
        {
            QSqlError err = query.lastError().text();
            QMessageBox::critical(0,  ("Соединение не установлено "), err.text(), QMessageBox::Ok);
            db.rollback();
        }
        else
        {
            while (query.next())
            {
                if ( query.value(0).toInt() )
                {
                    QMessageBox::information(0,  ("Информация"), ("Изменение прошло успешно!"), QMessageBox::Ok);
                }
            }

        }
        db.commit();
        break;
    case 3:
        db.transaction();
        query.prepare( "SELECT * FROM own_forces.set_munition(?, ?, ?, ?, ?)" );
        query.addBindValue( combatHierarchy );
        query.addBindValue( getMunitionName() );
        query.addBindValue( getCount() );
        query.addBindValue( getPlace() );
        query.addBindValue( 3 );
        if (!query.exec())
        {
            QSqlError err = query.lastError().text();
            QMessageBox::critical(0,  ("Соединение не установлено "), err.text(), QMessageBox::Ok);
            db.rollback();
        }
        else
        {
            while (query.next())
            {
                if ( query.value(0).toInt() )
                {
                    QMessageBox::information(0,  ("Информация"), ("Удаление прошло успешно!"), QMessageBox::Ok);
                }
            }

        }
        db.commit();
        break;
    }
}

battleOrderDialog::~battleOrderDialog()
{
    delete ui;
}
