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

    bool openPort();
    void closePort();
    void northSearch();
    int readNorthSearchResult();

private:
    QSerialPort* serial;
    QTimer* readPortTimer;

    QComboBox* portNumberCB;
    QComboBox* portBaudRateCB;

private slots:


    void slotGetSearchResult();
    void slotEnterCalibrationData();
    void slotUseCalibrationData();
};

#endif // MAINWINDOW_H
