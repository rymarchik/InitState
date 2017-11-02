#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QSerialPort>
#include <QSerialPortInfo>

class NorthSearchUnit : public QObject
{
    Q_OBJECT

public:
    explicit NorthSearchUnit(QObject *parent = 0);

    void setPortName(QString);
    bool openSerialPort();
    void closeSerialPort();
    void northSearch();
    int readNorthSearchResult();

private:
    QSerialPort* serial;

private slots:
    void slotGetSearchResult();
    void slotEnterCalibrationData();
    void slotUseCalibrationData();
};

#endif // MAINWINDOW_H
