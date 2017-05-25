#include "battleOrderChangesBM.h"
#include "ui_battleOrderChangesBM.h"

battleOrderChangesBM::battleOrderChangesBM(QSqlDatabase db, QString combatHierarchy, QWidget *parent) :
    db(db),
    combatHierarchy(combatHierarchy),
    QWidget(parent),
    ui(new Ui::battleOrderChangesBM)
{
    ui->setupUi(this);
    ui->lineEditCoordinates->setInputMask(">99°99'99.99\"A 999°99'99.99\"A 9999.9;0");
    ui->lineEditAzimuth->setInputMask(">999°99'99.99\";0");
    getData();
}

void battleOrderChangesBM::getData()
{
    qDebug()<<db;
    qDebug()<<combatHierarchy;

    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    query.prepare( "SELECT t3.termname, t2.termname, to_char(m.date_add, 'DD.MM.YYYY HH24:MI:SS'), "
                   "       own_forces.get_coordinates(l.obj_location), "
                   "       to_char(l.date_add, 'DD.MM.YYYY HH24:MI:SS'), own_forces.get_azimut(l.direction), "
                   "       t4.termname, to_char(r.date_add, 'DD.MM.YYYY HH24:MI:SS'), t5.termname "
                   //"       t6.termname, mu.count, mu.place  "
                   "FROM own_forces.combatstructure c "
                   "LEFT JOIN own_forces.currentmode cur         ON cur.combat_hierarchy = c.combat_hierarchy "
                   "LEFT JOIN own_forces.combatobject_manner m   ON m.combat_hierarchy   = c.combat_hierarchy "
                   "LEFT JOIN own_forces.combatobject_location l ON l.combat_hierarchy   = c.combat_hierarchy "
                   "LEFT JOIN own_forces.rocket    r             ON r.combat_hierarchy   = c.combat_hierarchy "
                   //"LEFT JOIN own_forces.munition  mu            ON mu.combat_hierarchy  = c.combat_hierarchy "
                   "LEFT JOIN reference_data.terms t0            ON c.object_name = t0.termhierarchy "
                   "LEFT JOIN reference_data.terms t1            ON c.object_name = t1.termhierarchy "
                   "LEFT JOIN reference_data.terms t2            ON m.manner_tid  = t2.termhierarchy "
                   "LEFT JOIN reference_data.terms t3            ON cur.currentmode_tid = t3.termhierarchy "
                   "LEFT JOIN reference_data.terms t4            ON r.type_tid    = t4.termhierarchy "
                   "LEFT JOIN reference_data.terms t5            ON r.state_tid   = t5.termhierarchy "
                   //"LEFT JOIN reference_data.terms t6            ON mu.name_tid   = t6.termhierarchy "
                   "WHERE c.date_delete IS NULL "
                   "      AND c.combat_hierarchy = ? "
                   "order by r.number");
    query.addBindValue( combatHierarchy );
    query.exec();

    for(int i=1; query.next(); i++)
    {
        ui->lineEditMode->setText( query.value(0).toString() );
        ui->comboBoxState->addItem( query.value(1).toString() );        
        ui->lineEditTimeState->setText( query.value(2).toString() );
        ui->lineEditCoordinates->setText( query.value(3).toString() );
        ui->lineEditTimeCoordinates->setText( query.value(4).toString() );
        ui->lineEditAzimuth->setText( query.value(5).toString() );

        switch (i) {
        case 1:
            qDebug()<<i;
            ui->comboBoxType_1->addItem( query.value(6).toString() );
            ui->lineEditTime_1->setText( query.value(7).toString());
            ui->comboBoxState_1->addItem( query.value(8).toString() );
            break;
        case 2:
            ui->comboBoxType_2->addItem( query.value(6).toString() );
            ui->lineEditTime_2->setText( query.value(7).toString());
            ui->comboBoxState_2->addItem( query.value(8).toString() );
            break;
        case 3:
            ui->comboBoxType_3->addItem( query.value(6).toString() );
            ui->lineEditTime_3->setText( query.value(7).toString());
            ui->comboBoxState_3->addItem( query.value(8).toString() );
            break;
        case 4:
            ui->comboBoxType_4->addItem( query.value(6).toString() );
            ui->lineEditTime_4->setText( query.value(7).toString());
            ui->comboBoxState_4->addItem( query.value(8).toString() );
            break;
        case 5:
            ui->comboBoxType_5->addItem( query.value(6).toString() );
            ui->lineEditTime_5->setText( query.value(7).toString());
            ui->comboBoxState_5->addItem( query.value(8).toString() );
            break;
        case 6:
            ui->comboBoxType_6->addItem( query.value(6).toString() );
            ui->lineEditTime_6->setText( query.value(7).toString());
            ui->comboBoxState_6->addItem( query.value(8).toString() );
            break;
        case 7:
            ui->comboBoxType_7->addItem( query.value(6).toString() );
            ui->lineEditTime_7->setText( query.value(7).toString());
            ui->comboBoxState_7->addItem( query.value(8).toString() );
            break;
        case 8:
            ui->comboBoxType_8->addItem( query.value(6).toString() );
            ui->lineEditTime_8->setText( query.value(7).toString());
            ui->comboBoxState_8->addItem( query.value(8).toString() );
            break;
        }
    }
    db.commit();

    db.transaction();
    query.prepare( "SELECT t.termname, mu.count, mu.place  "
                   "FROM own_forces.combatstructure c "
                   "LEFT JOIN own_forces.munition  mu  ON mu.combat_hierarchy  = c.combat_hierarchy "
                   "LEFT JOIN reference_data.terms t   ON mu.name_tid   = t.termhierarchy "
                   "WHERE c.date_delete IS NULL "
                   "      AND c.combat_hierarchy = ? ");
    query.addBindValue( combatHierarchy );
    query.exec();
    for(int i = 0; query.next(); i++)
    {
        ui->tableWidgetData->insertRow(i);

        ui->tableWidgetData->setItem(i, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableWidgetData->setItem(i, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableWidgetData->setItem(i, 2, new QTableWidgetItem(query.value(2).toString()));
    }
    db.commit();
}

battleOrderChangesBM::~battleOrderChangesBM()
{
    delete ui;
}
