#include "mainwindow.h"

MainWindow::MainWindow(QSqlDatabase DB, int UserID, QWidget *parent) :
    db(DB),
    idUser(UserID),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Формирование названия формы:
    windowsTitle(idUser);

    gps = new GPSModule(db, this);

    ui->battleOrderTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    ui->battleOrderTabWidget->tabBar()->tabButton(1, QTabBar::RightSide)->resize(0, 0);
    ui->hitTargetsTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    ui->hitTargetsTabWidget->tabBar()->tabButton(1, QTabBar::RightSide)->resize(0, 0);
    ui->commandsTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    ui->commandsTabWidget->tabBar()->tabButton(1, QTabBar::RightSide)->resize(0, 0);

    c_battleOrder = new BattleOrder(db, combatHierarchy, currentMode, ui->navigatorBattleOrderTree, ui->navigatorBattleOrderReceivers,
                                    ui->changesBattleOrderTable, ui->changesBattleOrderReceivers);

    c_hitTargets  = new HitTargets(db, ui->navigatorHitTargetsTable, ui->navigatorHitTargetsReceivers,
                                   ui->changesHitTargetsTable, ui->changesHitTargetsReceivers);
    c_commands    = new Commands(db, ui->navigatorCommandsTree, ui->navigatorCommandsReceivers,
                                 ui->changesCommandsTree, ui->changesCommandsReceivers);

    currentContent   = c_battleOrder;
    currentTabWidget = ui->battleOrderTabWidget;

    ui->toolBox->setCurrentIndex(0);

    slotNavigator();
    connect(ui->toolBox, SIGNAL(currentChanged(int)), this, SLOT(slotChangeCurrentClass(int)));
    connect(ui->m_exit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(currentTabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotOnChangeTab(int)));
    connect(ui->navigatorCommandsTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slotOnItemSelected()));

    mapProcess = new QProcess(this);
    if (!NetworkModule::Instance().serverFunc()) {
        QMessageBox::critical(this, "Ошибка", "Server error!");
    }
    connect(mapProcess,                 SIGNAL(finished(int)),                                 SLOT(finished(int)));
    connect(&NetworkModule::Instance(), SIGNAL(serverError(QString)),                          SLOT(serverError(QString)));
    connect(&NetworkModule::Instance(), SIGNAL(sendNetworkUserMap(QTcpSocket*)),               SLOT(sendNetworkUserMap(QTcpSocket*)));
    connect(&NetworkModule::Instance(), SIGNAL(receiveInsertObjectNetwork(QByteArray&)),       SLOT(receiveInsertObjectNetwork(QByteArray&)));
    connect(&NetworkModule::Instance(), SIGNAL(receiveInsertObjectNetworkFromDB(QByteArray&)), SLOT(receiveInsertObjectNetworkFromDB(QByteArray&)));
    connect(&NetworkModule::Instance(), SIGNAL(receiveDeleteObjectNetwork(QByteArray&)),       SLOT(receiveDeleteObjectNetwork(QByteArray&)));
}

//Формирование названия формы:
void MainWindow::windowsTitle(int idUser)
{
    QSqlQuery query(db);
    db.transaction();
    query.prepare( " SELECT cs2.object_number || t2.termname || ' / ' || cs1.object_number || t1.termname, "
                   "        cs2.combat_hierarchy, cu.currentmode_tid, t3.termname "
                   " FROM reference_data.users u "
                   " LEFT JOIN own_forces.currentmode cu ON  cu.combat_hierarchy = u.combat_hierarchy "
                   "                                     AND cu.date_delete IS NULL  "
                   " LEFT JOIN own_forces.combatstructure cs2 ON  cs2.combat_hierarchy = u.combat_hierarchy "
                   "                                          AND cs2.date_delete IS NULL "
                   " LEFT JOIN own_forces.combatstructure cs1 ON  cs1.combat_hierarchy = subltree(cs2.combat_hierarchy,0,1) "
                   "                                          AND cs1.date_delete IS NULL "
                   " LEFT JOIN reference_data.terms t1 ON t1.termhierarchy = cs1.object_name "
                   " LEFT JOIN reference_data.terms t2 ON t2.termhierarchy = cs2.object_name "
                   " LEFT JOIN reference_data.terms t3 ON t3.termhierarchy = cu.currentmode_tid "
                   " WHERE u.user_id =  ? "
                   "       AND u.date_delete IS NULL  " );
    query.addBindValue( idUser );
    query.exec();

    while(query.next())
    {
        objectName      = query.value(0).toString();
        combatHierarchy = query.value(1).toString();
        currentMode     = query.value(2).toString();
        currentModeName = query.value(3).toString();
        //qDebug()<< "objectName: "  << objectName;
        //qDebug()<< "combatHierarchy1: "  << combatHierarchy;
        //qDebug()<< "currentMode: " << currentMode;
        //qDebug()<< "currentModeName: " << currentModeName;
    }
    db.commit();

    title = title + " - " + objectName + " ( " + currentModeName + " режим )";
    setWindowTitle(title);

    //c_battleOrder = new BattleOrder(db, currentMode, ui->navigatorBattleOrderTree, ui->navigatorBattleOrderReceivers,
    //                                ui->changesBattleOrderTable, ui->changesBattleOrderReceivers);
}

//заполнение закладки "Навигатор":
void MainWindow::slotNavigator()
{
    ui->m_navigator->setChecked(true);
    ui->m_changes->setChecked(false);
    currentContent->fillNavigator();
    currentTabWidget->setCurrentIndex(0);
    currentTabWidget->setTabEnabled(0, true);
    currentTabWidget->setTabEnabled(1, false);
}

//заполнение закладки "Изменить":
void MainWindow::slotChanges()
{
    ui->m_navigator->setChecked(false);
    ui->m_changes->setChecked(true);
    currentContent->fillChanges();
    currentTabWidget->setCurrentIndex(1);
    currentTabWidget->setTabEnabled(0, false);
    currentTabWidget->setTabEnabled(1, true);
}

//создание вкладки "Новый":
void MainWindow::slotAdd()
{
    QWidget* addTab = currentContent->onAdd();
    currentTabWidget->addTab(addTab, "Новый");
    currentTabWidget->setCurrentWidget(addTab);
}

//реализация функции "Править":
void MainWindow::slotEdit()
{
    QWidget* editTab = currentContent->onEdit();

    //Вставляем название редактируемой записи в название закладки:
    QString tabName;
    switch (ui->toolBox->currentIndex())
    {
     case 0: //страница "Свои войска"
        tabName = c_battleOrder->getBattleOrderName();
        break;
     case 1: //страница "Поражаемые цели"
        tabName = c_hitTargets->getTargetNumber();
        break;
     case 2: //страница "Команды и сигналы, документы"
        tabName = c_commands->getCommandName();
        break;
     case 3: //страница "Районы и позиции"
        break;
    }

    currentTabWidget->addTab(editTab, tabName);
    currentTabWidget->setCurrentWidget(editTab);
}

//реализация функции "Удалить":
void MainWindow::slotDelete()
{
    switch (ui->toolBox->currentIndex())
    {
     case 0: //страница "Свои войска"

        break;
     case 1: //страница "Поражаемые цели"
        if (c_hitTargets->onDelete())
        {
            if (currentTabWidget->currentIndex() > 1)
            {
                slotOnCloseTab(currentTabWidget->currentIndex());
            }
            else if (currentTabWidget->currentIndex() == 0)
            {
                int index = c_hitTargets->removeFormFromNavigator();
                switch (index)
                {
                case -1:
                    break;
                default:
                    qDebug() << index;
                    currentTabWidget->removeTab(index);
                    break;
                }
            }
            slotNavigator();
        }
        break;
     case 2: //страница "Команды и сигналы, документы"
        if (currentContent->onDelete()) {
                currentContent->fillNavigator();
                currentTabWidget->setCurrentIndex(0);
        }
        break;
     case 3: //страница "Районы и позиции"

        break;
    }
}

//реализация функции "Сохранить"
void MainWindow::slotSave()
{
    if (currentContent->onSave(currentTabWidget->currentIndex()))
    {
        slotOnCloseTab(currentTabWidget->currentIndex());
        slotNavigator();
    }
}

void MainWindow::slotMap()
{
    if (mapProcess->state() != QProcess::Running ) {
        mapProcess->setWorkingDirectory(mapPath + "/BIN");
        mapProcess->start(mapProcess->workingDirectory() + QString("/Karta.exe"));
    }
/*
    QString title1 = "КАРТА-2017 - [Окно Карты" + mapPath + "/maps/100000.rag]";
    LPCWSTR title = (const wchar_t*) title1.utf16();
    HWND hwnd = FindWindow(0,title);
    SetForegroundWindow(hwnd);
*/
}

void MainWindow::slotSend() {
    if (currentContent->onSend()) {
            currentContent->fillNavigator();
            currentTabWidget->setCurrentIndex(0);
            QMessageBox::information(this, "OK", "Отправка прошла успешно");
    }
}

//Изменение ссылок на актуальный класс:
void MainWindow::slotChangeCurrentClass(int index)
{
    ui->m_edit->setEnabled(false);
    ui->m_delete->setEnabled(false);
    ui->m_save->setEnabled(false);
    ui->m_send->setEnabled(false);

    switch (index)
    {
     case 0: //страница "Свои войска"
        currentContent = c_battleOrder;
        currentTabWidget = ui->battleOrderTabWidget;
        break;
     case 1: //страница "Поражаемые цели"
        currentContent = c_hitTargets;
        currentTabWidget = ui->hitTargetsTabWidget;
        break;
     case 2: //страница "Команды и сигналы, документы"
        currentContent = c_commands;
        currentTabWidget = ui->commandsTabWidget;
        break;
     case 3: //страница "Районы и позиции"

        break;
    }
    disconnect(currentTabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotOnChangeTab(int)));
    connect(currentTabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotOnChangeTab(int)));
    slotNavigator();
}

void MainWindow::slotOnChangeTab(int index)
{
    switch (index)
    {
    case 0: //вкладка Навигатор
        ui->m_edit->setEnabled(false);
        ui->m_delete->setEnabled(false);
        ui->m_save->setEnabled(false);
        ui->m_send->setEnabled(false);
        break;
    case 1: //вкладка Изменения
        ui->m_edit->setEnabled(false);
        ui->m_delete->setEnabled(false);
        ui->m_save->setEnabled(false);
        ui->m_send->setEnabled(false);
        break;
    default: //остальные вкладки
        ui->m_edit->setEnabled(false);
        if (ui->toolBox->currentIndex() == 1 && currentTabWidget->tabText(index).contains("Цель"))
        {
            ui->m_delete->setEnabled(true);
        }
        else
        {
            ui->m_delete->setEnabled(false);
        }
        ui->m_save->setEnabled(true);
        ui->m_send->setEnabled(false);
        break;
    }
}

void MainWindow::slotOnCloseTab(int index)
{
    currentContent->removeForm(index);
    currentTabWidget->removeTab(index);
}

void MainWindow::slotOnItemSelected()
{
    ui->m_edit->setEnabled(true);
    ui->m_delete->setEnabled(true);
    ui->m_send->setEnabled(true);
    ui->m_map->setEnabled(true);
}

void MainWindow::finished(int) {
    ui->m_map->setEnabled(false);
}

void MainWindow::serverError(QString str) {
        QMessageBox::critical(0, "Server Error", "Unable to start the server:" + str);
}

void MainWindow::sendNetworkUserMap(QTcpSocket* pSocket) {
    QSqlQuery query = QSqlQuery(db);
    QString selectMapObjects = "SELECT id, code, object_name, geometry_type, enemy, visibility, "
                               "        access_level, object_location, mas_object "
                               "FROM map_objects.object_params";
    if (!query.exec(selectMapObjects)) {
        qDebug() << query.lastError();
        QMessageBox::critical(this, "Ошибка", "Загрузить объекты с БД не удалось!");
    }
    while (query.next()) {
        NetworkObject obj;
        obj.data.m_OBJECT_ID = query.value(0).toInt();
        obj.data.m_CODE = query.value(1).toInt();
        obj.data.m_NAME = query.value(2).toString();
        obj.data.m_TYPE_ID = query.value(3).toInt();
        obj.data.m_ENEMY = query.value(4).toInt();
        obj.data.m_VISIBLE = query.value(5).toBool();
        obj.data.m_ACCESS_LVL = query.value(6).toInt();
        QString rawCoords = query.value(7).toString();
        obj.masObject = query.value(8).toByteArray();

        QSqlQuery coordQuery = QSqlQuery(db);
        QString getParsedCoordinates = "SELECT ST_X(geom), ST_Y(geom), ST_Z(geom) "
                                       "FROM (SELECT (ST_dumppoints(?)).geom "
                                       "      FROM map_objects.object_params) AS foo";
        coordQuery.prepare(getParsedCoordinates);
        coordQuery.addBindValue(rawCoords);
        coordQuery.exec();
        coordQuery.next();

        tagDataPoint metrics;
        metrics.m_LATITUDE = coordQuery.value(0).toDouble();
        metrics.m_LONGITUDE = coordQuery.value(1).toDouble();
        metrics.m_HEIGHT = coordQuery.value(2).toDouble();
        obj.metrics.append(metrics);

        manager.listObject.push_back(obj);
    }
    QByteArray mas = NetworkModule::Instance().maskData(NETWORK_SEND_MAP, manager.serialize());
    NetworkModule::Instance().SendToClient(pSocket, mas);
}

void MainWindow::receiveInsertObjectNetwork(QByteArray& data) {
    qDebug() << "receive object created on map?";
    unsigned char * lp=(unsigned char *)(data.data());
    lp+=2*sizeof(quint32);
    NetworkObject obj;
    obj.deserialize(lp,data.size()-8);

    qDebug() << obj.data.m_CODE;
    qDebug() << obj.data.m_NAME;
    qDebug() << obj.data.m_TYPE_ID;
    qDebug() << obj.data.m_ENEMY;
    qDebug() << obj.data.m_VISIBLE;
    qDebug() << obj.data.m_ACCESS_LVL;
    qDebug() << obj.metrics[0].m_LATITUDE;
    qDebug() << obj.metrics[0].m_LONGITUDE;
    qDebug() << obj.metrics[0].m_HEIGHT;
    qDebug() << obj.metrics[1].m_LATITUDE;
    qDebug() << obj.metrics[1].m_LONGITUDE;
    qDebug() << obj.metrics[1].m_HEIGHT;
    qDebug() << obj.masObject;


    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    QString insertPattern;
    QString insertObjectFromMap;
    QString makePointPattern = "ST_MakePoint(%1, %2, %3)";
    QString makeStartPointString = makePointPattern.arg(obj.metrics[0].m_LATITUDE)
                                                   .arg(obj.metrics[0].m_LONGITUDE)
                                                   .arg(obj.metrics[0].m_HEIGHT);
    switch (obj.data.m_TYPE_ID) {
    case TYPE_METRIC_LINE:
        insertPattern = "INSERT INTO map_objects.object_params (code, object_name, geometry_type, "
                        "       enemy, visibility, access_level, object_location, mas_object) "
                        "VALUES (?, ?, ?, ?, ?, ?, %1, ?)";
        if (obj.metrics.size() == 2) {
            QString makeLinePattern = "ST_MakeLine(ST_MakePoint(%1, %2, %3), ST_MakePoint(%4, %5, %6))";
            QString makeLineString = makeLinePattern.arg(obj.metrics[0].m_LATITUDE)
                                                    .arg(obj.metrics[0].m_LONGITUDE)
                                                    .arg(obj.metrics[0].m_HEIGHT)
                                                    .arg(obj.metrics[1].m_LATITUDE)
                                                    .arg(obj.metrics[1].m_LONGITUDE)
                                                    .arg(obj.metrics[1].m_HEIGHT);
            insertObjectFromMap = insertPattern.arg(makeLineString);
        }
        else if (obj.metrics.size() > 2) {
            QString makePolygonString = "ST_MakePolygon(ST_MakeLine(ARRAY[";
            for (int i = 0; i < obj.metrics.size(); i++) {
                QString makePointString = makePointPattern.arg(obj.metrics[i].m_LATITUDE)
                                                          .arg(obj.metrics[i].m_LONGITUDE)
                                                          .arg(obj.metrics[i].m_HEIGHT);
                makePolygonString.append(makePointString).append(", ");
            }
            makePolygonString.append(makeStartPointString).append("]))");
            insertObjectFromMap = insertPattern.arg(makePolygonString);
        }
        query.prepare(insertObjectFromMap);
        query.addBindValue(obj.data.m_CODE);
        query.addBindValue(obj.data.m_NAME);
        query.addBindValue(obj.data.m_TYPE_ID);
        query.addBindValue(obj.data.m_ENEMY);
        query.addBindValue(obj.data.m_VISIBLE);
        query.addBindValue(obj.data.m_ACCESS_LVL);
        query.addBindValue(obj.masObject);
        break;

    case TYPE_METRIC_RECT:
        insertPattern = "INSERT INTO map_objects.object_params (code, object_name, geometry_type, enemy, "
                        "       visibility, access_level, object_location, front, depth, deviation, mas_object) "
                        "VALUES (?, ?, ?, ?, ?, ?, %1, ?, ?, ?, ?)";
        insertObjectFromMap = insertPattern.arg(makeStartPointString);

        query.prepare(insertObjectFromMap);
        query.addBindValue(obj.data.m_CODE);
        query.addBindValue(obj.data.m_NAME);
        query.addBindValue(obj.data.m_TYPE_ID);
        query.addBindValue(obj.data.m_ENEMY);
        query.addBindValue(obj.data.m_VISIBLE);
        query.addBindValue(obj.data.m_ACCESS_LVL);
        query.addBindValue(obj.metrics[1].m_LATITUDE);
        query.addBindValue(obj.metrics[1].m_LONGITUDE);
        query.addBindValue(obj.metrics[1].m_HEIGHT);
        query.addBindValue(obj.masObject);
        break;

    case TYPE_METRIC_CIRCLE:
        insertPattern = "INSERT INTO map_objects.object_params (code, object_name, geometry_type, enemy, "
                        "       visibility, access_level, object_location, radius, mas_object) "
                        "VALUES (?, ?, ?, ?, ?, ?, %1, ?, ?)";
        insertObjectFromMap = insertPattern.arg(makeStartPointString);

        query.prepare(insertObjectFromMap);
        query.addBindValue(obj.data.m_CODE);
        query.addBindValue(obj.data.m_NAME);
        query.addBindValue(obj.data.m_TYPE_ID);
        query.addBindValue(obj.data.m_ENEMY);
        query.addBindValue(obj.data.m_VISIBLE);
        query.addBindValue(obj.data.m_ACCESS_LVL);
        query.addBindValue(obj.metrics[1].m_LATITUDE);
        query.addBindValue(obj.masObject);
        break;
    }
    if (!query.exec()) {
        qDebug() << query.lastError();
        QMessageBox::critical(this, "Ошибка", "Добавить объект не удалось!");
    }


//    QString selectId = "SELECT id "
//                       "FROM map_objects.object_params "
//                       "WHERE mas_object = ?";

//    query.prepare(selectId);
//    query.addBindValue(obj.masObject);
//    if (!query.exec()) {
//        qDebug() << query.lastError();
//        QMessageBox::critical(this, "Ошибка", "Получить ID объекта не удалось!");
//    }
//    query.next();
//    obj.data.m_OBJECT_ID = query.value(0).toInt();
//    qDebug() << "ID" << obj.data.m_OBJECT_ID;
//    manager.listObject.push_back(obj);

    db.commit();

//    QByteArray mas = NetworkModule::Instance().maskData(NETWORK_OBJECT_SET_ID, obj.serialize());
//    NetworkModule::Instance().sendDataFromMap(mas);
}

void MainWindow::receiveInsertObjectNetworkFromDB(QByteArray& data)
{
    qDebug() << "object back to server with mas_object";

    unsigned char * lp=(unsigned char *)(data.data());
    lp+=2*sizeof(quint32);
    NetworkObject obj;
    obj.deserialize(lp,data.size()-8);

    qDebug() << obj.metrics[0].m_LATITUDE;
    qDebug() << obj.metrics[0].m_LONGITUDE;
    qDebug() << obj.metrics[0].m_HEIGHT;

//    qDebug() << obj.metrics[1].m_LATITUDE;
//    qDebug() << obj.metrics[1].m_LONGITUDE;
//    qDebug() << obj.metrics[1].m_HEIGHT;

    qDebug() << "ID" << obj.data.m_OBJECT_ID;
    qDebug() << obj.data.m_CODE;
    qDebug() << obj.data.m_NAME;
    qDebug() << obj.data.m_TYPE_ID;
    qDebug() << obj.data.m_ENEMY;
    qDebug() << obj.data.m_VISIBLE;
    qDebug() << obj.data.m_ACCESS_LVL;
    qDebug() << obj.masObject;

    QSqlQuery query = QSqlQuery(db);
    QString updateMasObject = "UPDATE map_objects.object_params "
                              "SET mas_object = ? "
                              "WHERE id = ?";
    query.prepare(updateMasObject);
    query.addBindValue(obj.masObject);
    query.addBindValue(obj.data.m_OBJECT_ID);
    if (!query.exec()) {
        qDebug() << query.lastError();
        QMessageBox::critical(this, "Ошибка", "Добавить объект не удалось!");
    }
    manager.listObject.push_back(obj);
}


void MainWindow::receiveDeleteObjectNetwork(QByteArray& mas)
{
    QVector<int> masId;

    unsigned char * lp=(unsigned char *)(mas.data());
    lp+=2*sizeof(quint32);

    quint32 count;
    memcpy(&count,lp,sizeof(quint32));
    lp+=sizeof(quint32);

    for(quint32 i=0;i<count;i++)
    {
        quint32 index;
        memcpy(&index,lp,sizeof(quint32));
        lp+=sizeof(quint32);
        masId.push_back(index);
    }

    for(int i=0;i<masId.size();i++)
    {
        for(int j=0;j<manager.listObject.size();j++)
        {
            if(masId[i]==manager.listObject[j].data.m_OBJECT_ID)
            {
                QSqlQuery query;
                QString deleteObject = "DELETE FROM map_objects.object_params WHERE id = ?";
                QString deleteObject2 = "UPDATE map_objects.object_params SET visibility = false WHERE id = ?";
                query.prepare(deleteObject2);
                query.addBindValue(manager.listObject[j].data.m_OBJECT_ID);
                if (!query.exec()) {
                    qDebug() << query.lastError();
                    QMessageBox::critical(this, "Ошибка", "Удалить объект не удалось!");
                }
                manager.listObject.remove(j);
                break;
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete c_battleOrder;
    delete c_hitTargets;
    delete c_commands;
    delete ui;
}
