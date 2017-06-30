#include "mainwindow.h"

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
}

MainWindow::~MainWindow()
{
    delete c_battleOrder;
    delete c_hitTargets;
    delete c_commands;
    delete ui;
}
