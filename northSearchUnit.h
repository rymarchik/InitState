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

    /*!
    Метод задания имени порта
    \param[in] portName имя порта
    */
    void setPortName(QString portName);
    /*!
    Метод открытия порта
    \return true, если порт открылся успешно, иначе false
    */
    bool openSerialPort();
    //! Метод закрытия порта
    void closeSerialPort();
    //! Метод посылки команды на поиск севера
    void northSearch();
    /*!
    Метод считывания результата поиска севера
    \return готовое значение для записи в БД
    */
    int readNorthSearchResult();

private:
    QSerialPort* serial;
};

#endif // MAINWINDOW_H
