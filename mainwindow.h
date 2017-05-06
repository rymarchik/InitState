#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtSql>

#include "basetoolclass.h"
#include "battleOrder.h"
#include "hitTargets.h"
#include "commands.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:  
    explicit MainWindow(QSqlDatabase DB = QSqlDatabase::database(), QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    QSqlDatabase db;
    BaseToolClass *currentContent;
    QTabWidget    *currentTabWidget;
    BattleOrder   *c_battleOrder;
    HitTargets    *c_hitTargets;
    Commands      *c_commands;

private slots:
    void slotChangeCurrentClass(int);
    void slotNavigator(); //заполнение закладки "Навигатор"
    void slotChanges();   //заполнение закладки "Изменить"
    void slotAdd();       //создание закладки "Новый"
    void slotEdit();      //реализация функции "Править"
    void slotDelete();    //реализация функции "Удалить"
    void slotSave();      //реализация функции "Сохранить"
    void slotMap();
    void slotSend();

    void slotOnChangeTab(int);
    void slotOnCloseTab(int);
    void slotOnItemSelected();
};

#endif // MAINWINDOW_H
