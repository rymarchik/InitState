#include "mainwindow.h"
#include <QApplication>
#include <QtSql>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase DB = QSqlDatabase::addDatabase("QPSQL");
    DB.setDatabaseName("A200");
    DB.setUserName("asrymarchik");
    DB.setPassword("123456");
    //DB.setHostName("localhost");
    DB.setHostName("192.168.1.42");

    if (!DB.open())
    {
        qDebug() << "Сообщение: " << DB.lastError().text();
        return 0;
    }
    else
    {
        qDebug() << "Success!";
    }

    MainWindow w(DB);
    w.show();

    return a.exec();
}
