#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "ui_mainwindow2.h"
#include <QDebug>


MainWindow::MainWindow(QSqlDatabase DB, QWidget *parent) :
    db(DB),
    QMainWindow(parent),
    ui(new Ui::MainWindow2)
{
    ui->setupUi(this);

    c_battleOrder = new BattleOrder(db,ui->navigatorBattleOrderTree,ui->navigatorBattleOrderReceivers,
                                    ui->changesBattleOrderTable,ui->changesBattleOrderReceivers);
    c_hitTargets  = new HitTargets(db, ui->navigatorHitTargetsTable, ui->navigatorHitTargetsReceivers,
                                   ui->changesHitTargetsTable, ui->changesHitTargetsReceivers);
    c_commands    = new Commands(db,ui->navigatorCommandsTree, ui->navigatorCommandsReceivers,
                                 ui->changesCommandsTree, ui->changesCommandsReceivers);

    currentContent = c_battleOrder;
    currentTabWidget = ui->battleOrderTabWidget;

    ui->toolBox->setCurrentIndex(0);
    slotNavigator();

    connect(ui->toolBox, SIGNAL(currentChanged(int)), this, SLOT(slotChangeCurrentClass(int)));
    connect(ui->m_exit, SIGNAL(triggered()), qApp, SLOT(quit()));
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
    currentTabWidget->addTab(currentContent->onAdd(),"Новый");
    currentTabWidget->setCurrentIndex(2);
}

//реализация функции "Править":
void MainWindow::slotEdit()
{

}

//реализация функции "Удалить":
void MainWindow::slotDelete()
{

}

//реализация функции "Сохранить"
void MainWindow::slotSave()
{

}

void MainWindow::slotMap() {

}

void MainWindow::slotSend() {

}

//Изменение ссылок на актуальный класс:
void MainWindow::slotChangeCurrentClass(int index)
{
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
     case 4: //страница "Тактическая обстановка"

        break;
     case 5: //страница "Условия ведения боевых действий"

        break;
    }
    slotNavigator();
}

MainWindow::~MainWindow()
{
    delete c_battleOrder;
    delete c_hitTargets;
    delete c_commands;
    delete ui;
}


