#include "battleOrderChangesMBU.h"
#include "ui_battleOrderChangesMBU.h"

battleOrderChangesMBU::battleOrderChangesMBU(QSqlDatabase db, QString combatHierarchy, QWidget *parent) :
    db(db),
    combatHierarchy(combatHierarchy),
    QWidget(parent),
    ui(new Ui::battleOrderChangesMBU)
{
    ui->setupUi(this);
    //connect (ui->);

    getData();
}

void battleOrderChangesMBU::getData()
{
    qDebug()<<db;
    qDebug()<<combatHierarchy;

    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    query.prepare( "SELECT t3.termname, t2.termname, to_char(m.date_add, 'DD.MM.YYYY HH24:MI:SS') "
                   "FROM own_forces.combatstructure c "
                   "LEFT JOIN own_forces.currentmode cur       ON cur.combat_hierarchy = c.combat_hierarchy "
                   "LEFT JOIN own_forces.combatobject_manner m ON m.combat_hierarchy = c.combat_hierarchy "
                   "LEFT JOIN reference_data.terms t0          ON c.object_name = t0.termhierarchy "
                   "LEFT JOIN reference_data.terms t1          ON c.object_name = t1.termhierarchy "
                   "LEFT JOIN reference_data.terms t2          ON m.manner_tid  = t2.termhierarchy "
                   "LEFT JOIN reference_data.terms t3          ON cur.currentmode_tid = t3.termhierarchy "
                   "WHERE c.date_delete IS NULL "
                   "      AND c.combat_hierarchy = ? " );
    query.addBindValue( combatHierarchy );
    query.exec();

    while(query.next())
    {
        ui->lineEditMode->setText( query.value(0).toString() );
        ui->lineEditTime_1->setText( query.value(2).toString() );

        qDebug() << query.value(0);
    }

    db.commit();

}

void battleOrderChangesMBU::setData()
{

}

battleOrderChangesMBU::~battleOrderChangesMBU()
{
    delete ui;
}
