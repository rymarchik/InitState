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

    ui->toolBox->setCurrentIndex(0);
    slotNavigator();
    c_commands->setTables(ui->navigatorCommandsTree, ui->navigatorCommandsRecivers);

    connect( ui->toolBox,     SIGNAL (currentChanged(int)), this, SLOT (slotNavigator()) );
    connect( ui->m_navigator, SIGNAL (triggered()),         this, SLOT (slotNavigator()) );
    connect( ui->m_changes,   SIGNAL (triggered()),         this, SLOT (slotChanges())   );
    connect( ui->m_add,       SIGNAL (triggered()),         this, SLOT (slotAdd())       );
    connect( ui->m_exit,      SIGNAL (triggered()),         qApp, SLOT(quit())           );

    //connect( ui->m_Refresh, SIGNAL (triggered()), this, SLOT (slotRefresh()) );
}

//заполнение закладки "Навигатор":
void MainWindow::slotNavigator()
{
    //включение/выключение кнопок:
    ui->m_navigator->setChecked(true);
    ui->m_changes->setChecked(false);

    qDebug()<< "slotNavigator,tool:" << ui->toolBox->currentIndex();

    switch ( ui->toolBox->currentIndex() )
    {
     case 0: //страница "Свои войска"
        ui->tabWidget_0->setTabEnabled(1, false);
        ui->tabWidget_0->setTabEnabled(0, true);
        c_battleOrder->fillNavigator(ui->navigatorBattleOrder);
        break;
     case 1: //страница "Поражаемые цели"
        ui->tabWidget_1->setTabEnabled(1, false);
        ui->tabWidget_1->setTabEnabled(0, true);
        c_hitTargets->fillNavigator(ui->navigatorHitTargets);
        break;
     case 2: //страница "Команды и сигналы, документы"
        ui->tabWidget_2->setTabEnabled(1, false);
        ui->tabWidget_2->setTabEnabled(0, true);
        c_commands->fillNavigator();

        break;
     case 3: //страница "Районы и позиции"
        ui->tabWidget_3->setTabEnabled(1, false);
        ui->tabWidget_3->setTabEnabled(0, true);

        break;
     case 4: //страница "Тактическая обстановка"
        ui->tabWidget_4->setTabEnabled(1, false);
        ui->tabWidget_4->setTabEnabled(0, true);

        break;
     case 5: //страница "Условия ведения боевых действий"
        ui->tabWidget_5->setTabEnabled(1, false);
        ui->tabWidget_5->setTabEnabled(0, true);
        //ui->tabWidget_5->setCurrentWidget( ui->tabNavigator_5 );
        break;
    }
}

//заполнение закладки "Изменить":
void MainWindow::slotChanges()
{
    //включение/выключение кнопок:
    ui->m_navigator->setChecked(false);
    ui->m_changes->setChecked(true);

    qDebug()<< "slotChanges,tool:" << ui->toolBox->currentIndex();
    //qDebug()<< "slotChanges,tab:"  << ui->tabWidget_0->currentIndex();

    switch ( ui->toolBox->currentIndex() )
    {
     case 0: //страница "Свои войска"
        ui->tabWidget_0->setTabEnabled(0, false);
        ui->tabWidget_0->setTabEnabled(1, true);

        ui->tabWidget_0->setCurrentWidget( ui->tabChanges_0 );
        c_battleOrder->fillChanges(ui->changesBattleOrder);

        break;
     case 1: //страница "Поражаемые цели"
        ui->tabWidget_1->setCurrentWidget( ui->tabChanges_1 );
        break;
     case 2: //страница "Команды и сигналы, документы"
        ui->tabWidget_2->setCurrentWidget( ui->tabChanges_2 );
        break;
     case 3: //страница "Районы и позиции"
        ui->tabWidget_3->setCurrentWidget( ui->tabChanges_3 );
        break;
     case 4: //страница "Тактическая обстановка"
        ui->tabWidget_4->setCurrentWidget( ui->tabChanges_4 );
        break;
     case 5: //страница "Условия ведения боевых действий"
        ui->tabWidget_5->setCurrentWidget( ui->tabChanges_5 );
        break;
    }
}

void MainWindow::slotAdd()
{
    qDebug()<< "slotNavigator,tool:" << ui->toolBox->currentIndex();
    switch ( ui->toolBox->currentIndex() )
    {
     case 0: //страница "Свои войска"
        ui->tabWidget_0->setTabEnabled(1, false);
        ui->tabWidget_0->setTabEnabled(0, true);
        c_battleOrder->fillNavigator(ui->navigatorBattleOrder);
        break;
     case 1: //страница "Поражаемые цели"
        ui->tabWidget_1->setTabEnabled(1, false);
        ui->tabWidget_1->setTabEnabled(0, true);
        c_hitTargets->fillNavigator(ui->navigatorHitTargets);
        break;
     case 2: //страница "Команды и сигналы, документы"
        ui->tabWidget_2->setTabEnabled(1, false);
        ui->tabWidget_2->setTabEnabled(0, true);
        ui->tabWidget_2->addTab(c_commands->onAdd(),"Новый");
        break;
     case 3: //страница "Районы и позиции"
        ui->tabWidget_3->setTabEnabled(1, false);
        ui->tabWidget_3->setTabEnabled(0, true);

        break;
     case 4: //страница "Тактическая обстановка"
        ui->tabWidget_4->setTabEnabled(1, false);
        ui->tabWidget_4->setTabEnabled(0, true);

        break;
     case 5: //страница "Условия ведения боевых действий"
        ui->tabWidget_5->setTabEnabled(1, false);
        ui->tabWidget_5->setTabEnabled(0, true);
        //ui->tabWidget_5->setCurrentWidget( ui->tabNavigator_5 );
        break;
    }
}

MainWindow::~MainWindow()
{
    delete c_battleOrder;
    delete c_hitTargets;
    delete ui;
}


