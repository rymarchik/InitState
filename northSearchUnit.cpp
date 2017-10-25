#include "northSearchUnit.h"

NorthSearchUnit::NorthSearchUnit(QObject *parent) : QObject(parent)
{
//    serial = new QSerialPort(this);

//    portNumberCB = new QComboBox;
//    foreach (const QSerialPortInfo &port, QSerialPortInfo::availablePorts()){
//        portNumberCB->addItem(port.portName());
//    }
//    portBaudRateCB = new QComboBox;
//    foreach (qint32 baudRate, QSerialPortInfo::standardBaudRates()){
//        portBaudRateCB->addItem(QString::number(baudRate));
//    }
//    portNumberCB->setCurrentText("COM4");
//    portBaudRateCB->setCurrentText("115200");
}

bool NorthSearchUnit::openPort() {
//    serial->setPortName(portNumberCB->currentText());
//    serial->setBaudRate(portBaudRateCB->currentText().toInt());
//    serial->setDataBits(QSerialPort::Data8);
//    serial->setStopBits(QSerialPort::OneStop);
//    if (serial->open(QIODevice::ReadWrite)) {
//        return true;
//    } else {
//        QMessageBox::critical(0, tr("Error"), serial->errorString());
//    }
    return false;
}

void NorthSearchUnit::closePort() {
//    if (serial->isOpen()) {
//        serial->close();
//    }
}

void NorthSearchUnit::northSearch() {
    QByteArray transmitCommand("t1843010100\x0D");
//    serial->write(transmitCommand);
}

int NorthSearchUnit::readNorthSearchResult() {
//    QByteArray input = serial->readAll();
    QByteArray input("t204705358B358B358B");
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
