#include "battleOrderChangesBM.h"
#include "ui_battleOrderChangesBM.h"

battleOrderChangesBM::battleOrderChangesBM(QSqlDatabase db, QString combatHierarchy, QWidget *parent) :
    db(db),
    combatHierarchy(combatHierarchy),
    QWidget(parent),
    ui(new Ui::battleOrderChangesBM)
{
    ui->setupUi(this);

    getData();
}

void battleOrderChangesBM::getData()
{
    qDebug()<<db;
    qDebug()<<combatHierarchy;

    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    query.prepare( "SELECT t3.termname, t2.termname, to_char(m.date_add, 'DD.MM.YYYY HH24:MI:SS'), "
                   "       ST_X(l.obj_location)::varchar, "
                   "       CASE substring(ST_X(l.obj_location)::varchar from 1 for 1) "
                   "         WHEN '-' THEN 'S' "
                   "         ELSE 'N'          "
                   "       END,                "
                   "       ST_Y(l.obj_location)::varchar, "
                   "       CASE substring(ST_Y(l.obj_location)::varchar from 1 for 1) "
                   "         WHEN '-' THEN 'W' "
                   "         ELSE 'E'          "
                   "       END,                "
                   "       ST_Z(l.obj_location)::varchar, "
                   "       to_char(l.date_add, 'DD.MM.YYYY HH24:MI:SS'), l.direction "
                   "FROM own_forces.combatstructure c "
                   "LEFT JOIN own_forces.currentmode cur         ON cur.combat_hierarchy = c.combat_hierarchy "
                   "LEFT JOIN own_forces.combatobject_manner m   ON m.combat_hierarchy   = c.combat_hierarchy "
                   "LEFT JOIN own_forces.combatobject_location l ON l.combat_hierarchy   = c.combat_hierarchy "
                   "LEFT JOIN reference_data.terms t0            ON c.object_name = t0.termhierarchy "
                   "LEFT JOIN reference_data.terms t1            ON c.object_name = t1.termhierarchy "
                   "LEFT JOIN reference_data.terms t2            ON m.manner_tid  = t2.termhierarchy "
                   "LEFT JOIN reference_data.terms t3            ON cur.currentmode_tid = t3.termhierarchy "
                   "WHERE c.date_delete IS NULL "
                   "      AND c.combat_hierarchy = ? " );
    query.addBindValue( combatHierarchy );
    query.exec();

    while(query.next())
    {
        ui->lineEditMode->setText( query.value(0).toString() );
        ui->comboBoxState->addItem( query.value(1).toString() );
        ui->lineEditTimeState->setText( query.value(2).toString() );

        ui->lineEditLatitude->setText( query.value(3).toString() );
        ui->comboBoxNS->addItem( query.value(4).toString() );
        ui->lineEditLongitude->setText( query.value(5).toString() );
        ui->comboBoxEW->addItem( query.value(6).toString() );;
        ui->lineEditHeight->setText( query.value(7).toString() );

        ui->lineEditTimeCoordinates->setText( query.value(8).toString() );
        ui->lineEditAzimuth->setText( query.value(9).toString() );
        qDebug() << query.value(0);
    }

    db.commit();

}

battleOrderChangesBM::~battleOrderChangesBM()
{
    delete ui;
}
