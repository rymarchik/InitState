#include "mainwindow.h"
#include <QApplication>
//#include "socketapi.h"
#include <QtSql>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);

    QApplication a(argc, argv);

    int UserID = 0;
    for (int i=0; i<argc; i++)
    {
        UserID = QString(argv[i]).toInt();
    }

    QSqlDatabase DB = QSqlDatabase::addDatabase("QPSQL");
    DB.setDatabaseName("A200");
    DB.setUserName("postgres");
    DB.setPassword("qwerty");
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

    MainWindow w(DB, UserID);
    w.show();

    return a.exec();
}
