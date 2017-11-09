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

/*!
Метод удаления объекта из менеджера объектов
\param[in] obj удаляемый объект
*/
void MapModule::removeObjectFromManager(NetworkObject obj) {
    for (int i = 0; i < manager.listObject.size(); i++) {
        if (manager.listObject.at(i).data.m_OBJECT_ID == obj.data.m_OBJECT_ID) {
            manager.listObject.removeAt(i);
            break;
        }
    }
}

//! Метод удаления всех объектов из менеджера объектов
void MapModule::clearObjectManager() {
    manager.listObject.clear();
}

//! Метод запуска карты
void MapModule::launchMap() {
    if (mapProcess->state() != QProcess::Running ) {
        mapProcess->setWorkingDirectory(mapPath + "/BIN");
        mapProcess->start(mapProcess->workingDirectory() + QString("/Karta.exe"));
    }
}

//! Метод загрузки боевых машин на карту
void MapModule::addBMsToMap() {
    QSqlQuery query = QSqlQuery(db);
    db.transaction();

    QString selectMachineNames = "SELECT c.combat_hierarchy, t.termhierarchy, t.termname "
                                 "FROM own_forces.combatstructure c "
                                 "JOIN own_forces.currentmode cm ON cm.combat_hierarchy = c.combat_hierarchy "
                                 "   AND cm.date_delete IS NULL "
                                 "JOIN reference_data.terms t ON c.object_name = t.termhierarchy "
                                 "WHERE cm.currentmode_tid = ? "
                                 "   AND nlevel(c.object_name) = 2 "
                                 "   AND c.date_delete is NULL";
    query.prepare(selectMachineNames);
    query.addBindValue(currentModeTemp); //костыль, заменить на currentMode
    query.exec();
    QStringList machineCombatHierarchies;
    QStringList machineTermHierarchies;
    QStringList machineNames;
    while (query.next()) {
        machineCombatHierarchies << query.value(0).toString();
        machineTermHierarchies << query.value(1).toString();
        machineNames << query.value(2).toString();
    }

    for (int i = 0; i < machineCombatHierarchies.size(); i++) {
        //создание и добавление боевой машины в менеджер объектов
        NetworkObject obj;
        obj.data.m_OBJECT_ID = i + 1;
        obj.data.m_TYPE_ID = TYPE_METRIC_POINT; //но нужно TYPE_METRIC_POINT2 и добавить вторую точку для направления
        obj.data.m_ENEMY = 0;
        obj.data.m_VISIBLE = true;

        QString selectClassifierCode = "SELECT id_terms "
                                       "FROM reference_data.map_code "
                                       "WHERE termhierarchy = ?";
        query.prepare(selectClassifierCode);
        query.addBindValue(machineTermHierarchies.at(i));
        query.exec();
        query.next();
        obj.data.m_CODE = query.value(0).toInt();
        obj.data.m_NAME = machineNames.at(i);


        QString selectLocation = "SELECT obj_location "
                                 "FROM own_forces.combatobject_location "
                                 "WHERE combat_hierarchy = ? "
                                 "  AND date_add = (SELECT MAX(date_add) FROM own_forces.combatobject_location "
                                 "                  WHERE combat_hierarchy = ?)";
        query.prepare(selectLocation);
        query.addBindValue(machineCombatHierarchies.at(i));
        query.addBindValue(machineCombatHierarchies.at(i));
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

        QByteArray mas = NetworkModule::Instance().maskData(NETWORK_INSERT_OBJECT_FROM_DB, obj.serialize());
        NetworkModule::Instance().sendDataFromMap(mas);
    }
    db.commit();
}

/*!
Слот обработки ошибок сервера
\param[in] error текст ошибки
*/
void MapModule::serverError(QString str) {
    QMessageBox::critical(0, "Server Error", "Unable to start the server:" + str);
}

/*!
Слот для посылки на карту всех имеющихся объектов с БД
\param[in] pSocket сокет, по которому передается информация
*/
void MapModule::sendNetworkUserMap(QTcpSocket* pSocket) {
    QSqlQuery query = QSqlQuery(db);

    QString selectMapObjects = "SELECT id_terrain_objective, mc.id_terms, t.termname, geometry_type, enemy, "
                               "        visibility, access_level, tp.obj_location, mas_object "
                               "FROM map_objects.object_params op "
                               "JOIN targets.obj_targets ot ON CAST(op.military_object AS INTEGER) = ot.id_target "
                               "JOIN targets.targets_param tp ON ot.id_target = tp.id_target "
                               "JOIN reference_data.map_code mc ON ot.target_name = mc.termhierarchy "
                               "JOIN reference_data.terms t ON mc.termhierarchy = t.termhierarchy "
                               "WHERE op.date_delete is null";
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
                                       "FROM (SELECT (ST_dumppoints(?)).geom) AS foo";
        coordQuery.prepare(getParsedCoordinates);
        coordQuery.addBindValue(rawCoords);
        coordQuery.exec();
        tagDataPoint point;

        switch (obj.data.m_TYPE_ID) {
        case TYPE_METRIC_LINE: {
            while (coordQuery.next()) {
                point.m_LATITUDE = query.value(0).toDouble();
                point.m_LONGITUDE = query.value(1).toDouble();
                point.m_HEIGHT = query.value(2).toDouble();
                obj.metrics.append(point);
            }
            break;
        }
        case TYPE_METRIC_RECT: {
            coordQuery.next();
            point.m_LATITUDE = query.value(0).toDouble();
            point.m_LONGITUDE = query.value(1).toDouble();
            point.m_HEIGHT = query.value(2).toDouble();
            coordQuery.next();
            tagDataPoint squareData;
            squareData.m_LATITUDE = query.value(0).toDouble();
            squareData.m_LONGITUDE = query.value(1).toDouble();
            squareData.m_HEIGHT = query.value(2).toDouble();
            obj.metrics.append(point);
            obj.metrics.append(squareData);
            break;
        }
        case TYPE_METRIC_CIRCLE: {
            coordQuery.next();
            point.m_LATITUDE = query.value(0).toDouble();
            point.m_LONGITUDE = query.value(1).toDouble();
            point.m_HEIGHT = query.value(2).toDouble();
            coordQuery.next();
            tagDataPoint roundData;
            roundData.m_LATITUDE = query.value(0).toDouble();
            obj.metrics.append(point);
            obj.metrics.append(roundData);
            break;
        }
        }
        manager.listObject.push_back(obj);
    }

    QByteArray mas = NetworkModule::Instance().maskData(NETWORK_SEND_MAP, manager.serialize());
    NetworkModule::Instance().SendToClient(pSocket, mas);

    addBMsToMap();
}

/*!
Слот для обработки сигнала о приеме пакета данных, в котором хранится информация о созданном на карте объекте
\param[in] mas пакет данных, в который была упакована информация о созданном объекте
*/
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

    //! Запрос на получение termhierarchy по названию, формируемому картой
    QString selectTermHierarchy = "SELECT termhierarchy FROM reference_data.map_code "
                                  "WHERE terms = ?";
    query.prepare(selectTermHierarchy);
    query.addBindValue(obj.data.m_NAME);
    query.exec();
    query.next();
    QString termHierarchy = query.value(0).toString();

    //! Запрос на получение последнего номера цели (временно)
    QString selectLastTargetNumber =  "SELECT MAX(target_number) FROM targets.obj_targets";
    query.prepare(selectLastTargetNumber);
    query.exec();
    query.next();
    int targetNumber = query.value(0).toInt();


    qDebug() << termHierarchy;
    qDebug() << targetNumber + 1;
    qDebug() << combatHierarchyTemp;
    qDebug() << idManager;
    //! Вставка данных в таблицу obj_targets
    QString insertObjTargets = "INSERT INTO targets.obj_targets (target_name, target_number, combat_hierarchy, id_manager) "
                               "VALUES (?, ?, ?, ?)";
    query.prepare(insertObjTargets);
    query.addBindValue(termHierarchy);
    query.addBindValue(targetNumber + 1); //костыль
    query.addBindValue(combatHierarchyTemp); //костыль, заменить на combatHierarchy
    query.addBindValue(idManager);
    query.exec();

    //! Запрос на получение последней добавленной цели
    QString selectLastIdTarget = "SELECT MAX(id_target) FROM targets.obj_targets";
    query.prepare(selectLastIdTarget);
    query.exec();
    query.next();
    int targetId = query.value(0).toInt();


    QString insertPattern;
    QString insertTargetsParam;
    QString makePointPattern = "ST_MakePoint(%1, %2, %3)";
    QString makeStartPointString = makePointPattern.arg(obj.metrics[0].m_LATITUDE)
                                                   .arg(obj.metrics[0].m_LONGITUDE)
                                                   .arg(obj.metrics[0].m_HEIGHT);
    switch (obj.data.m_TYPE_ID) {
    case TYPE_METRIC_POINT2: {
        insertPattern = "INSERT INTO targets.targets_param (id_target, target_time, importance, "
                        "   method_location, obj_location, manner_tid, tid, id_manager) "
                        "VALUES (?, now(), ?, ?, %1, ?, txid_current(), ?)";
        QString makeLinePattern = "ST_MakeLine(ST_MakePoint(%1, %2, %3), ST_MakePoint(%4, %5, %6))";
        QString makeLineString = makeLinePattern.arg(obj.metrics[0].m_LATITUDE)
                                                .arg(obj.metrics[0].m_LONGITUDE)
                                                .arg(obj.metrics[0].m_HEIGHT)
                                                .arg(obj.metrics[1].m_LATITUDE)
                                                .arg(obj.metrics[1].m_LONGITUDE)
                                                .arg(obj.metrics[1].m_HEIGHT);
        insertTargetsParam = insertPattern.arg(makeLineString);
        query.prepare(insertTargetsParam);
        query.addBindValue(targetId);
        query.addBindValue(importance);
        query.addBindValue(0);
        query.addBindValue(mannerTid);
        query.addBindValue(idManager);
        query.exec();
        break;
    }

    case TYPE_METRIC_LINE: {
        insertPattern = "INSERT INTO targets.targets_param (id_target, target_time, importance, "
                        "   method_location, obj_location, manner_tid, tid, id_manager) "
                        "VALUES (?, now(), ?, ?, %1, ?, txid_current(), ?)";
        if (obj.metrics.size() == 2) {
            QString makeLinePattern = "ST_MakeLine(ST_MakePoint(%1, %2, %3), ST_MakePoint(%4, %5, %6))";
            QString makeLineString = makeLinePattern.arg(obj.metrics[0].m_LATITUDE)
                                                    .arg(obj.metrics[0].m_LONGITUDE)
                                                    .arg(obj.metrics[0].m_HEIGHT)
                                                    .arg(obj.metrics[1].m_LATITUDE)
                                                    .arg(obj.metrics[1].m_LONGITUDE)
                                                    .arg(obj.metrics[1].m_HEIGHT);
            insertTargetsParam = insertPattern.arg(makeLineString);
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
            insertTargetsParam = insertPattern.arg(makePolygonString);
        }
        query.prepare(insertTargetsParam);
        query.addBindValue(targetId);
        query.addBindValue(importance);
        query.addBindValue(0);
        query.addBindValue(mannerTid);
        query.addBindValue(idManager);
        query.exec();
        break;
    }

    case TYPE_METRIC_RECT: {
        insertPattern = "INSERT INTO targets.targets_param (id_target, target_time, importance, "
                        "   method_location, obj_location, front, depth, deviation, manner_tid, tid, id_manager) "
                        "VALUES (?, now(), ?, ?, %1, ?, ?, ?, ?, txid_current(), ?)";
        insertTargetsParam = insertPattern.arg(makeStartPointString);

        query.prepare(insertTargetsParam);
        query.addBindValue(targetId);
        query.addBindValue(importance);
        query.addBindValue(1);
        query.addBindValue(obj.metrics[1].m_LATITUDE);
        query.addBindValue(obj.metrics[1].m_LONGITUDE);
        query.addBindValue(obj.metrics[1].m_HEIGHT);
        query.addBindValue(mannerTid);
        query.addBindValue(idManager);
        query.exec();
        break;
    }

    case TYPE_METRIC_CIRCLE: {
        insertPattern = "INSERT INTO targets.targets_param (id_target, target_time, importance, "
                        "   method_location, obj_location, radius, manner_tid, tid, id_manager) "
                        "VALUES (?, now(), ?, ?, %1, ?, ?, txid_current(), ?)";
        insertTargetsParam = insertPattern.arg(makeStartPointString);

        query.prepare(insertTargetsParam);
        query.addBindValue(targetId);
        query.addBindValue(importance);
        query.addBindValue(2);
        query.addBindValue(obj.metrics[1].m_LATITUDE);
        query.addBindValue(mannerTid);
        query.addBindValue(idManager);
        query.exec();
        break;
    }
    }


    //! Вставка данных в таблицу для объектов карты object_params
    QString insertObjectParams = "INSERT INTO map_objects.object_params (military_object, geometry_type, "
                                 "        enemy, visibility, access_level, mas_object, id_manager) "
                                 "VALUES (?, ?, ?, ?, ?, ?, ?)";
    query.prepare(insertObjectParams);
    query.addBindValue(151);
    query.addBindValue(obj.data.m_TYPE_ID);
    query.addBindValue(obj.data.m_ENEMY);
    query.addBindValue(obj.data.m_VISIBLE);
    query.addBindValue(obj.data.m_ACCESS_LVL);
    query.addBindValue(obj.masObject);
    query.addBindValue(idManager);
    qDebug() << query.exec() << "    " << query.lastError();

    QString selectId = "SELECT id_terrain_objective "
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
    manager.listObject.push_back(obj);

    db.commit();

    QByteArray mas = NetworkModule::Instance().maskData(NETWORK_OBJECT_SET_ID, obj.serialize());
    NetworkModule::Instance().sendDataFromMap(mas);
}

/*!
Слот для обработки сигнала о приеме пакета данных, в котором хранится поле mas_object созданного на сервере объекта
\param[in] mas пакет данных, в который был упакован mas_object созданного на сервере объекта
*/
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
                              "WHERE id_terrain_objective = ?";
    query.prepare(updateMasObject);
    query.addBindValue(obj.masObject);
    query.addBindValue(obj.data.m_OBJECT_ID);
    if (!query.exec()) {
        qDebug() << query.lastError();
        QMessageBox::critical(0, "Ошибка", "Добавить объект не удалось!");
    }
    manager.listObject.push_back(obj);
}

/*!
Слот для обработки сигнала о приеме пакета данных, в котором хранятся идентификаторы удаленных объектов
\param[in] mas пакет данных, в который был упакован список идентификаторов удаленных объектов
*/
void MapModule::receiveDeleteObjectNetwork(QByteArray& mas) {
    QVector<int> masId;

    unsigned char * lp=(unsigned char *)(mas.data());
    lp+=2*sizeof(quint32);

    quint32 count;
    memcpy(&count,lp,sizeof(quint32));
    lp+=sizeof(quint32);

    for (quint32 i = 0; i < count; i++) {
        quint32 index;
        memcpy(&index,lp,sizeof(quint32));
        lp+=sizeof(quint32);
        masId.push_back(index);
    }

    for (int i=0; i < masId.size(); i++) {
        for (int j = 0; j < manager.listObject.size(); j++) {
            if (masId[i] == manager.listObject[j].data.m_OBJECT_ID) {
                QSqlQuery query = QSqlQuery(db);
                db.transaction();

                QString deleteFromObjTargets = "UPDATE targets.obj_targets "
                                               "SET date_delete = now(), id_manager = ? "
                                               "FROM map_objects.object_params "
                                               "WHERE id_target = CAST(military_object AS INTEGER) "
                                               "    AND id_terrain_objective = ?";
                query.prepare(deleteFromObjTargets);
                query.addBindValue(idManager);
                query.addBindValue(masId[i]);
                query.exec();

                QString deleteFromMapObjects = "UPDATE map_objects.object_params "
                                               "SET visibility = false, date_delete = now(), id_manager = ? "
                                               "WHERE id_terrain_objective = ?";
                query.prepare(deleteFromMapObjects);
                query.addBindValue(idManager);
                query.addBindValue(masId[i]);
                query.exec();

                if (!db.commit()) {
                    QMessageBox::critical(0, "Ошибка", "Удалить объект не удалось!");
                }
                manager.listObject.remove(j);
                break;
            }
        }
    }
}
