#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtSql>

#include "battleOrder.h"
#include "hitTargets.h"

namespace Ui {
class MainWindow2;
}

class BattleOrder;
class HitTargets;
/*
class Commands;
class Positions;
class ReceiversDialog;
*/

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:  
    explicit MainWindow(QSqlDatabase DB = QSqlDatabase::database(), QWidget *parent = 0);
    ~MainWindow();

    QSqlDatabase db;    
    BattleOrder *c_battleOrder = new BattleOrder(db);
    HitTargets  *c_hitTargets  = new HitTargets(db);


private:
    Ui::MainWindow2* ui;

private slots:
    void slotNavigator(); //заполнение закладки "Навигатор"
    void slotChanges();   //заполнение закладки "Изменить"

};

#endif // MAINWINDOW_H
