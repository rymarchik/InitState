#ifndef DATATRANSMISSIONMODULE_H
#define DATATRANSMISSIONMODULE_H

#include <QString>
#include <QUdpSocket>
#include <QtSql>
#include "converter.h"


class DataTransmissionModule
{
public:
    DataTransmissionModule(QString myIp, QString targetIp, QString myPort, QString targetPort, QSqlDatabase db);
    void sendCommand(QString q);
    void sendDocument(QString q);
private:
    QString myPort;
    QString targetPort;
    int unicumMessageId = 1;
    QUdpSocket udpSocket;
    QHostAddress myIp;
    QHostAddress targetIp;
    Converter *converter;
    QSqlDatabase db;

    QString makeDatagramCommand(QString q);
    QString makeDatagramDocument(QString q);
    QString getCommandInformation(QString object);
    QString getDocumentInformation(QString object);
};

#endif // DATATRANSMISSIONMODULE_H
