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
    explicit GPSModule(QSqlDatabase db, QObject *parent = 0);

private:
    QSqlDatabase db;
    QSerialPort* serial;
    QTimer* readPortTimer;

private slots:
//    void slotOpenPort();
//    void slotClosePort();
    void slotParseInput();
};

#endif // GPSMODULE_H
