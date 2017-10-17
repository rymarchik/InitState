#ifndef MAPMODULE_H
#define MAPMODULE_H

#include <QObject>
#include <QtSql>
#include <QProcess>
#include <QTcpSocket>
#include "mapsrc/NetworkObjectManager.h"
#include "mapsrc/networkmodule.h"
#include "mapsrc/PropertyObj.h"

class MapModule : public QObject
{
    Q_OBJECT
public:
    explicit MapModule(QSqlDatabase db, QObject *parent = 0);

    QProcess* getMapProcess();
    QString getMapPath();
    NetworkObjectManager getObjectManager();
    void setObjectManager(NetworkObjectManager);
    void clearObjectManager();
    void launchMap();

private:
    QSqlDatabase db;
    QProcess* mapProcess;
    QString mapPath = QCoreApplication::applicationDirPath() + "/mapFiles";
    NetworkObjectManager manager;

public slots:
    void serverError(QString);
    void sendNetworkUserMap(QTcpSocket* pSocket);
    void receiveInsertObjectNetwork(QByteArray&);
    void receiveInsertObjectNetworkFromDB(QByteArray&);

    /**
    \fn   receiveDeleteObjectNetwork
    Слот для обработки сигнала о приеме пакета данных, в котором хранятся идентификаторы удаленных объектов
    \param[in] QByteArray & mas - ссылка пакет данных, в который был упакован список идентификаторов удаленных объектов
    \return не возвращает
    */
    void receiveDeleteObjectNetwork(QByteArray&);
};

#endif // MAPMODULE_H
