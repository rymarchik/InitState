#ifndef MAPMODULE_H
#define MAPMODULE_H

#include <QObject>
#include <QtSql>
#include <QProcess>
#include <QTcpSocket>
#include "mapsrc/NetworkObjectManager.h"
#include "mapsrc/networkmodule.h"

class MapModule : public QObject
{
    Q_OBJECT
public:
    explicit MapModule(QSqlDatabase db, QString combatHierarchy, QString currentMode, QObject *parent = 0);

    QProcess* getMapProcess();
    QString getMapPath();
    NetworkObjectManager getObjectManager();
    void setObjectManager(NetworkObjectManager);

    /*!
    Метод удаления объекта из менеджера объектов
    \param[in] obj удаляемый объект
    */
    void removeObjectFromManager(NetworkObject obj);
    //! Метод удаления всех объектов из менеджера объектов
    void clearObjectManager();
    //! Метод запуска карты
    void launchMap();
    //! Метод загрузки боевых машин на карту
    void addBMsToMap();

private:
    QSqlDatabase db;
    QString combatHierarchy;
    QString currentMode;
    QProcess* mapProcess;
    QString mapPath = QCoreApplication::applicationDirPath() + "/mapFiles";
    NetworkObjectManager manager;

    //костыли
    QString currentModeTemp = "50.10";
    QString combatHierarchyTemp = "1.11";
    int importance = 100;
    QString mannerTid = "95.20.10";
    int idManager = 10;

public slots:
    /*!
    Слот обработки ошибок сервера
    \param[in] error текст ошибки
    */
    void serverError(QString error);
    /*!
    Слот для посылки на карту всех имеющихся объектов с БД
    \param[in] pSocket сокет, по которому передается информация
    */
    void sendNetworkUserMap(QTcpSocket* pSocket);
    /*!
    Слот для обработки сигнала о приеме пакета данных, в котором хранится информация о созданном на карте объекте
    \param[in] mas пакет данных, в который была упакована информация о созданном объекте
    */
    void receiveInsertObjectNetwork(QByteArray& mas);
    /*!
    Слот для обработки сигнала о приеме пакета данных, в котором хранится поле mas_object созданного на сервере объекта
    \param[in] mas пакет данных, в который был упакован mas_object созданного на сервере объекта
    */
    void receiveInsertObjectNetworkFromDB(QByteArray& mas);
    /*!
    Слот для обработки сигнала о приеме пакета данных, в котором хранятся идентификаторы удаленных объектов
    \param[in] mas пакет данных, в который был упакован список идентификаторов удаленных объектов
    */
    void receiveDeleteObjectNetwork(QByteArray& mas);
};

#endif // MAPMODULE_H
