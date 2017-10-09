#ifndef MAPCOMMANDS_H
#define MAPCOMMANDS_H

#include <QObject>
#include <QtSql>

#include <QProcess>
#include <QTcpSocket>
#include "mapsrc/NetworkObjectManager.h"
#include "mapsrc/networkmodule.h"
#include "mapsrc/PropertyObj.h"

class MapCommands : public QObject
{
    Q_OBJECT
public:
    explicit MapCommands(QSqlDatabase db, QObject *parent = 0);

private:
    QSqlDatabase db;
    QProcess* mapProcess;
    NetworkObjectManager manager;
    QString mapPath = QCoreApplication::applicationDirPath() + "/mapFiles";

public slots:
//    void finished(int);
//    void serverError(QString);
//    void sendNetworkUserMap(QTcpSocket* pSocket);
//    void receiveInsertObjectNetwork(QByteArray&);
//    void receiveInsertObjectNetworkFromDB(QByteArray&);

//    /**
//    \fn   receiveDeleteObjectNetwork
//    Слот для обработки сигнала о приеме пакета данных, в котором хранятся идентификаторы удаленных объектов
//    \param[in] QByteArray & mas - ссылка пакет данных, в который был упакован список идентификаторов удаленных объектов
//    \return не возвращает
//    */
//    void receiveDeleteObjectNetwork(QByteArray&);
};

#endif // MAPCOMMANDS_H
