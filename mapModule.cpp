#include "mapModule.h"
#include <QMessageBox>

MapModule::MapModule(QSqlDatabase db, QString combatHierarchy, QString currentMode, QObject *parent) :
    db(db),
    combatHierarchy(combatHierarchy),
    currentMode(currentMode),
    QObject(parent)
{
    mapProcess = new QProcess(this);
    if (!NetworkModule::Instance().serverFunc()) {
        QMessageBox::critical(0, "Ошибка", "Server error!");
    }

    connect(&NetworkModule::Instance(), SIGNAL(serverError(QString)),                          SLOT(serverError(QString)));
    connect(&NetworkModule::Instance(), SIGNAL(sendNetworkUserMap(QTcpSocket*)),               SLOT(sendNetworkUserMap(QTcpSocket*)));
    connect(&NetworkModule::Instance(), SIGNAL(receiveInsertObjectNetwork(QByteArray&)),       SLOT(receiveInsertObjectNetwork(QByteArray&)));
    connect(&NetworkModule::Instance(), SIGNAL(receiveInsertObjectNetworkFromDB(QByteArray&)), SLOT(receiveInsertObjectNetworkFromDB(QByteArray&)));
    connect(&NetworkModule::Instance(), SIGNAL(receiveDeleteObjectNetwork(QByteArray&)),       SLOT(receiveDeleteObjectNetwork(QByteArray&)));
}

NetworkObjectManager MapModule::getObjectManager() {
    return manager;
}

QProcess* MapModule::getMapProcess() {
    return mapProcess;
}

QString MapModule::getMapPath() {
    return mapPath;
}

void MapModule::setObjectManager(NetworkObjectManager manager) {
    this->manager = manager;
}

void MapModule::removeObjectFromManager(NetworkObject obj) {
    for (int i = 0; i < manager.listObject.size(); i++) {
        if (manager.listObject.at(i).data.m_OBJECT_ID == obj.data.m_OBJECT_ID) {
            manager.listObject.removeAt(i);
            break;
        }
    }
}

void MapModule::clearObjectManager() {
    manager.listObject.clear();
}

void MapModule::launchMap() {
    if (mapProcess->state() != QProcess::Running ) {
        mapProcess->setWorkingDirectory(mapPath + "/BIN");
        mapProcess->start(mapProcess->workingDirectory() + QString("/Karta.exe"));
    }
}

void MapModule::addBMsToMap() {
    QSqlQuery query = QSqlQuery(db);
    db.transaction();

    QString kostil = "50.10";
    QString selectMachineNames = "SELECT c.combat_hierarchy, t.termname "
                                 "FROM own_forces.combatstructure c "
                                 "JOIN own_forces.currentmode cm ON cm.combat_hierarchy = c.combat_hierarchy "
                                 "   AND cm.date_delete IS NULL "
                                 "JOIN reference_data.terms t ON c.object_name = t.termhierarchy "
                                 "WHERE cm.currentmode_tid = ? "
                                 "   AND nlevel(c.object_name) = 2 "
                                 "   AND c.date_delete is NULL";
    query.prepare(selectMachineNames);
    query.addBindValue(currentMode);
//    query.addBindValue(kostil);
    query.exec();
    QStringList machineCodes;
    QStringList machineNames;
    while (query.next()) {
        machineCodes << query.value(0).toString();
        machineNames << query.value(1).toString();
    }

    for (int i = 0; i < machineCodes.size(); i++) {
        //создание и добавление боевой машины в менеджер объектов
        NetworkObject obj;
        obj.data.m_OBJECT_ID = i + 1;
        obj.data.m_TYPE_ID = TYPE_METRIC_POINT; //но нужно TYPE_METRIC_POINT2 и добавить направление
        obj.data.m_ENEMY = 0;
        obj.data.m_VISIBLE = true;

        QString selectLocation = "SELECT obj_location "
                                 "FROM own_forces.combatobject_location "
                                 "WHERE combat_hierarchy = ? "
                                 "  AND date_add = (SELECT MAX(date_add) FROM own_forces.combatobject_location "
                                 "                  WHERE combat_hierarchy = ?)";
        query.prepare(selectLocation);
        query.addBindValue(machineCodes.at(i));
        query.addBindValue(machineCodes.at(i));
        query.exec();
        query.next();
        QString rawCoords = query.value(0).toString();


        QString getParsedCoordinates = "SELECT ST_X(geom), ST_Y(geom), ST_Z(geom) "
                                       "FROM (SELECT (ST_dumppoints(?)).geom) AS foo";
        query.prepare(getParsedCoordinates);
        query.addBindValue(rawCoords);
        query.exec();
        query.next();
        tagDataPoint point;
        point.m_LATITUDE = query.value(0).toDouble();
        point.m_LONGITUDE = query.value(1).toDouble();
        point.m_HEIGHT = query.value(2).toDouble();
        obj.metrics.append(point);

        if (machineNames.at(i) == "Боевая машина В-200") {
            obj.data.m_CODE = 1253010110; //код Боевой машины РСЗО
            obj.data.m_NAME = "Боевая машина B-200";
        }
        else if (machineNames.at(i) == "Машина боевого управления") {
            obj.data.m_CODE = 1015030110; //код КП батальона
            obj.data.m_NAME = "Машина боевого управления";
        }
        else if (machineNames.at(i) == "Транспортно-заряжающая машина") {
            obj.data.m_CODE = 1018030110; //код НП
            obj.data.m_NAME = "Транспортно-заряжающая машина";
        }

        QByteArray mas = NetworkModule::Instance().maskData(NETWORK_INSERT_OBJECT_FROM_DB, obj.serialize());
        NetworkModule::Instance().sendDataFromMap(mas);
    }
    db.commit();
}

void MapModule::serverError(QString str) {
    QMessageBox::critical(0, "Server Error", "Unable to start the server:" + str);
}

void MapModule::sendNetworkUserMap(QTcpSocket* pSocket) {
    QSqlQuery query = QSqlQuery(db);

    QString selectMapObjects = "SELECT id, code, object_name, geometry_type, enemy, visibility, "
                               "        access_level, object_location, mas_object "
                               "FROM map_objects.object_params "
                               "WHERE delete_time is null";
    if (!query.exec(selectMapObjects)) {
        qDebug() << query.lastError();
        QMessageBox::critical(0, "Ошибка", "Загрузить объекты с БД не удалось!");
    }
    while (query.next()) {
        NetworkObject obj;
        obj.data.m_OBJECT_ID = query.value(0).toInt();
        obj.data.m_CODE = query.value(1).toInt();
        obj.data.m_NAME = query.value(2).toString();
        obj.data.m_TYPE_ID = query.value(3).toInt();
        obj.data.m_ENEMY = query.value(4).toInt();
        obj.data.m_VISIBLE = query.value(5).toBool();
        obj.data.m_ACCESS_LVL = query.value(6).toInt();
        QString rawCoords = query.value(7).toString();
        obj.masObject = query.value(8).toByteArray();

        QSqlQuery coordQuery = QSqlQuery(db);
        QString getParsedCoordinates = "SELECT ST_X(geom), ST_Y(geom), ST_Z(geom) "
                                       "FROM (SELECT (ST_dumppoints(?)).geom "
                                       "      FROM map_objects.object_params) AS foo";
        coordQuery.prepare(getParsedCoordinates);
        coordQuery.addBindValue(rawCoords);
        coordQuery.exec();
        coordQuery.next();

        tagDataPoint metrics;
        metrics.m_LATITUDE = coordQuery.value(0).toDouble();
        metrics.m_LONGITUDE = coordQuery.value(1).toDouble();
        metrics.m_HEIGHT = coordQuery.value(2).toDouble();
        obj.metrics.append(metrics);

        manager.listObject.push_back(obj);
    }

    QByteArray mas = NetworkModule::Instance().maskData(NETWORK_SEND_MAP, manager.serialize());
    NetworkModule::Instance().SendToClient(pSocket, mas);

    addBMsToMap();
}

void MapModule::receiveInsertObjectNetwork(QByteArray& data) {
    unsigned char * lp=(unsigned char *)(data.data());
    lp+=2*sizeof(quint32);
    NetworkObject obj;
    obj.deserialize(lp,data.size()-8);

    qDebug() << obj.data.m_CODE;
    qDebug() << obj.data.m_NAME;
    qDebug() << obj.data.m_TYPE_ID;
    qDebug() << obj.data.m_ENEMY;
    qDebug() << obj.data.m_VISIBLE;
    qDebug() << obj.data.m_ACCESS_LVL;
    qDebug() << obj.metrics[0].m_LATITUDE;
    qDebug() << obj.metrics[0].m_LONGITUDE;
    qDebug() << obj.metrics[0].m_HEIGHT;
    qDebug() << obj.metrics[1].m_LATITUDE;
    qDebug() << obj.metrics[1].m_LONGITUDE;
    qDebug() << obj.metrics[1].m_HEIGHT;
    qDebug() << obj.masObject;

    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    QString insertPattern;
    QString insertObjectFromMap;
    QString makePointPattern = "ST_MakePoint(%1, %2, %3)";
    QString makeStartPointString = makePointPattern.arg(obj.metrics[0].m_LATITUDE)
                                                   .arg(obj.metrics[0].m_LONGITUDE)
                                                   .arg(obj.metrics[0].m_HEIGHT);
    switch (obj.data.m_TYPE_ID) {
    case TYPE_METRIC_LINE:
        insertPattern = "INSERT INTO map_objects.object_params (code, object_name, geometry_type, "
                        "       enemy, visibility, access_level, object_location, mas_object) "
                        "VALUES (?, ?, ?, ?, ?, ?, %1, ?)";
        if (obj.metrics.size() == 2) {
            QString makeLinePattern = "ST_MakeLine(ST_MakePoint(%1, %2, %3), ST_MakePoint(%4, %5, %6))";
            QString makeLineString = makeLinePattern.arg(obj.metrics[0].m_LATITUDE)
                                                    .arg(obj.metrics[0].m_LONGITUDE)
                                                    .arg(obj.metrics[0].m_HEIGHT)
                                                    .arg(obj.metrics[1].m_LATITUDE)
                                                    .arg(obj.metrics[1].m_LONGITUDE)
                                                    .arg(obj.metrics[1].m_HEIGHT);
            insertObjectFromMap = insertPattern.arg(makeLineString);
        }
        else if (obj.metrics.size() > 2) {
            QString makePolygonString = "ST_MakePolygon(ST_MakeLine(ARRAY[";
            for (int i = 0; i < obj.metrics.size(); i++) {
                QString makePointString = makePointPattern.arg(obj.metrics[i].m_LATITUDE)
                                                          .arg(obj.metrics[i].m_LONGITUDE)
                                                          .arg(obj.metrics[i].m_HEIGHT);
                makePolygonString.append(makePointString).append(", ");
            }
            makePolygonString.append(makeStartPointString).append("]))");
            insertObjectFromMap = insertPattern.arg(makePolygonString);
        }
        query.prepare(insertObjectFromMap);
        query.addBindValue(obj.data.m_CODE);
        query.addBindValue(obj.data.m_NAME);
        query.addBindValue(obj.data.m_TYPE_ID);
        query.addBindValue(obj.data.m_ENEMY);
        query.addBindValue(obj.data.m_VISIBLE);
        query.addBindValue(obj.data.m_ACCESS_LVL);
        query.addBindValue(obj.masObject);
        break;

    case TYPE_METRIC_RECT:
        insertPattern = "INSERT INTO map_objects.object_params (code, object_name, geometry_type, enemy, "
                        "       visibility, access_level, object_location, front, depth, deviation, mas_object) "
                        "VALUES (?, ?, ?, ?, ?, ?, %1, ?, ?, ?, ?)";
        insertObjectFromMap = insertPattern.arg(makeStartPointString);

        query.prepare(insertObjectFromMap);
        query.addBindValue(obj.data.m_CODE);
        query.addBindValue(obj.data.m_NAME);
        query.addBindValue(obj.data.m_TYPE_ID);
        query.addBindValue(obj.data.m_ENEMY);
        query.addBindValue(obj.data.m_VISIBLE);
        query.addBindValue(obj.data.m_ACCESS_LVL);
        query.addBindValue(obj.metrics[1].m_LATITUDE);
        query.addBindValue(obj.metrics[1].m_LONGITUDE);
        query.addBindValue(obj.metrics[1].m_HEIGHT);
        query.addBindValue(obj.masObject);
        break;

    case TYPE_METRIC_CIRCLE:
        insertPattern = "INSERT INTO map_objects.object_params (code, object_name, geometry_type, enemy, "
                        "       visibility, access_level, object_location, radius, mas_object) "
                        "VALUES (?, ?, ?, ?, ?, ?, %1, ?, ?)";
        insertObjectFromMap = insertPattern.arg(makeStartPointString);

        query.prepare(insertObjectFromMap);
        query.addBindValue(obj.data.m_CODE);
        query.addBindValue(obj.data.m_NAME);
        query.addBindValue(obj.data.m_TYPE_ID);
        query.addBindValue(obj.data.m_ENEMY);
        query.addBindValue(obj.data.m_VISIBLE);
        query.addBindValue(obj.data.m_ACCESS_LVL);
        query.addBindValue(obj.metrics[1].m_LATITUDE);
        query.addBindValue(obj.masObject);
        break;
    }
    if (!query.exec()) {
        qDebug() << query.lastError();
        QMessageBox::critical(0, "Ошибка", "Добавить объект не удалось!");
    }


    QString selectId = "SELECT id "
                       "FROM map_objects.object_params "
                       "WHERE mas_object = ?";
    query.prepare(selectId);
    query.addBindValue(obj.masObject);
    if (!query.exec()) {
        qDebug() << query.lastError();
        QMessageBox::critical(0, "Ошибка", "Получить ID объекта не удалось!");
    }
    query.next();
    obj.data.m_OBJECT_ID = query.value(0).toInt();
    qDebug() << "ID" << obj.data.m_OBJECT_ID;
    manager.listObject.push_back(obj);

    db.commit();

    QByteArray mas = NetworkModule::Instance().maskData(NETWORK_OBJECT_SET_ID, obj.serialize());
    NetworkModule::Instance().sendDataFromMap(mas);
}

void MapModule::receiveInsertObjectNetworkFromDB(QByteArray& data)
{
    unsigned char * lp=(unsigned char *)(data.data());
    lp+=2*sizeof(quint32);
    NetworkObject obj;
    obj.deserialize(lp,data.size()-8);

    if (obj.data.m_OBJECT_ID < 10) { //для боевых машин (условно машин до 10)
        manager.listObject.push_front(obj);
        return;
    }

//    qDebug() << obj.metrics[0].m_LATITUDE;
//    qDebug() << obj.metrics[0].m_LONGITUDE;
//    qDebug() << obj.metrics[0].m_HEIGHT;

//    qDebug() << obj.metrics[1].m_LATITUDE;
//    qDebug() << obj.metrics[1].m_LONGITUDE;
//    qDebug() << obj.metrics[1].m_HEIGHT;

//    qDebug() << "ID" << obj.data.m_OBJECT_ID;
//    qDebug() << obj.data.m_CODE;
//    qDebug() << obj.data.m_NAME;
//    qDebug() << obj.data.m_TYPE_ID;
//    qDebug() << obj.data.m_ENEMY;
//    qDebug() << obj.data.m_VISIBLE;
//    qDebug() << obj.data.m_ACCESS_LVL;
//    qDebug() << obj.masObject;

    QSqlQuery query = QSqlQuery(db);
    QString updateMasObject = "UPDATE map_objects.object_params "
                              "SET mas_object = ? "
                              "WHERE id = ?";
    query.prepare(updateMasObject);
    query.addBindValue(obj.masObject);
    query.addBindValue(obj.data.m_OBJECT_ID);
    if (!query.exec()) {
        qDebug() << query.lastError();
        QMessageBox::critical(0, "Ошибка", "Добавить объект не удалось!");
    }
    manager.listObject.push_back(obj);
}

void MapModule::receiveDeleteObjectNetwork(QByteArray& mas)
{
    QVector<int> masId;

    unsigned char * lp=(unsigned char *)(mas.data());
    lp+=2*sizeof(quint32);

    quint32 count;
    memcpy(&count,lp,sizeof(quint32));
    lp+=sizeof(quint32);

    for(quint32 i=0;i<count;i++)
    {
        quint32 index;
        memcpy(&index,lp,sizeof(quint32));
        lp+=sizeof(quint32);
        masId.push_back(index);
    }

    for(int i=0;i<masId.size();i++)
    {
        for(int j=0;j<manager.listObject.size();j++)
        {
            if(masId[i]==manager.listObject[j].data.m_OBJECT_ID)
            {
                QSqlQuery query;
//                QString deleteObject = "DELETE FROM map_objects.object_params WHERE id = ?";
                QString deleteObject = "UPDATE map_objects.object_params "
                                       "SET visibility = false, delete_time = now() "
                                       "WHERE id = ?";
                query.prepare(deleteObject);
                query.addBindValue(manager.listObject[j].data.m_OBJECT_ID);
                if (!query.exec()) {
                    qDebug() << query.lastError();
                    QMessageBox::critical(0, "Ошибка", "Удалить объект не удалось!");
                }
                manager.listObject.remove(j);
                break;
            }
        }
    }
}
