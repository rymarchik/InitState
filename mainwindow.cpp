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
    c_battleOrder = new BattleOrder(db);
    c_hitTargets  = new HitTargets(db, ui->navigatorHitTargetsTable, ui->navigatorHitTargetsRecivers);
    c_commands    = new Commands(db,ui->navigatorCommandsTree, ui->navigatorCommandsRecivers);

    ui->toolBox->setCurrentIndex(0);
    slotNavigator();

    connect( ui->toolBox,     SIGNAL (currentChanged(int)), this, SLOT (slotNavigator()) );
    connect( ui->m_navigator, SIGNAL (triggered()),         this, SLOT (slotNavigator()) );
    connect( ui->m_changes,   SIGNAL (triggered()),         this, SLOT (slotChanges())   );
    connect( ui->m_add,       SIGNAL (triggered()),         this, SLOT (slotAdd())       );
    connect( ui->m_edit,      SIGNAL (triggered()),         this, SLOT (slotEdit())      );
    connect( ui->m_delete,    SIGNAL (triggered()),         this, SLOT (slotDelete())    );
    connect( ui->m_save,      SIGNAL (triggered()),         this, SLOT (slotSave())      );
    connect( ui->m_exit,      SIGNAL (triggered()),         qApp, SLOT(quit())           );
}

//заполнение закладки "Навигатор":
void MainWindow::slotNavigator()
{
    //включение/выключение кнопок:
    ui->m_navigator->setChecked(true);
    ui->m_changes->setChecked(false);

    switch ( ui->toolBox->currentIndex() )
    {
     case 0: //страница "Свои войска"
        ui->battleOrderTabWidget->setTabEnabled(0, true);
        ui->battleOrderTabWidget->setTabEnabled(1, false);
        c_battleOrder->fillNavigator(ui->navigatorBattleOrderTree);
        break;
     case 1: //страница "Поражаемые цели"
        ui->hitTargetsTabWidget->setTabEnabled(0, true);
        ui->hitTargetsTabWidget->setTabEnabled(1, false);
        c_hitTargets->fillNavigator(ui->navigatorHitTargetsTable);
        break;
     case 2: //страница "Команды и сигналы, документы"
        ui->commandsTabWidget->setTabEnabled(0, true);
        ui->commandsTabWidget->setTabEnabled(1, false);
        c_commands->fillNavigator();

        break;
     case 3: //страница "Районы и позиции"
        ui->positionsTabWidget->setTabEnabled(0, true);
        ui->positionsTabWidget->setTabEnabled(1, false);

        break;
     case 4: //страница "Тактическая обстановка"
        ui->tacticalTabWidget->setTabEnabled(0, true);
        ui->tacticalTabWidget->setTabEnabled(1, false);

        break;
     case 5: //страница "Условия ведения боевых действий"
        ui->conditionsTabWidget->setTabEnabled(0, true);
        ui->conditionsTabWidget->setTabEnabled(1, false);
        break;
    }
}

//заполнение закладки "Изменить":
void MainWindow::slotChanges()
{
    //включение/выключение кнопок:
    ui->m_navigator->setChecked(false);
    ui->m_changes->setChecked(true);

    switch ( ui->toolBox->currentIndex() )
    {
     case 0: //страница "Свои войска"
        ui->battleOrderTabWidget->setTabEnabled(0, false);
        ui->battleOrderTabWidget->setTabEnabled(1, true);
        //c_battleOrder->fillChanges(ui->changesBattleOrderTable);
        break;
     case 1: //страница "Поражаемые цели"
        ui->hitTargetsTabWidget->setTabEnabled(0, false);
        ui->hitTargetsTabWidget->setTabEnabled(1, true);
        break;
     case 2: //страница "Команды и сигналы, документы"
        ui->commandsTabWidget->setTabEnabled(0, false);
        ui->commandsTabWidget->setTabEnabled(1, true);
        break;
     case 3: //страница "Районы и позиции"
        ui->positionsTabWidget->setTabEnabled(0, false);
        ui->positionsTabWidget->setTabEnabled(1, true);
        break;
     case 4: //страница "Тактическая обстановка"
        ui->tacticalTabWidget->setTabEnabled(0, false);
        ui->tacticalTabWidget->setTabEnabled(1, true);
        break;
     case 5: //страница "Условия ведения боевых действий"
        ui->conditionsTabWidget->setTabEnabled(0, false);
        ui->conditionsTabWidget->setTabEnabled(1, true);
        break;
    }
}

//создвние закладки "Новый":
void MainWindow::slotAdd()
{
    qDebug()<< "slotNavigator,tool:" << ui->toolBox->currentIndex();
    switch ( ui->toolBox->currentIndex() )
    {
     case 0: //страница "Свои войска"
        break;
     case 1: //страница "Поражаемые цели"
        ui->hitTargetsTabWidget->addTab(c_hitTargets->onAdd(),"Новый");
        ui->hitTargetsTabWidget->setCurrentIndex(2);
        break;
     case 2: //страница "Команды и сигналы, документы"
        ui->commandsTabWidget->addTab(c_commands->onAdd(),"Новый");
        ui->commandsTabWidget->setCurrentIndex(2);
        break;
     case 3: //страница "Районы и позиции"

        break;
     case 4: //страница "Тактическая обстановка"

        break;
     case 5: //страница "Условия ведения боевых действий"
        break;
    }
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

MainWindow::~MainWindow()
{
    delete c_battleOrder;
    delete c_hitTargets;
    delete ui;
}


