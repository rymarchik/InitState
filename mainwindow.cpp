#include "mainwindow.h"

#include "mapsrc/networkmodule.h"
#include "mapsrc/PropertyObj.h"

MainWindow::MainWindow(QSqlDatabase DB, QWidget *parent) :
    db(DB),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->battleOrderTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    ui->battleOrderTabWidget->tabBar()->tabButton(1, QTabBar::RightSide)->resize(0, 0);
    ui->hitTargetsTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    ui->hitTargetsTabWidget->tabBar()->tabButton(1, QTabBar::RightSide)->resize(0, 0);
    ui->commandsTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    ui->commandsTabWidget->tabBar()->tabButton(1, QTabBar::RightSide)->resize(0, 0);

    c_battleOrder = new BattleOrder(db, ui->navigatorBattleOrderTree, ui->navigatorBattleOrderReceivers,
                                    ui->changesBattleOrderTable, ui->changesBattleOrderReceivers);
    c_hitTargets  = new HitTargets(db, ui->navigatorHitTargetsTable, ui->navigatorHitTargetsReceivers,
                                   ui->changesHitTargetsTable, ui->changesHitTargetsReceivers);
    c_commands    = new Commands(db, ui->navigatorCommandsTree, ui->navigatorCommandsReceivers,
                                 ui->changesCommandsTree, ui->changesCommandsReceivers);

    currentContent  = c_battleOrder;
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
    connect(mapProcess, SIGNAL(finished(int)), SLOT(finished(int)));
    connect(&NetworkModule::Instance(), SIGNAL(serverError(QString)), SLOT(serverError(QString)));
    connect(&NetworkModule::Instance(), SIGNAL(sendNetworkUserMap(QTcpSocket*)), SLOT(sendNetworkUserMap(QTcpSocket*)));
    connect(&NetworkModule::Instance(), SIGNAL(receiveInsertObjectNetwork(QByteArray&)), SLOT(receiveInsertObjectNetwork(QByteArray&)));
    connect(&NetworkModule::Instance(), SIGNAL(receiveDeleteObjectNetwork(QByteArray&)), SLOT(receiveDeleteObjectNetwork(QByteArray&)));
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
        mapProcess->setWorkingDirectory(mapPath);
        mapProcess->start(mapPath + QString("/Karta.exe"));
    }

    QString title1 = "КАРТА-2017 - [Окно КартыD:/Volat/Qt/KARTA/sample/maps/100000.rag]";
    LPCWSTR title = (const wchar_t*) title1.utf16();
    HWND hwnd = FindWindow(0,title);
    SetForegroundWindow(hwnd);
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
        ui->m_delete->setEnabled(false);
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
    QString selectMapObjects = "SELECT id, code, object_name, geometry_type, enemy, visible, "
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
    unsigned char * lp=(unsigned char *)(data.data());
    lp+=2*sizeof(quint32);
    NetworkObject obj;
    obj.deserialize(lp,data.size()-8);

    QString makePointPattern = "ST_MakePoint(%1, %2, %3)";
    QString makePointString = makePointPattern.arg(obj.metrics[0].m_LATITUDE).arg(obj.metrics[0].m_LONGITUDE)
            .arg(obj.metrics[0].m_HEIGHT);

    QString insertPattern = "INSERT INTO map_objects.object_params (code, object_name, geometry_type, "
                            "       enemy, visible, access_level, object_location, mas_object) "
                            "VALUES (?, ?, ?, ?, ?, ?, %1, ?)";
    QString insertObjectFromMap = insertPattern.arg(makePointString);

    QString selectId = "SELECT id "
                       "FROM map_objects.object_params "
                       "WHERE mas_object = ?";

    QSqlQuery query = QSqlQuery(db);
    db.transaction();

    query.prepare(insertObjectFromMap);
    query.addBindValue(obj.data.m_CODE);
    query.addBindValue(obj.data.m_NAME);
    query.addBindValue(obj.data.m_TYPE_ID);
    query.addBindValue(obj.data.m_ENEMY);
    query.addBindValue(obj.data.m_VISIBLE);
    query.addBindValue(obj.data.m_ACCESS_LVL);
    query.addBindValue(obj.masObject);
    if (!query.exec()) {
        qDebug() << query.lastError();
        QMessageBox::critical(this, "Ошибка", "Добавить объект не удалось!");
    }

    query.prepare(selectId);
    query.addBindValue(obj.masObject);
    if (!query.exec()) {
        qDebug() << query.lastError();
        QMessageBox::critical(this, "Ошибка", "Получить ID объекта не удалось!");
    }
    query.next();
    obj.data.m_OBJECT_ID = query.value(0).toInt();
    qDebug() << "ID" << obj.data.m_OBJECT_ID;
    manager.listObject.push_back(obj);

    db.commit();

    QByteArray mas = NetworkModule::Instance().maskData(NETWORK_OBJECT_SET_ID, obj.serialize());
    NetworkModule::Instance().sendDataFromMap(mas);
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
                QString deleteObject2 = "UPDATE map_objects.object_params SET visible = false WHERE id = ?";
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
