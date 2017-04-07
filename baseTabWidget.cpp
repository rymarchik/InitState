#include "baseTabWidget.h"
#include "utility.h"

BaseTabWidget::BaseTabWidget(QWidget *parent) : QTabWidget(parent)
{
    navigatorUpperTable = new QTableWidget;
    navigatorLowerTable = new QTableWidget;
    changesUpperTable = new QTableWidget;
    changesLowerTable = new QTableWidget;

    Utility::initializeTableSettings(navigatorUpperTable);
    Utility::initializeTableSettings(navigatorLowerTable);
    Utility::initializeTableSettings(changesUpperTable);
    Utility::initializeTableSettings(changesLowerTable);

    navigatorTabLayout = new QVBoxLayout;
    navigatorTabLayout->addWidget(navigatorUpperTable);
    navigatorTabLayout->addWidget(navigatorLowerTable);

    navigatorTabWidget = new QWidget;
    navigatorTabWidget->setLayout(navigatorTabLayout);

    changesTabLayout = new QVBoxLayout;
    changesTabLayout->addWidget(changesUpperTable);
    changesTabLayout->addWidget(changesLowerTable);

    changesTabWidget = new QWidget;
    changesTabWidget->setLayout(changesTabLayout);

    setHidden(true);
    setTabsClosable(true);
    font.setBold(true);
    font.setPointSize(9);
}

QTableWidget* BaseTabWidget::getNavigatorUpperTable() {
    return navigatorUpperTable;
}

void BaseTabWidget::setNavigatorTab() {
    this->removeTab(0);
    this->insertTab(0, navigatorTabWidget, "Навигатор");
    this->setCurrentWidget(navigatorTabWidget);
    this->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
}

void BaseTabWidget::setChangesTab() {
    this->removeTab(0);
    this->insertTab(0, changesTabWidget, "Изменения");
    this->setCurrentWidget(changesTabWidget);
    this->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
}
