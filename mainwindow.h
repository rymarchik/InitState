#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
//#include <QUdpSocket>
#include <QtSql>
#include <QDebug>

#include "ui_mainwindow.h"
#include "basetoolclass.h"
#include "battleOrder.h"
#include "hitTargets.h"
#include "commands.h"
#include "gpsModule.h"
#include "mapModule.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:  
    explicit MainWindow(QSqlDatabase DB = QSqlDatabase::database(), int UserID = 0, QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSqlDatabase  db;
    int idUser = 0;

    QString title = "Исходная обстановка";
    QString objectName;
    QString combatHierarchy;
    QString currentMode;
    QString currentModeName;

    BaseToolClass *currentContent;
    QTabWidget    *currentTabWidget;

    BattleOrder   *c_battleOrder;
    HitTargets    *c_hitTargets;
    Commands      *c_commands;
    GPSModule     *gps;
    MapModule     *map;

    void windowsTitle(int idUser);  //Формирование названия формы

private slots:
    void slotChangeCurrentClass(int);
    void slotNavigator(); //заполнение закладки "Навигатор"
    void slotChanges();   //заполнение закладки "Изменения"
    void slotAdd();       //создание закладки "Новый"
    void slotEdit();      //реализация функции "Править"
    void slotDelete();    //реализация функции "Удалить"
    void slotSave();      //реализация функции "Сохранить"
    void slotMap();
    void slotMapClose();
    void slotSend();

    void slotOnChangeTab(int);
    void slotOnCloseTab(int);
    void slotOnItemSelected();
};

#endif // MAINWINDOW_H
