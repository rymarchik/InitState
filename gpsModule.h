#ifndef GPSMODULE_H
#define GPSMODULE_H

#include <QObject>
#include <QtSql>
#include <QSerialPort>
#include <QSerialPortInfo>

class GPSModule : public QObject
{
    Q_OBJECT
public:
    explicit GPSModule(QSqlDatabase db, QString combatHierarchy, QObject *parent = 0);

    void updateDatabaseGeoInfo(double, double, double);
    int getLastDirectionValue();

private:
    QSqlDatabase db;
    QString combatHierarchy;
    QSerialPort* serial;
    QTimer* readPortTimer;

private slots:
    void slotParseInput();
};

#endif // GPSMODULE_H
