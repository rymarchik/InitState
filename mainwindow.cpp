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

    gps = new GPSModule(db, combatHierarchy, this);
    map = new MapModule(db, this);

    ui->battleOrderTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    ui->battleOrderTabWidget->tabBar()->tabButton(1, QTabBar::RightSide)->resize(0, 0);
    ui->hitTargetsTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    ui->hitTargetsTabWidget->tabBar()->tabButton(1, QTabBar::RightSide)->resize(0, 0);
    ui->commandsTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    ui->commandsTabWidget->tabBar()->tabButton(1, QTabBar::RightSide)->resize(0, 0);

    c_battleOrder = new BattleOrder(db, combatHierarchy, currentMode, ui->navigatorBattleOrderTree, ui->navigatorBattleOrderReceivers,
                                    ui->changesBattleOrderTable, ui->changesBattleOrderReceivers, map);

    c_hitTargets  = new HitTargets(db, ui->navigatorHitTargetsTable, ui->navigatorHitTargetsReceivers,
                                   ui->changesHitTargetsTable, ui->changesHitTargetsReceivers, map);
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
    connect(map->getMapProcess(), SIGNAL(finished(int)), this, SLOT(slotMapClose()));
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
    map->launchMap();
/*
    QString title1 = "КАРТА-2017 - [Окно Карты" + map->getMapPath() + "/maps/100000.rag]";
    LPCWSTR title = (const wchar_t*) title1.utf16();
    HWND hwnd = FindWindow(0,title);
    SetForegroundWindow(hwnd);
*/
}

void MainWindow::slotMapClose()
{
    ui->m_map->setEnabled(false);
    map->clearObjectManager();
}

void MainWindow::slotSend()
{
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


MainWindow::~MainWindow()
{
    delete c_battleOrder;
    delete c_hitTargets;
    delete c_commands;
    delete ui;
}
