#include "northSearchUnit.h"

NorthSearchUnit::NorthSearchUnit(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
}

void NorthSearchUnit::setPortName(QString portName) {
    serial->setPortName(portName);
}

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

void NorthSearchUnit::closeSerialPort() {
    if (serial->isOpen()) {
        serial->close();
    }
}

void NorthSearchUnit::northSearch() {
    QByteArray transmitCommand("t1843010000\x0D");
    serial->write(transmitCommand);
}

int NorthSearchUnit::readNorthSearchResult() {
    QByteArray input = serial->readAll().right(20);
//    QByteArray input("t204705358B358B358B");
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

void NorthSearchUnit::slotGetSearchResult() {

}

void NorthSearchUnit::slotEnterCalibrationData() {

}

void NorthSearchUnit::slotUseCalibrationData() {

}
