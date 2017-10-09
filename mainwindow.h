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

#include <QProcess>
#include <QTcpSocket>
#include "mapsrc/NetworkObjectManager.h"
#include "mapsrc/networkmodule.h"
#include "mapsrc/PropertyObj.h"

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

    QProcess      *mapProcess;
    NetworkObjectManager manager;

    QString mapPath = QCoreApplication::applicationDirPath() + "/mapFiles";

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
    void slotSend();

    void slotOnChangeTab(int);
    void slotOnCloseTab(int);
    void slotOnItemSelected();

    void finished(int);
    void serverError(QString);
    void sendNetworkUserMap(QTcpSocket* pSocket);
    void receiveInsertObjectNetwork(QByteArray&);
    void receiveInsertObjectNetworkFromDB(QByteArray&);

    /**
    \fn   receiveDeleteObjectNetwork
    Слот для обработки сигнала о приеме пакета данных, в котором хранятся идентификаторы удаленных объектов
    \param[in] QByteArray & mas - ссылка пакет данных, в который был упакован список идентификаторов удаленных объектов
    \return не возвращает
    */
    void receiveDeleteObjectNetwork(QByteArray&);
};

#endif // MAINWINDOW_H
