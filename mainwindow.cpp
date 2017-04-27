#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QSqlDatabase DB, QWidget *parent) :
    db(DB),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    c_battleOrder = new BattleOrder(db,ui->navigatorBattleOrderTree,ui->navigatorBattleOrderRecivers,
                                    ui->changesBattleOrderTable,ui->changesBattleOrderRecivers);
    c_hitTargets  = new HitTargets(db, ui->navigatorHitTargetsTable, ui->navigatorHitTargetsRecivers,
                                   ui->changesHitTargetsTable, ui->changesHitTargetsRecivers);
    c_commands    = new Commands(db,ui->navigatorCommandsTree, ui->navigatorCommandsRecivers,
                                 ui->changesCommandsTree, ui->changesCommandsRecivers);

    currentContent = c_battleOrder;
    currentTabWidget = ui->battleOrderTabWidget;

    ui->toolBox->setCurrentIndex(0);
    slotNavigator();

    connect( ui->toolBox,     SIGNAL (currentChanged(int)), this, SLOT (slotChangeCurrentClass()) );
    connect( ui->m_navigator, SIGNAL (triggered()),         this, SLOT (slotNavigator())          );
    connect( ui->m_changes,   SIGNAL (triggered()),         this, SLOT (slotChanges())            );
    connect( ui->m_add,       SIGNAL (triggered()),         this, SLOT (slotAdd())                );
    connect( ui->m_edit,      SIGNAL (triggered()),         this, SLOT (slotEdit())               );
    connect( ui->m_delete,    SIGNAL (triggered()),         this, SLOT (slotDelete())             );
    connect( ui->m_save,      SIGNAL (triggered()),         this, SLOT (slotSave())               );
    connect( ui->m_exit,      SIGNAL (triggered()),         qApp, SLOT (quit())                   );
}

//заполнение закладки "Навигатор":
void MainWindow::slotNavigator()
{
    //включение/выключение кнопок:
    ui->m_navigator->setChecked(true);
    ui->m_changes->setChecked(false);
    currentContent->fillNavigator();
    currentTabWidget->setCurrentIndex(0);
}

//заполнение закладки "Изменить":
void MainWindow::slotChanges()
{
    //включение/выключение кнопок:
    ui->m_navigator->setChecked(false);
    ui->m_changes->setChecked(true);
    currentContent->fillChanges();
    currentTabWidget->setCurrentIndex(1);
}

//создвние закладки "Новый":
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

//Изменение ссылок на актуальный класс:
void MainWindow::slotChangeCurrentClass()
{
    switch ( ui->toolBox->currentIndex() )
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


