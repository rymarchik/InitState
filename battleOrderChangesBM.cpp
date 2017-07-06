#include "battleOrderChangesBM.h"
#include "ui_battleOrderChangesBM.h"

battleOrderChangesBM::battleOrderChangesBM(QSqlDatabase db, QString combatHierarchy, QWidget *parent) :
    db(db),
    combatHierarchy(combatHierarchy),
    QWidget(parent),
    ui(new Ui::battleOrderChangesBM),
    m_DialogBM(new battleOrderDialogBM(db, combatHierarchy, this))
{
    ui->setupUi(this);

    ui->lineEditCoordinates->setInputMask(">99°99'99.99\"A 999°99'99.99\"A 9999.9;_");
    ui->lineEditAzimuth->setInputMask(">999°99'99.99\";_");

    connect( ui->tableWidgetData, SIGNAL(cellClicked(int, int)), this, SLOT(slotTableMunitionSignal()) );
    connect( m_DialogBM,          SIGNAL(signalRecordDB()),      this, SLOT(slotTableMunition())       );
    //connect( m_twView, SIGNAL(tabCloseRequested(int)), this, SLOT(slotCloseTab(int)));
}

void battleOrderChangesBM::slotData()
{
    slotStatusStatement();
    slotDataOnRockets();
    slotTableMunition();
}

void battleOrderChangesBM::comboBoxManner(QString str)
{
    int index;
    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    query.prepare( "SELECT termhierarchy, termname FROM reference_data.terms WHERE termhierarchy ~ '51.10.*{1}'" );
    query.exec();

    while(query.next())
    {
        ui->comboBoxManner->addItem( query.value(1).toString(), query.value(0).toString() );
        index = ui->comboBoxManner->findText(str);
        ui->comboBoxManner->setCurrentIndex(index);
    }
    db.commit();
}

void battleOrderChangesBM::comboBoxTypeRockets(int num, QString str)
{
    int index;
    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    query.prepare( "SELECT termhierarchy, termname FROM reference_data.terms WHERE termhierarchy ~ '51.50.*{1}'" );
    query.exec();

    while(query.next())
    {
        switch (num) {
        case 1:
            ui->comboBoxType_1->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxType_1->findText(str);
            ui->comboBoxType_1->setCurrentIndex(index);
            break;
        case 2:
            ui->comboBoxType_2->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxType_2->findText(str);
            ui->comboBoxType_2->setCurrentIndex(index);
            break;
        case 3:
            ui->comboBoxType_3->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxType_3->findText(str);
            ui->comboBoxType_3->setCurrentIndex(index);
            break;
        case 4:
            ui->comboBoxType_4->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxType_4->findText(str);
            ui->comboBoxType_4->setCurrentIndex(index);
            break;
        case 5:
            ui->comboBoxType_5->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxType_5->findText(str);
            ui->comboBoxType_5->setCurrentIndex(index);
            break;
        case 6:
            ui->comboBoxType_6->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxType_6->findText(str);
            ui->comboBoxType_6->setCurrentIndex(index);
            break;
        case 7:
            ui->comboBoxType_7->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxType_7->findText(str);
            ui->comboBoxType_7->setCurrentIndex(index);
            break;
        case 8:
            ui->comboBoxType_8->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxType_8->findText(str);
            ui->comboBoxType_8->setCurrentIndex(index);
            break;
        }
    }
    db.commit();
}

void battleOrderChangesBM::comboBoxState(int num, QString str)
{
    int index;
    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    query.prepare( "SELECT termhierarchy, termname FROM reference_data.terms WHERE termhierarchy ~ '51.40.*{1}'" );
    query.exec();

    while(query.next())
    {
        switch (num) {
        case 1:
            ui->comboBoxState_1->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxState_1->findText(str);
            ui->comboBoxState_1->setCurrentIndex(index);
            break;
        case 2:
            ui->comboBoxState_2->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxState_2->findText(str);
            ui->comboBoxState_2->setCurrentIndex(index);
            break;
        case 3:
            ui->comboBoxState_3->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxState_3->findText(str);
            ui->comboBoxState_3->setCurrentIndex(index);
            break;
        case 4:
            ui->comboBoxState_4->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxState_4->findText(str);
            ui->comboBoxState_4->setCurrentIndex(index);
            break;
        case 5:
            ui->comboBoxState_5->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxState_5->findText(str);
            ui->comboBoxState_5->setCurrentIndex(index);
            break;
        case 6:
            ui->comboBoxState_6->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxState_6->findText(str);
            ui->comboBoxState_6->setCurrentIndex(index);
            break;
        case 7:
            ui->comboBoxState_7->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxState_7->findText(str);
            ui->comboBoxState_7->setCurrentIndex(index);
            break;
        case 8:
            ui->comboBoxState_8->addItem( query.value(1).toString(), query.value(0).toString() );
            index = ui->comboBoxState_8->findText(str);
            ui->comboBoxState_8->setCurrentIndex(index);
            break;
        }
    }
    db.commit();
}

void battleOrderChangesBM::slotStatusStatement()
{
    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    query.prepare( "SELECT t3.termname, t2.termname, to_char(m.date_add, 'DD.MM.YYYY HH24:MI:SS'), "
                   "       own_forces.coordinates_output(l.obj_location), "
                   "       to_char(l.date_add, 'DD.MM.YYYY HH24:MI:SS'), own_forces.azimut_output(l.direction) "
                   "FROM own_forces.combatstructure c "
                   "LEFT JOIN own_forces.currentmode cur         ON cur.combat_hierarchy = c.combat_hierarchy AND cur.date_delete IS NULL "
                   "LEFT JOIN own_forces.combatobject_manner m   ON m.combat_hierarchy   = c.combat_hierarchy AND m.date_delete IS NULL "
                   "LEFT JOIN own_forces.combatobject_location l ON l.combat_hierarchy   = c.combat_hierarchy AND l.date_delete IS NULL "
                   "LEFT JOIN reference_data.terms t0            ON c.object_name = t0.termhierarchy "
                   "LEFT JOIN reference_data.terms t1            ON c.object_name = t1.termhierarchy "
                   "LEFT JOIN reference_data.terms t2            ON m.manner_tid  = t2.termhierarchy "
                   "LEFT JOIN reference_data.terms t3            ON cur.currentmode_tid = t3.termhierarchy "
                   "WHERE c.date_delete IS NULL "
                   "      AND c.combat_hierarchy = ? ");
    query.addBindValue( combatHierarchy );
    query.exec();

    for(int i=1; query.next(); i++)
    {
        ui->lineEditMode->setText( query.value(0).toString() );
        comboBoxManner( query.value(1).toString() );
        ui->lineEditTimeState->setText( query.value(2).toString() );
        ui->lineEditCoordinates->setText( query.value(3).toString() );
        ui->lineEditTimeCoordinates->setText( query.value(4).toString() );
        ui->lineEditAzimuth->setText( query.value(5).toString() );
    }
    db.commit();
}

void battleOrderChangesBM::slotDataOnRockets()
{
    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    query.prepare( "SELECT t1.termname, to_char(r.date_add, 'DD.MM.YYYY HH24:MI:SS'), t2.termname, r.number "
                   "FROM own_forces.rocket r "
                   "LEFT JOIN reference_data.terms t1  ON r.type_tid    = t1.termhierarchy "
                   "LEFT JOIN reference_data.terms t2  ON r.state_tid   = t2.termhierarchy "
                   "WHERE r.date_delete IS NULL "
                   "      AND r.combat_hierarchy = ? ");
    query.addBindValue( combatHierarchy );
    query.exec();

    while(query.next())
    {
        int i = query.value(3).toInt();
        switch (i) {
        case 1:
            comboBoxTypeRockets( i, query.value(0).toString() );
            ui->lineEditTime_1->setText( query.value(1).toString());
            comboBoxState( i, query.value(2).toString() );
            break;
        case 2:
            comboBoxTypeRockets(i, query.value(0).toString());
            ui->lineEditTime_2->setText( query.value(1).toString());
            comboBoxState( i, query.value(2).toString() );
            break;
        case 3:
            comboBoxTypeRockets(i, query.value(0).toString() );
            ui->lineEditTime_3->setText( query.value(1).toString());
            comboBoxState( i, query.value(2).toString() );
            break;
        case 4:
            comboBoxTypeRockets(i, query.value(0).toString() );
            ui->lineEditTime_4->setText( query.value(1).toString());
            comboBoxState( i, query.value(2).toString() );
            break;
        case 5:
            comboBoxTypeRockets(i, query.value(0).toString() );
            ui->lineEditTime_5->setText( query.value(1).toString());
            comboBoxState( i, query.value(2).toString() );
            break;
        case 6:
            comboBoxTypeRockets(i, query.value(0).toString() );
            ui->lineEditTime_6->setText( query.value(1).toString());
            comboBoxState( i, query.value(2).toString() );
            break;
        case 7:
            comboBoxTypeRockets(i, query.value(0).toString() );
            ui->lineEditTime_7->setText( query.value(1).toString());
            comboBoxState( i, query.value(2).toString() );
            break;
        case 8:
            comboBoxTypeRockets(i, query.value(0).toString() );
            ui->lineEditTime_8->setText( query.value(1).toString());
            comboBoxState( i, query.value(2).toString() );
            break;
        }
    }
    db.commit();
}

void battleOrderChangesBM::slotTableMunition()
{
    ui->tableWidgetData->clearContents();       //удаление содержимого таблицы, сохранение названия полей
    ui->tableWidgetData->setRowCount(0);        //обнуление строк в таблице

    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    query.prepare( "SELECT t.termname, mu.count, mu.place, mu.tid  "
                   "FROM own_forces.combatstructure c "
                   "LEFT JOIN own_forces.munition  mu  ON mu.combat_hierarchy  = c.combat_hierarchy "
                   "LEFT JOIN reference_data.terms t   ON mu.name_tid   = t.termhierarchy "
                   "WHERE mu.date_delete IS NULL "
                   "      AND c.date_delete IS NULL "
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

void battleOrderChangesBM::slotTableMunitionSignal()
{        
    m_DialogBM->munition = ui->tableWidgetData->selectedItems().value(0)->text();
    m_DialogBM->show();
}

QString battleOrderChangesBM::getManner()
{
    return ui->comboBoxManner->itemData(ui->comboBoxManner->currentIndex()).toString();
}

QString battleOrderChangesBM::getCoordinates()
{
    QString coordinates = ui->lineEditCoordinates->text();
    //qDebug()<<"coordinates1: "<< coordinates ;
    //coordinates = coordinates.replace(QRegExp("[']"), "''");
    coordinates = coordinates.replace(QRegExp("[\"]"), "''");
    qDebug()<< "coordinates: " << coordinates;
    return coordinates;
}

QString battleOrderChangesBM::getAzimuth()
{
    QString azimuth = ui->lineEditAzimuth->text();
    //azimuth = azimuth.replace(QRegExp("[']"), "''");
    azimuth = azimuth.replace(QRegExp("[\"]"), "''");
    qDebug()<< "azimuth: " << azimuth;
    return azimuth;
}

QString battleOrderChangesBM::getTypeRocket(int number)
{
    switch (number) {
    case 1:
        return ui->comboBoxType_1->itemData(ui->comboBoxType_1->currentIndex()).toString();
        break;
    case 2:
        return ui->comboBoxType_2->itemData(ui->comboBoxType_2->currentIndex()).toString();
        break;
    case 3:
        return ui->comboBoxType_3->itemData(ui->comboBoxType_3->currentIndex()).toString();
        break;
    case 4:
        return ui->comboBoxType_4->itemData(ui->comboBoxType_4->currentIndex()).toString();
        break;
    case 5:
        return ui->comboBoxType_5->itemData(ui->comboBoxType_5->currentIndex()).toString();
        break;
    case 6:
        return ui->comboBoxType_6->itemData(ui->comboBoxType_6->currentIndex()).toString();
        break;
    case 7:
        return ui->comboBoxType_7->itemData(ui->comboBoxType_7->currentIndex()).toString();
        break;
    case 8:
        return ui->comboBoxType_8->itemData(ui->comboBoxType_8->currentIndex()).toString();
        break;
    default:
        return "0";
        break;
    }
}

QString battleOrderChangesBM::getStateRocket(int number)
{
    switch (number) {
    case 1:
        return ui->comboBoxState_1->itemData(ui->comboBoxState_1->currentIndex()).toString();
        break;
    case 2:
        return ui->comboBoxState_2->itemData(ui->comboBoxState_2->currentIndex()).toString();
        break;
    case 3:
        return ui->comboBoxState_3->itemData(ui->comboBoxState_3->currentIndex()).toString();
        break;
    case 4:
        return ui->comboBoxState_4->itemData(ui->comboBoxState_4->currentIndex()).toString();
        break;
    case 5:
        return ui->comboBoxState_5->itemData(ui->comboBoxState_5->currentIndex()).toString();
        break;
    case 6:
        return ui->comboBoxState_6->itemData(ui->comboBoxState_6->currentIndex()).toString();
        break;
    case 7:
        return ui->comboBoxState_7->itemData(ui->comboBoxState_7->currentIndex()).toString();
        break;
    case 8:
        return ui->comboBoxState_8->itemData(ui->comboBoxState_8->currentIndex()).toString();
        break;
    default:
        return "0";
        break;
    }
}

void battleOrderChangesBM::slotSave()
{
    qDebug()<<"Успех!";
    QSqlQuery query = QSqlQuery(db);    
/*
    qDebug()<< "combatHierarchy: "  << combatHierarchy << //endl <<
               "getManner(): "      << getManner() << //endl <<
               "getCoordinates(): " << getCoordinates() << //endl <<
               "getAzimuth(): "     << getAzimuth() << //endl <<
               "getTypeRocket(1): " << getTypeRocket(1) << //endl <<
               "getTypeRocket(2): " << getTypeRocket(2) << //endl <<
               "getTypeRocket(3): " << getTypeRocket(3) << //endl <<
               "getTypeRocket(4): " << getTypeRocket(4) << //endl <<
               "getTypeRocket(5): " << getTypeRocket(5) << //endl <<
               "getTypeRocket(6): " << getTypeRocket(6) << //endl <<
               "getTypeRocket(7): " << getTypeRocket(7) << //endl <<
               "getTypeRocket(8): " << getTypeRocket(8) << //endl <<
               "getStateRocket(1): "<< getStateRocket(1)<< //endl <<
               "getStateRocket(2): "<< getStateRocket(2)<< //endl <<
               "getStateRocket(3): "<< getStateRocket(3)<< //endl <<
               "getStateRocket(4): "<< getStateRocket(4)<< //endl <<
               "getStateRocket(5): "<< getStateRocket(5)<< //endl <<
               "getStateRocket(6): "<< getStateRocket(6)<< //endl <<
               "getStateRocket(7): "<< getStateRocket(7)<< //endl <<
               "getStateRocket(8): "<< getStateRocket(8);
*/
    db.transaction();
    query.prepare( "SELECT * FROM own_forces.set_changes_bm(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)" );
    query.addBindValue( combatHierarchy );
    query.addBindValue( getManner() );    
    query.addBindValue( getCoordinates() );
    query.addBindValue( getAzimuth() );
    query.addBindValue( getTypeRocket(1) );
    query.addBindValue( getTypeRocket(2) );
    query.addBindValue( getTypeRocket(3) );
    query.addBindValue( getTypeRocket(4) );
    query.addBindValue( getTypeRocket(5) );
    query.addBindValue( getTypeRocket(6) );
    query.addBindValue( getTypeRocket(7) );
    query.addBindValue( getTypeRocket(8) );
    query.addBindValue( getStateRocket(1) );
    query.addBindValue( getStateRocket(2) );
    query.addBindValue( getStateRocket(3) );
    query.addBindValue( getStateRocket(4) );
    query.addBindValue( getStateRocket(5) );
    query.addBindValue( getStateRocket(6) );
    query.addBindValue( getStateRocket(7) );
    query.addBindValue( getStateRocket(8) );
    query.addBindValue( 0 );
    query.exec();

    while(query.next())
    {
        qDebug() << "Возвращаемое значение транзакции сохранения: " << query.value(0).toString();
    }

    db.commit();
}

battleOrderChangesBM::~battleOrderChangesBM()
{
    delete m_DialogBM;
    delete ui;
}
