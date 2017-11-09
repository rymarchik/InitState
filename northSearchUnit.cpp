#include "northSearchUnit.h"

NorthSearchUnit::NorthSearchUnit(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
}

/*!
Метод задания имени порта
\param[in] portName имя порта
*/
void NorthSearchUnit::setPortName(QString portName) {
    serial->setPortName(portName);
}

/*!
Метод открытия порта
\return true, если порт открылся успешно, иначе false
*/
bool NorthSearchUnit::openSerialPort() {
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    if (serial->open(QIODevice::ReadWrite)) {
        return true;
    }
    else {
        QMessageBox::critical(0, tr("Error"), serial->errorString());
    }
    return false;
}

//! Метод закрытия порта
void NorthSearchUnit::closeSerialPort() {
    if (serial->isOpen()) {
        serial->close();
    }
}

//! Метод посылки команды на поиск севера
void NorthSearchUnit::northSearch() {
    QByteArray transmitCommand("t1843010000\x0D");
    serial->write(transmitCommand);
}

/*!
Метод считывания результата поиска севера
\return готовое значение для записи в БД
*/
int NorthSearchUnit::readNorthSearchResult() {
    QByteArray input = serial->readAll().right(20);
//    QByteArray input("t204705358B358B358B"); костыль, чтобы сымитировать результат работы АПС (если требуется)
    int parsedAzim = -1;
    if (input.contains("t204705")) {
        QString azimInHex;
        azimInHex.append(input.at(9)).append(input.at(10)).append(input.at(7)).append(input.at(8));
        bool ok;
        int azimInDec = azimInHex.toInt(&ok,16);
        parsedAzim = qRound((double)azimInDec / 60000 * 360 * 3600) * 100;
    }
    return parsedAzim;
}
