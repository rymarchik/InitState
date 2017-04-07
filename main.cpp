#include "mainwindow.h"
#include <QApplication>
#include <QtSql>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase DB = QSqlDatabase::addDatabase("QPSQL");
    DB.setDatabaseName("A200");
    //db.setUserName("asrymarchik");
    //db.setPassword("123456");
    //db.setHostName("192.168.1.42");
    DB.setUserName("postgres");
    DB.setPassword("qwerty");
    DB.setHostName("localhost");
    DB.setPort(5432);

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
