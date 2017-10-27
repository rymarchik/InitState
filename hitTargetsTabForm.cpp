#include "hitTargetsTabForm.h"
#include "ui_hittargets.h"
#include "mapModule.h"
#include "utility.h"

HitTargetsTabForm::HitTargetsTabForm(QSqlDatabase db, QString combatHierarchy, MapModule* map, QWidget *parent) :
    db(db),
    combatHierarchy(combatHierarchy),
    map(map),
    QWidget(parent),
    ui(new Ui::HitTargets)
{
    ui->setupUi(this);
    ui->coordinateLE->setInputMask("99°99'99.99\''A 999°99'99.99\''A 9999.9;_");

    if (map->getMapProcess()->state() == QProcess::Running) {
        ui->pickCoordinatesBtn->setEnabled(true);
    }

    connect(&NetworkModule::Instance(), SIGNAL(receiveMetricsNetwork(QByteArray&)), this, SLOT(receiveMetricsNetwork(QByteArray&)));
    connect(map->getMapProcess(), SIGNAL(started()), this, SLOT(slotMapOpen()));
    connect(map->getMapProcess(), SIGNAL(finished(int)), this, SLOT(slotMapClose()));
}

/*!
Метод получения номера цели (на интерфейсе № Цели)
\return возвращает строку с номером цели
*/
QString HitTargetsTabForm::getTargetNumberString() {
    return ui->targetNumberLE->text();
}

/*!
Метод получения текущего названия цели (на интерфейсе Наименование)
\return возвращает строку с текущим названием цели
*/
QString HitTargetsTabForm::getTargetNameString() {
    return ui->targetNameCB->currentText();
}

//!Слот обработки открытия карты
void HitTargetsTabForm::slotMapOpen() {
    ui->pickCoordinatesBtn->setEnabled(true);
}

//!Слот обработки закрытия карты
void HitTargetsTabForm::slotMapClose() {
    ui->pickCoordinatesBtn->setEnabled(false);
}

/*!
\brief Слот обработки нажатия на кнопку Съем координат

Посылает запрос на переход в режим съема координат,
учитывая выбор геометрии цели (ломаная, прямоугольник или круг)
*/
void HitTargetsTabForm::slotPickCoordinates() {
    QString title1 = "КАРТА-2017 - [Окно Карты" + map->getMapPath() + "/maps/200000.rag]";
    LPCWSTR title = (const wchar_t*) title1.utf16();
    HWND hwnd = FindWindow(0,title);
    SetForegroundWindow(hwnd);

    if (ui->randomRB->isChecked()) {
        NetworkModule::Instance().sendMetricsReq(TYPE_METRIC_LINE);
    }
    else if (ui->squareRB->isChecked()) {
        NetworkModule::Instance().sendMetricsReq(TYPE_METRIC_RECT);
    }
    else if (ui->roundRB->isChecked()) {
        NetworkModule::Instance().sendMetricsReq(TYPE_METRIC_CIRCLE);
    }
}

/*!
Метод преобразования широты, долготы и высоты, выраженных десятичной дробью,
в градусы, минуты и секунды
\param[in] lat широта
\param[in] lon долгота
\param[in] alt высота
\return возвращает строку с координатами в виде градусов, минут и секунд и высотой
*/
QString HitTargetsTabForm::getParsedCoordinates(double lat, double lon, double alt) {
    QSqlQuery query = QSqlQuery(db);
    QString makePointString = "SELECT ST_MakePoint(?, ?, ?)";
    query.prepare(makePointString);
    query.addBindValue(lat);
    query.addBindValue(lon);
    query.addBindValue(alt);
    query.exec();
    query.next();
    QString coordinatesInHex = query.value(0).toString();
    qDebug() << "coordinatesInHex: " << coordinatesInHex;

    QString parsedString = "SELECT own_forces.coordinates_output(?)";
    query.prepare(parsedString);
    query.addBindValue(coordinatesInHex);
    query.exec();
    query.next();

    qDebug() << "parsedCoordinates: " << query.value(0).toString();
    return query.value(0).toString();
}

/*!
Слот обработки полученных координат, снятых с карты
\param[in] data массив данных, содержащий информацию о координатах
*/
void HitTargetsTabForm::receiveMetricsNetwork(QByteArray& data)
{
    unsigned char * lp=(unsigned char *)(data.data());
    lp+=2*sizeof(quint32);
    NetworkObject obj;
    obj.deserialize(lp,data.size()-8);

    ui->coordinateLE->setText(getParsedCoordinates(obj.metrics[0].m_LATITUDE,
                              obj.metrics[0].m_LONGITUDE, obj.metrics[0].m_HEIGHT));

    switch(obj.data.m_TYPE_ID)
    {
    case TYPE_METRIC_LINE:
        Utility::clearLayout(ui->extraCoordinatesLayout);
        QHBoxLayout* coordStringLayout;
        QLabel* newCoordLbl;
        QLineEdit* newCoordLE;
        for (int i = 1; i < obj.metrics.size(); i++) {
            newCoordLbl = new QLabel("Координата");
            newCoordLbl->setFont(font);

            newCoordLE = new QLineEdit(getParsedCoordinates(obj.metrics[i].m_LATITUDE,
                                       obj.metrics[i].m_LONGITUDE, obj.metrics[i].m_HEIGHT));
            newCoordLE->setInputMask("99°99'99.99\''A 999°99'99.99\''A 9999.9;_");

            coordStringLayout = new QHBoxLayout;
            coordStringLayout->addWidget(newCoordLbl);
            coordStringLayout->addWidget(newCoordLE);
            ui->extraCoordinatesLayout->addLayout(coordStringLayout);
        }
        if (ui->extraCoordinatesLayout->count() > 1)
            ui->removePointBtn->setEnabled(true);
        else
            ui->removePointBtn->setEnabled(false);
        break;
    case TYPE_METRIC_RECT:
        ui->frontLE->setText(QString::number(qRound(obj.metrics[1].m_LATITUDE)));
        ui->depthLE->setText(QString::number(qRound(obj.metrics[1].m_LONGITUDE)));
        ui->deviationLE->setText(QString::number(qRound(obj.metrics[1].m_HEIGHT)));
        break;
    case TYPE_METRIC_CIRCLE:
        ui->radiusLE->setText(QString::number(qRound(obj.metrics[1].m_LATITUDE)));
        break;
    }
}

//!Метод инициализации формы для новой поражаемой цели
void HitTargetsTabForm::onAddSetup() {
    addCommonFormData();
    int index = ui->dataSourceBatteryCB->findData("1");
    ui->dataSourceBatteryCB->setCurrentIndex(index);
    int index2 = ui->dataSourceWeaponryCB->findData("1.11");
    ui->dataSourceWeaponryCB->setCurrentIndex(index2);
//    setCurrentDataSourceBattery();
//    setCurrentDataSourceWeaponry();
    ui->detectionTimeDTE->setDateTime(QDateTime::currentDateTime());

    ui->targetNumberLE->setEnabled(true);
    ui->targetNameCB->setEnabled(true);

    if (ui->randomRB->isChecked() == true) {
        slotToggleRandomRB();
    }
    else {
        ui->randomRB->setChecked(true);
    }
    slotAddPoint();
}

/*!
Метод инициализации формы для редактируемой поражаемой цели
\param[in] table верхняя таблица вкладки Навигатор
*/
void HitTargetsTabForm::onEditSetup(QTableWidget* table) {
    addCommonFormData();
    ui->dataSourceBatteryCB->setCurrentText(table->item(table->currentRow(), 2)->text().split('/').last());
    ui->dataSourceWeaponryCB->setCurrentText(table->item(table->currentRow(), 2)->text().split('/').first());
    ui->targetNumberLE->setText(table->item(table->currentRow(), 0)->text());
    ui->targetNameCB->setCurrentText(table->item(table->currentRow(), 1)->text());

    ui->targetNumberLE->setEnabled(false);
    ui->targetNameCB->setEnabled(false);

    QSqlQuery query = QSqlQuery(db);
    db.transaction();
    QString selectTargetsParam = "SELECT tp.id_target, importance, target_time, method_location, t.termname "
                                 "FROM targets.targets_param tp "
                                 "JOIN targets.obj_targets ot ON tp.id_target = ot.id_target "
                                 "JOIN reference_data.terms t ON tp.manner_tid = t.termhierarchy "
                                 "WHERE ot.target_number = ? "
                                 "     AND ot.target_name = ? "
                                 "     AND ot.combat_hierarchy = ? "
                                 "     AND ot.date_delete IS null";
    query.prepare(selectTargetsParam);
    query.addBindValue(ui->targetNumberLE->text());
    query.addBindValue(ui->targetNameCB->currentData());
    query.addBindValue(ui->dataSourceWeaponryCB->currentData());

    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    query.next();
    int targetID = query.value(0).toInt();
    ui->importanceLE->setText(query.value(1).toString());
    ui->detectionTimeDTE->setDateTime(query.value(2).toDateTime());
    ui->coverDegreeCB->setCurrentText(query.value(4).toString());

    switch (query.value(3).toInt()) {
    case 0: {
        if (ui->randomRB->isChecked() == true)
            slotToggleRandomRB();
        else
            ui->randomRB->setChecked(true);

        addFilledPoints();
        break;
    }
    case 1: {
        slotAddPoint();
        if (ui->squareRB->isChecked() == true)
            slotToggleSquareRB();
        else
            ui->squareRB->setChecked(true);

        QString squareQuery = "SELECT own_forces.coordinates_output(obj_location), front, depth, deviation "
                              "FROM targets.targets_param "
                              "WHERE id_target = ?";
        query.prepare(squareQuery);
        query.addBindValue(targetID);

        if (!query.exec()) {
            qDebug() << query.lastError();
        }
        query.next();

        ui->coordinateLE->setText(query.value(0).toString());
        ui->frontLE->setText(QString::number(query.value(1).toInt()));
        ui->depthLE->setText(QString::number(query.value(2).toInt()));
        ui->deviationLE->setText(QString::number(query.value(3).toInt()));
        break;
    }
    case 2: {
        slotAddPoint();
        if (ui->roundRB->isChecked() == true)
            slotToggleRoundRB();
        else
            ui->roundRB->setChecked(true);

        QString roundQuery = "SELECT own_forces.coordinates_output(obj_location), radius "
                             "FROM targets.targets_param "
                             "WHERE id_target = ?";
       query.prepare(roundQuery);
       query.addBindValue(targetID);

        if (!query.exec()) {
            qDebug() << query.lastError();
        }
        query.next();

        ui->coordinateLE->setText(query.value(0).toString());
        ui->radiusLE->setText(QString::number(query.value(1).toInt()));
        break;
    }
    }

    //! Запрос на получение метода и времени поражения
    QString selectDamageCondition = "SELECT method_destruction, stated_time "
                                    "FROM targets.damage_condition "
                                    "WHERE id_target = ?";
    query.prepare(selectDamageCondition);
    query.addBindValue(targetID);

    if (!query.exec()) {
        qDebug() << "Ошибка в запросе на получение метода и времени поражения" << query.lastError();
    }
    if (query.size() == 1) {
        query.next();

        if (query.value(1).toDateTime().isValid() && query.value(0).toBool() == true) {
            ui->explosionChB->setChecked(true);
            slotToggleExplosionCB();
        }
        else if (query.value(1).toDateTime().isValid() && query.value(0).toBool() == false) {
            ui->launchChB->setChecked(true);
            slotToggleLaunchCB();
        }
        ui->launchTimeDTE->setDateTime(query.value(1).toDateTime());
    }

    //! Запрос на получение степени поражения
    QString selectDamageDegree = "SELECT degree_tid "
                                 "FROM targets.damage_degree "
                                 "WHERE id_target = ?";
    query.prepare(selectDamageDegree);
    query.addBindValue(targetID);

    if (!query.exec()) {
        qDebug() << "Ошибка в запросе на получение степени поражения" << query.lastError();
    }

    query.next();
    ui->damageDegreeCB->setCurrentText(Utility::convertCodeToReferenceName(db, query.value(0).toString()));

    //! Запрос на получение типа и количества ракет
    QString selectStatedWeapons = "SELECT type_tid, amount "
                                  "FROM targets.stated_weapons "
                                  "WHERE id_target = ?";
    query.prepare(selectStatedWeapons);
    query.addBindValue(targetID);

    if (!query.exec()) {
        qDebug() << "Ошибка в запросе на получение типа и количества ракет" << query.lastError();
    }

    query.next();
    ui->rocketTypeCB->setCurrentText(Utility::convertCodeToReferenceName(db, query.value(0).toString()));
    ui->quantityLE->setText(QString::number(query.value(1).toInt()));

    if (!db.commit()) {
        QMessageBox::critical(this, "Ошибка", "Не все данные удалось получить!");
    }
}

/*!
\brief Метод сохранения заполненной формы в БД

Форма сохраняется как для новой, так и для редактируемой цели.
В обоих случаях координаты ложатся в БД четырьмя разными способами:
как линия, как ломаная, как прямоугольник и как круг (4 insert и 4 update запроса)
\return возвращает true, если сохранение прошло успешно, иначе false
*/
bool HitTargetsTabForm::onSaveSetup() {
    if (ui->targetNumberLE->text().isEmpty() || ui->importanceLE->text().isEmpty() ||
            !ui->coordinateLE->text().contains(QRegExp("\\d+")) || ui->quantityLE->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Не заполнено поле с координатами!");
        return false;
    }

    QSqlQuery query = QSqlQuery(db);

    if (ui->targetNameCB->isEnabled()) { //i.e. add tab
        db.transaction();

        //! Запрос на добавление данных номера, наименования цели и машины, ее обнаружившей
        QString insertObjTargets = "INSERT INTO targets.obj_targets (target_number, target_name, "
                                   "    combat_hierarchy, date_add, id_manager) "
                                   "VALUES (?, ?, ?, now(), ?)";
        query.prepare(insertObjTargets);
        query.addBindValue(ui->targetNumberLE->text());
        query.addBindValue(ui->targetNameCB->currentData());
        query.addBindValue(ui->dataSourceWeaponryCB->currentData());
        query.addBindValue(10); //костыль
        if (!query.exec()) {
            qDebug() << "Ошибка добавления данных номера и наименования цели в БД!" << query.lastError();
        }

        //! Запрос на получение айди только что добавленной цели
        QString selectJustAddedId = "SELECT MAX(id_target) "
                                    "FROM targets.obj_targets";
        query.prepare(selectJustAddedId);
        query.exec();
        query.next();
        int targetID = query.value(0).toInt();

        //! Запрос на получение кода классификатора из таблицы карты
        QString selectCode = "SELECT code "
                             "FROM map_objects.classifier_codes "
                             "WHERE object_name = ?";
        query.prepare(selectCode);
        query.addBindValue(ui->targetNameCB->currentText());
        if (!query.exec()) {
            qDebug() << "Ошибка получения кода классификатора!" << query.lastError();
        }
        query.next();
        int code = query.value(0).toInt();


        QString insertTargetsParam;
        QString insertMapObject;
        int mapObjectId;

        //! Запрос на получение айди только что добавленной цели из таблицы карты
        QString selectMapObjectId = "SELECT MAX(id) "
                                    "FROM map_objects.object_params";
        //! Запрос на получение данных только что добавленной цели
        QString selectMapObject = "SELECT id, code, object_name, geometry_type, enemy, visibility, access_level "
                                  "FROM map_objects.object_params "
                                  "WHERE id = ?";
        QString getLatLonAtiValues = "SELECT ST_X(geom), ST_Y(geom), ST_Z(geom) "
                                     "FROM (SELECT (ST_dumppoints(object_location)).geom "
                                     "      FROM map_objects.object_params"
                                     "      WHERE id = ?) AS foo";
        NetworkObject obj;

        if (ui->randomRB->isChecked()) {
            for (int i = 0; i < ui->extraCoordinatesLayout->count(); i++) {
                QLineEdit* coord = (QLineEdit*)ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(1)->widget();
                if (!coord->text().contains(QRegExp("\\d+"))) {
                    QMessageBox::warning(this, "Ошибка", "Не заполнены все поля с координатами!");
                    return false;
                }
            }

            QString insertTargetsParamPatt = "INSERT INTO targets.targets_param (id_target, target_time, "
                                             "      importance, method_location, obj_location, manner_tid, "
                                             "      tid, date_add, id_manager) "
                                             "VALUES (?, ?, ?, ?, %1, ?, ?, now(), ?)";
            QString insertMapObjectPatt = "INSERT INTO map_objects.object_params (code, object_name, geometry_type, "
                                          "     enemy, visibility, access_level, object_location) "
                                          "VALUES (?, ?, ?, ?, ?, ?, %1)";
            if (ui->extraCoordinatesLayout->count() >= 2) {
                //! Запрос на вставку в БД поражаемой цели, представленной полигоном
                insertTargetsParam = insertTargetsParamPatt.arg(getMakePolygonString());
                //! Запрос на вставку в таблицу для карты поражаемой цели, представленной полигоном
                insertMapObject = insertMapObjectPatt.arg(getMakePolygonString());
            }
            else {
                //! Запрос на вставку в БД поражаемой цели, представленной линией
                insertTargetsParam = insertTargetsParamPatt.arg(getMakeLineString());
                //! Запрос на вставку в таблицу для карты поражаемой цели, представленной линией
                insertMapObject = insertMapObjectPatt.arg(getMakeLineString());
            }

            query.prepare(insertTargetsParam);
            query.addBindValue(targetID);
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(0);
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(1);
            if (!query.exec()) {
                qDebug() << "Ошибка добавления данных координат в БД!" << query.lastError();
            }

            query.prepare(insertMapObject);
            query.addBindValue(1031010110); //костыль, должен быть code
            query.addBindValue(ui->targetNameCB->currentText());
            query.addBindValue(TYPE_METRIC_LINE);
            query.addBindValue(1);
            query.addBindValue(true);
            query.addBindValue(1);
            if (!query.exec()) {
                qDebug() << "Ошибка добавления цели в таблицу для карты!" << query.lastError();
            }

            query.prepare(selectMapObjectId);
            query.exec();
            query.next();
            mapObjectId = query.value(0).toInt();

            query.prepare(selectMapObject);
            query.addBindValue(mapObjectId);
            if (!query.exec()) {
                qDebug() << "Ошибка выбора данных цели из таблицы для карты!" << query.lastError();
            }
            query.next();
            obj.data.m_OBJECT_ID = query.value(0).toInt();
            obj.data.m_CODE = query.value(1).toInt();
            obj.data.m_NAME = query.value(2).toString();
            obj.data.m_TYPE_ID = query.value(3).toInt();
            obj.data.m_ENEMY = query.value(4).toInt();
            obj.data.m_VISIBLE = query.value(5).toBool();
            obj.data.m_ACCESS_LVL = query.value(6).toInt();

            query.prepare(getLatLonAtiValues);
            query.addBindValue(mapObjectId);
            query.exec();
            tagDataPoint point;
            while (query.next()) {
                point.m_LATITUDE = query.value(0).toDouble();
                point.m_LONGITUDE = query.value(1).toDouble();
                point.m_HEIGHT = query.value(2).toDouble();
                obj.metrics.append(point);
            }

            qDebug() << "onSave lat" << obj.metrics[0].m_LATITUDE;
            qDebug() << "onSave lon" << obj.metrics[0].m_LONGITUDE;
            qDebug() << "onSave ati" << obj.metrics[0].m_HEIGHT;

            qDebug() << "onSave lat2" << obj.metrics[1].m_LATITUDE;
            qDebug() << "onSave lon2" << obj.metrics[1].m_LONGITUDE;
            qDebug() << "onSave ati2" << obj.metrics[1].m_HEIGHT;
        }
        else if (ui->squareRB->isChecked()) {
            if (ui->frontLE->text().isEmpty() || ui->depthLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            //! Запрос на вставку в БД поражаемой цели, представленной прямоугольником
            insertTargetsParam = "INSERT INTO targets.targets_param (id_target, target_time, "
                                 "      importance, method_location, obj_location, front, "
                                 "      depth, deviation, manner_tid, tid, date_add, id_manager) "
                                 "VALUES (?, ?, ?, ?, own_forces.coordinates_input(?), ?, ?, ?, ?, ?, now(), ?)";

            query.prepare(insertTargetsParam);
            query.addBindValue(targetID);
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(1);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->frontLE->text());
            query.addBindValue(ui->depthLE->text());
            query.addBindValue(ui->deviationLE->text().toInt());
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(1);
            if (!query.exec()) {
                qDebug() << "Ошибка добавления данных координат и фронта/глубины в БД!" << query.lastError();
            }

            //! Запрос на вставку в таблицу для карты поражаемой цели, представленной прямоугольником
            insertMapObject = "INSERT INTO map_objects.object_params (code, object_name, geometry_type, "
                              "     enemy, visibility, access_level, object_location, front, depth, deviation) "
                              "VALUES (?, ?, ?, ?, ?, ?, own_forces.coordinates_input(?), ?, ?, ?)";
            query.prepare(insertMapObject);
            query.addBindValue(1031010110); //костыль, должен быть code
            query.addBindValue(ui->targetNameCB->currentText());
            query.addBindValue(TYPE_METRIC_RECT);
            query.addBindValue(1);
            query.addBindValue(true);
            query.addBindValue(1);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->frontLE->text());
            query.addBindValue(ui->depthLE->text());
            query.addBindValue(ui->deviationLE->text().toInt());
            if (!query.exec()) {
                qDebug() << "Ошибка добавления цели в таблицу для карты!" << query.lastError();
            }

            query.prepare(selectMapObjectId);
            query.exec();
            query.next();
            mapObjectId = query.value(0).toInt();

            query.prepare(selectMapObject);
            query.addBindValue(mapObjectId);
            if (!query.exec()) {
                qDebug() << "Ошибка выбора данных цели из таблицы для карты!" << query.lastError();
            }
            query.next();
            obj.data.m_OBJECT_ID = query.value(0).toInt();
            obj.data.m_CODE = query.value(1).toInt();
            obj.data.m_NAME = query.value(2).toString();
            obj.data.m_TYPE_ID = query.value(3).toInt();
            obj.data.m_ENEMY = query.value(4).toInt();
            obj.data.m_VISIBLE = query.value(5).toBool();
            obj.data.m_ACCESS_LVL = query.value(6).toInt();

            query.prepare(getLatLonAtiValues);
            query.addBindValue(mapObjectId);
            query.exec();
            query.next();
            tagDataPoint metrics, squareData;
            metrics.m_LATITUDE = query.value(0).toDouble();
            metrics.m_LONGITUDE = query.value(1).toDouble();
            metrics.m_HEIGHT = query.value(2).toDouble();
            squareData.m_LATITUDE = ui->frontLE->text().toDouble();
            squareData.m_LONGITUDE = ui->depthLE->text().toDouble();
            squareData.m_HEIGHT = ui->deviationLE->text().toDouble();
            obj.metrics.append(metrics);
            obj.metrics.append(squareData);

            qDebug() << "onSave lat" << obj.metrics[0].m_LATITUDE;
            qDebug() << "onSave lon" << obj.metrics[0].m_LONGITUDE;
            qDebug() << "onSave ati" << obj.metrics[0].m_HEIGHT;

            qDebug() << "onSave front" << obj.metrics[1].m_LATITUDE;
            qDebug() << "onSave depth" << obj.metrics[1].m_LONGITUDE;
            qDebug() << "onSave deviation" << obj.metrics[1].m_HEIGHT;
        }
        else if (ui->roundRB->isChecked()) {
            if (ui->radiusLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Не заполнено поле с радиусом!");
                return false;
            }
            //! Запрос на вставку в БД поражаемой цели, представленной кругом
            insertTargetsParam = "INSERT INTO targets.targets_param (id_target, target_time, "
                                 "      importance, method_location, obj_location, radius, "
                                 "      manner_tid, tid, date_add, id_manager) "
                                 "VALUES (?, ?, ?, ?, own_forces.coordinates_input(?), ?, ?, ?, now(), ?)";

            query.prepare(insertTargetsParam);
            query.addBindValue(targetID);
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(2);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->radiusLE->text());
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(1);
            if (!query.exec()) {
                qDebug() << "Ошибка добавления данных координат и радиуса в БД!" << query.lastError();
            }

            //! Запрос на вставку в таблицу для карты поражаемой цели, представленной кругом
            insertMapObject = "INSERT INTO map_objects.object_params (code, object_name, geometry_type, "
                              "     enemy, visibility, access_level, object_location, radius) "
                              "VALUES (?, ?, ?, ?, ?, ?, own_forces.coordinates_input(?), ?)";
            query.prepare(insertMapObject);
            query.addBindValue(1031010110); //костыль, должен быть code
            query.addBindValue(ui->targetNameCB->currentText());
            query.addBindValue(TYPE_METRIC_CIRCLE);
            query.addBindValue(1);
            query.addBindValue(true);
            query.addBindValue(1);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->radiusLE->text());
            if (!query.exec()) {
                qDebug() << "Ошибка добавления цели в таблицу для карты!" << query.lastError();
            }

            query.prepare(selectMapObjectId);
            query.exec();
            query.next();
            mapObjectId = query.value(0).toInt();

            query.prepare(selectMapObject);
            query.addBindValue(mapObjectId);
            if (!query.exec()) {
                qDebug() << "Ошибка выбора данных цели из таблицы для карты!" << query.lastError();
            }
            query.next();
            obj.data.m_OBJECT_ID = query.value(0).toInt();
            obj.data.m_CODE = query.value(1).toInt();
            obj.data.m_NAME = query.value(2).toString();
            obj.data.m_TYPE_ID = query.value(3).toInt();
            obj.data.m_ENEMY = query.value(4).toInt();
            obj.data.m_VISIBLE = query.value(5).toBool();
            obj.data.m_ACCESS_LVL = query.value(6).toInt();

            query.prepare(getLatLonAtiValues);
            query.addBindValue(mapObjectId);
            query.exec();
            query.next();
            tagDataPoint metrics, roundData;
            metrics.m_LATITUDE = query.value(0).toDouble();
            metrics.m_LONGITUDE = query.value(1).toDouble();
            metrics.m_HEIGHT = query.value(2).toDouble();
            roundData.m_LATITUDE = ui->radiusLE->text().toDouble();
            obj.metrics.append(metrics);
            obj.metrics.append(roundData);

            qDebug() << "onSave lat" << obj.metrics[0].m_LATITUDE;
            qDebug() << "onSave lon" << obj.metrics[0].m_LONGITUDE;
            qDebug() << "onSave ati" << obj.metrics[0].m_HEIGHT;

            qDebug() << "onSave radius" << obj.metrics[1].m_LATITUDE;
        }
        QByteArray mas = NetworkModule::Instance().maskData(NETWORK_INSERT_OBJECT_FROM_DB, obj.serialize());
        NetworkModule::Instance().sendDataFromMap(mas);

        if (ui->launchTimeDTE->isEnabled()) {
            //! Запрос на добавление данных метода и времени запуска в БД
            QString insertDamageCondition = "INSERT INTO targets.damage_condition (id_target, method_destruction, "
                                            "       stated_time, tid, date_add, id_manager) "
                                            "VALUES (?, ?, ?, ?, now(), ?)";
            query.prepare(insertDamageCondition);
            query.addBindValue(targetID);
            query.addBindValue(ui->explosionChB->isChecked());
            query.addBindValue(ui->launchTimeDTE->text());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(1);
            if (!query.exec()) {
                qDebug() << "Ошибка добавления данных метода и времени запуска в БД!" << query.lastError();
            }
        }

        //! Запрос на добавление данных степени поражения в БД
        QString insertDamageDegree = "INSERT INTO targets.damage_degree (id_target, degree_tid, "
                                     "      tid, date_add, id_manager) "
                                     "VALUES (?, ?, ?, now(), ?)";
        query.prepare(insertDamageDegree);
        query.addBindValue(targetID);
        query.addBindValue(ui->damageDegreeCB->currentData());
        query.addBindValue(Utility::getTid(db));
        query.addBindValue(1);
        if (!query.exec()) {
            qDebug() << "Ошибка добавления данных степени поражения в БД!" << query.lastError();
        }

        //! Запрос на добавление данных типа и количества ракет в БД
        QString insertStatedWeapons = "INSERT INTO targets.stated_weapons (id_target, type_tid, "
                                      "     amount, tid, date_add, id_manager) "
                                      "VALUES (?, ?, ?, ?, now(), ?)";
        query.prepare(insertStatedWeapons);
        query.addBindValue(targetID);
        query.addBindValue(ui->rocketTypeCB->currentData());
        query.addBindValue(ui->quantityLE->text());
        query.addBindValue(Utility::getTid(db));
        query.addBindValue(1);
        if (!query.exec()) {
            qDebug() << "Ошибка добавления данных типа и количества ракет в БД!" << query.lastError();
        }

        if (!db.commit()) {
            qDebug() << query.lastError();
            QMessageBox::critical(this, "Ошибка", "Добавить данные не удалось!");
        }
        else {
            QMessageBox::information(this, "Уведомление", "Добавление прошло успешно!");
            emit insertMapObjectCompleted();
            return true;
        }
    }
    else if (!ui->targetNameCB->isEnabled()) { //i.e. edit tab
        db.transaction();

        //! Запрос на получение айди цели по выбранному номеру, наименованию цели и машине, ее обнаружившей
        QString selectEditableId = "SELECT id_target "
                                   "FROM targets.obj_targets "
                                   "WHERE target_number = ? "
                                   "    AND target_name = ? "
                                   "    AND combat_hierarchy = ? "
                                   "    AND date_delete IS null";
        query.prepare(selectEditableId);
        query.addBindValue(ui->targetNumberLE->text());
        query.addBindValue(ui->targetNameCB->currentData());
        query.addBindValue(ui->dataSourceWeaponryCB->currentData());
        if (!query.exec()) {
            qDebug() << "Ошибка в выборе айди цели!" << query.lastError();
        }
        query.next();
        int targetID = query.value(0).toInt();

        //! Запрос на обновление даты изменения по айди цели в таблице obj_targets
        QString updateObjectTime = "UPDATE targets.obj_targets "
                                   "SET date_edit = now() "
                                   "WHERE id_target = ?";
        query.prepare(updateObjectTime);
        query.addBindValue(targetID);
        query.exec();


        //! Запрос на получение айди обновляемой цели из таблицы карты
        QString selectMapObjectId = "SELECT id "
                                    "FROM map_objects.object_params op "
                                    "JOIN targets.obj_targets ot on op.create_time = ot.date_add "
                                    "WHERE ot.id_target = ?";
        query.prepare(selectMapObjectId);
        query.addBindValue(targetID);
        query.exec();
        query.next();
        int mapObjectId = query.value(0).toInt();


        //! Запрос на получение данных обновляемой цели
        QString selectMapObject = "SELECT id, code, object_name, geometry_type, enemy, visibility, access_level "
                                  "FROM map_objects.object_params "
                                  "WHERE id = ?";
        QString getLatLonAtiValues = "SELECT ST_X(geom), ST_Y(geom), ST_Z(geom) "
                                       "FROM (SELECT (ST_dumppoints(object_location)).geom "
                                       "      FROM map_objects.object_params"
                                       "      WHERE id = ?) AS foo";
        NetworkObject obj;
        QString updateTargetsParam;
        QString updateMapObject;

        if (ui->randomRB->isChecked()) {
            for (int i = 0; i < ui->extraCoordinatesLayout->count(); i++) {
                QLineEdit* coord = (QLineEdit*)ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(1)->widget();
                if (!coord->text().contains(QRegExp("\\d+"))) {
                    QMessageBox::warning(this, "Ошибка", "Не заполнены все поля с координатами!");
                    return false;
                }
            }

            QString updateTargetsParamPatt = "UPDATE targets.targets_param "
                                             "SET target_time = ?, importance = ?, method_location = ?, "
                                             "       obj_location = %1, front = null, depth = null, "
                                             "       deviation = null, radius = null, manner_tid = ?, "
                                             "       tid = ?, date_edit = now() "
                                             "WHERE id_target = ?";
            QString updateMapObjectPatt = "UPDATE map_objects.object_params "
                                          "SET geometry_type = ?, object_location = %1, front = null, "
                                          "     depth = null, deviation = null, radius = null, update_time = now() "
                                          "WHERE id = ?";
            if (ui->extraCoordinatesLayout->count() >= 2) {
                //! Запрос на обновление в БД поражаемой цели, представленной полигоном
                updateTargetsParam = updateTargetsParamPatt.arg(getMakePolygonString());
                //! Запрос на обновление в таблице для карты поражаемой цели, представленной полигоном
                updateMapObject = updateMapObjectPatt.arg(getMakePolygonString());
            }
            else {
                //! Запрос на обновление в БД поражаемой цели, представленной линией
                updateTargetsParam = updateTargetsParamPatt.arg(getMakeLineString());
                //! Запрос на обновление в таблице для карты поражаемой цели, представленной линией
                updateMapObject = updateMapObjectPatt.arg(getMakeLineString());
            }

            query.prepare(updateTargetsParam);
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(0);
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(targetID);
            if (!query.exec()) {
                qDebug() << "Ошибка в запросе на обновление цели произвольной геометрии!" << query.lastError();
            }

            query.prepare(updateMapObject);
            query.addBindValue(TYPE_METRIC_LINE);
            query.addBindValue(mapObjectId);
            if (!query.exec()) {
                qDebug() << "Ошибка обновления цели в таблице для карты!" << query.lastError();
            }

            query.prepare(selectMapObject);
            query.addBindValue(mapObjectId);
            if (!query.exec()) {
                qDebug() << "Ошибка выбора данных цели из таблицы для карты!" << query.lastError();
            }
            query.next();
            obj.data.m_OBJECT_ID = query.value(0).toInt();
            obj.data.m_CODE = query.value(1).toInt();
            obj.data.m_NAME = query.value(2).toString();
            obj.data.m_TYPE_ID = query.value(3).toInt();
            obj.data.m_ENEMY = query.value(4).toInt();
            obj.data.m_VISIBLE = query.value(5).toBool();
            obj.data.m_ACCESS_LVL = query.value(6).toInt();

            query.prepare(getLatLonAtiValues);
            query.addBindValue(mapObjectId);
            query.exec();
            tagDataPoint point;
            while (query.next()) {
                point.m_LATITUDE = query.value(0).toDouble();
                point.m_LONGITUDE = query.value(1).toDouble();
                point.m_HEIGHT = query.value(2).toDouble();
                obj.metrics.append(point);
            }

            qDebug() << "onSave lat" << obj.metrics[0].m_LATITUDE;
            qDebug() << "onSave lon" << obj.metrics[0].m_LONGITUDE;
            qDebug() << "onSave ati" << obj.metrics[0].m_HEIGHT;

            qDebug() << "onSave lat2" << obj.metrics[1].m_LATITUDE;
            qDebug() << "onSave lon2" << obj.metrics[1].m_LONGITUDE;
            qDebug() << "onSave ati2" << obj.metrics[1].m_HEIGHT;
        }
        else if (ui->squareRB->isChecked()) {
            if (ui->frontLE->text().isEmpty() || ui->depthLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            //!Запрос на обновление в БД поражаемой цели, представленной прямоугольником
            updateTargetsParam = "UPDATE targets.targets_param "
                                 "SET target_time = ?, importance = ?, method_location = ?, "
                                 "       obj_location = own_forces.coordinates_input(?), front = ?, "
                                 "       depth = ?, deviation = ?, radius = null, manner_tid = ?, "
                                 "       tid = ?, date_edit = now() "
                                 "WHERE id_target = ?";
            query.prepare(updateTargetsParam);
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(1);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->frontLE->text());
            query.addBindValue(ui->depthLE->text());
            query.addBindValue(ui->deviationLE->text().toInt());
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(targetID);
            if (!query.exec()) {
                qDebug() << "Ошибка в запросе на обновление прямоугольной цели!" << query.lastError();
            }

            //! Запрос на обновление в таблице для карты поражаемой цели, представленной прямоугольником
            updateMapObject = "UPDATE map_objects.object_params "
                              "SET geometry_type = ?, object_location = own_forces.coordinates_input(?), "
                              "     front = ?, depth = ?, deviation = ?, radius = null, update_time = now() "
                              "WHERE id = ?";
            query.prepare(updateMapObject);
            query.addBindValue(TYPE_METRIC_RECT);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->frontLE->text().toDouble());
            query.addBindValue(ui->depthLE->text().toDouble());
            query.addBindValue(ui->deviationLE->text().toDouble());
            query.addBindValue(mapObjectId);
            if (!query.exec()) {
                qDebug() << "Ошибка обновления цели в таблице для карты!" << query.lastError();
            }

            query.prepare(selectMapObject);
            query.addBindValue(mapObjectId);
            if (!query.exec()) {
                qDebug() << "Ошибка выбора данных цели из таблицы для карты!" << query.lastError();
            }
            query.next();
            obj.data.m_OBJECT_ID = query.value(0).toInt();
            obj.data.m_CODE = query.value(1).toInt();
            obj.data.m_NAME = query.value(2).toString();
            obj.data.m_TYPE_ID = query.value(3).toInt();
            obj.data.m_ENEMY = query.value(4).toInt();
            obj.data.m_VISIBLE = query.value(5).toBool();
            obj.data.m_ACCESS_LVL = query.value(6).toInt();

            query.prepare(getLatLonAtiValues);
            query.addBindValue(mapObjectId);
            query.exec();
            query.next();
            tagDataPoint metrics, squareData;
            metrics.m_LATITUDE = query.value(0).toDouble();
            metrics.m_LONGITUDE = query.value(1).toDouble();
            metrics.m_HEIGHT = query.value(2).toDouble();
            squareData.m_LATITUDE = ui->frontLE->text().toDouble();
            squareData.m_LONGITUDE = ui->depthLE->text().toDouble();
            squareData.m_HEIGHT = ui->deviationLE->text().toDouble();
            obj.metrics.append(metrics);
            obj.metrics.append(squareData);

            qDebug() << "onSave lat" << obj.metrics[0].m_LATITUDE;
            qDebug() << "onSave lon" << obj.metrics[0].m_LONGITUDE;
            qDebug() << "onSave ati" << obj.metrics[0].m_HEIGHT;

            qDebug() << "onSave front" << obj.metrics[1].m_LATITUDE;
            qDebug() << "onSave depth" << obj.metrics[1].m_LONGITUDE;
            qDebug() << "onSave deviation" << obj.metrics[1].m_HEIGHT;
        }
        else if (ui->roundRB->isChecked()) {
            if (ui->radiusLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Не заполнено поле с радиусом!");
                return false;
            }
            //!Запрос на обновление в БД поражаемой цели, представленной кругом
            updateTargetsParam = "UPDATE targets.targets_param "
                                 "SET target_time = ?, importance = ?, method_location = ?, "
                                 "       obj_location = own_forces.coordinates_input(?), front = null, "
                                 "       depth = null, deviation = null, radius = ?, manner_tid = ?, "
                                 "       tid = ?, date_edit = now() "
                                 "WHERE id_target = ?";
            query.prepare(updateTargetsParam);
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(2);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->radiusLE->text());
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(targetID);
            if (!query.exec()) {
                qDebug() << "Ошибка в запросе на обновление круговой цели!" << query.lastError();
            }

            //! Запрос на обновление в таблице для карты поражаемой цели, представленной кругом
            updateMapObject = "UPDATE map_objects.object_params "
                              "SET geometry_type = ?, object_location = own_forces.coordinates_input(?), "
                              "     front = null, depth = null, deviation = null, radius = ?, update_time = now() "
                              "WHERE id = ?";
            query.prepare(updateMapObject);
            query.addBindValue(TYPE_METRIC_CIRCLE);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->radiusLE->text().toDouble());
            query.addBindValue(mapObjectId);
            if (!query.exec()) {
                qDebug() << "Ошибка обновления цели в таблице для карты!" << query.lastError();
            }

            query.prepare(selectMapObject);
            query.addBindValue(mapObjectId);
            if (!query.exec()) {
                qDebug() << "Ошибка выбора данных цели из таблицы для карты!" << query.lastError();
            }
            query.next();
            obj.data.m_OBJECT_ID = query.value(0).toInt();
            obj.data.m_CODE = query.value(1).toInt();
            obj.data.m_NAME = query.value(2).toString();
            obj.data.m_TYPE_ID = query.value(3).toInt();
            obj.data.m_ENEMY = query.value(4).toInt();
            obj.data.m_VISIBLE = query.value(5).toBool();
            obj.data.m_ACCESS_LVL = query.value(6).toInt();

            query.prepare(getLatLonAtiValues);
            query.addBindValue(mapObjectId);
            query.exec();
            query.next();
            tagDataPoint metrics, roundData;
            metrics.m_LATITUDE = query.value(0).toDouble();
            metrics.m_LONGITUDE = query.value(1).toDouble();
            metrics.m_HEIGHT = query.value(2).toDouble();
            roundData.m_LATITUDE = ui->radiusLE->text().toDouble();
            obj.metrics.append(metrics);
            obj.metrics.append(roundData);

            qDebug() << "onSave lat" << obj.metrics[0].m_LATITUDE;
            qDebug() << "onSave lon" << obj.metrics[0].m_LONGITUDE;
            qDebug() << "onSave ati" << obj.metrics[0].m_HEIGHT;

            qDebug() << "onSave radius" << obj.metrics[1].m_LATITUDE;
        }   
        map->removeObjectFromManager(obj);
        QByteArray refreshMap = NetworkModule::Instance().maskData(NETWORK_SEND_MAP, map->getObjectManager().serialize());
        NetworkModule::Instance().sendDataFromMap(refreshMap);
        QByteArray newObject = NetworkModule::Instance().maskData(NETWORK_INSERT_OBJECT_FROM_DB, obj.serialize()); //NETWORK_UPDATE_OBJECT на карте еще не реализовано, поэтому пока что через удаление и создание нового
        NetworkModule::Instance().sendDataFromMap(newObject);

        if (ui->launchTimeDTE->isEnabled()) {
            //! Проверка наличия айди редактируемой цели в таблице damage_condition
            QString checkForIdInDamageCondition = "SELECT 1 "
                                                  "FROM targets.damage_condition "
                                                  "WHERE id_target = ?";
            query.prepare(checkForIdInDamageCondition);
            query.addBindValue(targetID);
            query.exec();
            query.next();

            if (query.size() == 1) {
                //! Запрос на обновление данных метода и времени запуска в БД
                QString updateDamageCondition = "UPDATE targets.damage_condition "
                                                "SET method_destruction = ?, stated_time = ?, date_edit = now() "
                                                "WHERE id_target = ?";
                query.prepare(updateDamageCondition);
                query.addBindValue(ui->explosionChB->isChecked());
                query.addBindValue(ui->launchTimeDTE->text());
                query.addBindValue(targetID);
                if (!query.exec()) {
                    qDebug() << "Ошибка обновления данных метода и времени запуска в БД!" << query.lastError();
                }
            }
            else if (query.size() == 0) {
                //! Запрос на добавление данных метода и времени запуска в БД
                QString insertDamageCondition = "INSERT INTO targets.damage_condition (id_target, method_destruction, "
                                                "       stated_time, tid, date_add, id_manager) "
                                                "VALUES (?, ?, ?, ?, now(), ?)";
                query.prepare(insertDamageCondition);
                query.addBindValue(targetID);
                query.addBindValue(ui->explosionChB->isChecked());
                query.addBindValue(ui->launchTimeDTE->text());
                query.addBindValue(Utility::getTid(db));
                query.addBindValue(1);
                if (!query.exec()) {
                    qDebug() << "Ошибка добавления данных метода и времени запуска в БД!" << query.lastError();
                }
            }
        }

        //! Запрос на обновление данных степени поражения в БД
        QString updateDamageDegree = "UPDATE targets.damage_degree "
                                     "SET degree_tid = ?, date_edit = now() "
                                     "WHERE id_target = ?";
        query.prepare(updateDamageDegree);
        query.addBindValue(ui->damageDegreeCB->currentData());
        query.addBindValue(targetID);
        if (!query.exec()) {
            qDebug() << "Ошибка обновления данных степени поражения в БД!" << query.lastError();
        }

        //! Запрос на обновление данных типа и количества ракет в БД
        QString updateStatedWeapons = "UPDATE targets.stated_weapons "
                                      "SET type_tid = ?, amount = ?, date_edit = now() "
                                      "WHERE id_target = ?";
        query.prepare(updateStatedWeapons);
        query.addBindValue(ui->rocketTypeCB->currentData());
        query.addBindValue(ui->quantityLE->text());
        query.addBindValue(targetID);
        if (!query.exec()) {
            qDebug() << "Ошибка обновления данных типа и количества ракет в БД!" << query.lastError();
        }

        if (!db.commit()) {
            qDebug() << query.lastError();
            QMessageBox::critical(this, "Ошибка", "Обновить данные не удалось!");
        }
        else {
            QMessageBox::information(this, "Уведомление", "Обновление прошло успешно!");
            return true;
        }
    }
    return false;
}

//!Слот обработки нажатия на кнопку Добавить точку
void HitTargetsTabForm::slotAddPoint() {
    QLabel* newCoordLbl = new QLabel("Координата");
    QLineEdit* newCoordLE = new QLineEdit;

    newCoordLbl->setFont(font);
    newCoordLE->setInputMask("99°99'99.99\''A 999°99'99.99\''A 9999.9;_");

    QHBoxLayout* coordStringLayout = new QHBoxLayout;
    coordStringLayout->addWidget(newCoordLbl);
    coordStringLayout->addWidget(newCoordLE);
    ui->extraCoordinatesLayout->addLayout(coordStringLayout);

    if (ui->extraCoordinatesLayout->count() > 1)
        ui->removePointBtn->setEnabled(true);
}

//!Слот обработки нажатия на кнопку Убрать точку
void HitTargetsTabForm::slotRemovePoint() {
    QLayout* layout = ui->extraCoordinatesLayout->itemAt(ui->extraCoordinatesLayout->count() - 1)->layout();
    for (int i = 1; i >= 0; i--) {
        delete layout->takeAt(i)->widget();
        delete layout->takeAt(i);
    }
    ui->extraCoordinatesLayout->removeItem(layout);

    if (ui->extraCoordinatesLayout->count() < 2)
        ui->removePointBtn->setEnabled(false);
}

//!Метод заполнения координатами полей при просмотре/правке цели с произвольной геометрией
void HitTargetsTabForm::addFilledPoints() {
    QSqlQuery query = QSqlQuery(db);

    db.transaction();
    QString selectHexCoordsQuery = "SELECT (ST_dumppoints(obj_location)).geom "
                                   "FROM targets.targets_param tp "
                                   "JOIN targets.obj_targets ot ON tp.id_target = ot.id_target "
                                   "WHERE ot.target_number = ? "
                                   "     AND ot.target_name = ? "
                                   "     AND ot.combat_hierarchy = ? "
                                   "     AND ot.date_delete IS null";
    query.prepare(selectHexCoordsQuery);
    query.addBindValue(ui->targetNumberLE->text());
    query.addBindValue(ui->targetNameCB->currentData());
    query.addBindValue(ui->dataSourceWeaponryCB->currentData());

    if (!query.exec()) {
        qDebug() << "Unable to select hex coordinates!" << query.lastError();
    }
    QStringList hexCoords;
    while (query.next()) {
        hexCoords << query.value(0).toString();
    }
//    qDebug() << hexCoords;


    QString selectParsedCoordsQuery = "SELECT ";
    QString funcString = "own_forces.coordinates_output(?)";
    for (int i = 0; i < hexCoords.size() - 1; i++) {
        selectParsedCoordsQuery.append(funcString).append(", ");
    }
    selectParsedCoordsQuery.append(funcString);
//    qDebug() << "selectParsedCoordsQuery: " << selectParsedCoordsQuery << endl;

    query.prepare(selectParsedCoordsQuery);
    for (int i = 0; i < hexCoords.size(); i++) {
        query.addBindValue(hexCoords.at(i));
    }
    if (!query.exec()) {
        qDebug() << "Unable to select parsed coordinates!" << query.lastError();
    }
    db.commit();
    query.next();

    ui->coordinateLE->setText(query.value(0).toString());
    QHBoxLayout* coordStringLayout;
    QLabel* newCoordLbl;
    QLineEdit* newCoordLE;

    int i = 1;
    do {
        newCoordLbl = new QLabel("Координата");
        newCoordLbl->setFont(font);

        newCoordLE = new QLineEdit(query.value(i).toString());
        newCoordLE->setInputMask("99°99'99.99\''A 999°99'99.99\''A 9999.9;_");

        coordStringLayout = new QHBoxLayout;
        coordStringLayout->addWidget(newCoordLbl);
        coordStringLayout->addWidget(newCoordLE);
        ui->extraCoordinatesLayout->addLayout(coordStringLayout);

        i++;
    } while (i < query.record().count() - 1);

    if (ui->extraCoordinatesLayout->count() > 1)
        ui->removePointBtn->setEnabled(true);
    else
        ui->removePointBtn->setEnabled(false);
}

/*!
 * Слот обработки изменения индекса комбобокса Источника данных (батареи)
 * \param n индекс комбобокса
 */
void HitTargetsTabForm::slotChangeDataSourceBattery(int n) {
    if (n != -1) {
        ui->dataSourceWeaponryCB->clear();
        getDataSourceWeaponry();
    }
}

//!Слот обработки нажатия на радиокнопку Произвольная
void HitTargetsTabForm::slotToggleRandomRB() {
    ui->addPointBtn->setHidden(false);
    ui->removePointBtn->setHidden(false);

    for (int i = 0; i < ui->extraCoordinatesLayout->count(); i++) {
        for (int j = 0; j < 2; j++) {
            ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(j)->widget()->show();
        }
    }

    ui->depthLbl->setHidden(true);
    ui->frontLbl->setHidden(true);
    ui->deviationLbl->setHidden(true);
    ui->depthLE->setHidden(true);
    ui->frontLE->setHidden(true);
    ui->deviationLE->setHidden(true);

    ui->radiusLbl->setHidden(true);
    ui->radiusLE->setHidden(true);
}

//!Слот обработки нажатия на радиокнопку Прямоугольник
void HitTargetsTabForm::slotToggleSquareRB() {
    ui->addPointBtn->setHidden(true);
    ui->removePointBtn->setHidden(true);

    for (int i = 0; i < ui->extraCoordinatesLayout->count(); i++) {
        for (int j = 0; j < 2; j++) {
            ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(j)->widget()->hide();
        }
    }

    ui->depthLbl->setHidden(false);
    ui->frontLbl->setHidden(false);
    ui->deviationLbl->setHidden(false);
    ui->depthLE->setHidden(false);
    ui->frontLE->setHidden(false);
    ui->deviationLE->setHidden(false);

    ui->radiusLbl->setHidden(true);
    ui->radiusLE->setHidden(true);
}

//!Слот обработки нажатия на радиокнопку Круговая
void HitTargetsTabForm::slotToggleRoundRB() {
    ui->addPointBtn->setHidden(true);
    ui->removePointBtn->setHidden(true);

    for (int i = 0; i < ui->extraCoordinatesLayout->count(); i++) {
        for (int j = 0; j < 2; j++) {
            ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(j)->widget()->hide();
        }
    }

    ui->depthLbl->setHidden(true);
    ui->frontLbl->setHidden(true);
    ui->deviationLbl->setHidden(true);
    ui->depthLE->setHidden(true);
    ui->frontLE->setHidden(true);
    ui->deviationLE->setHidden(true);

    ui->radiusLbl->setHidden(false);
    ui->radiusLE->setHidden(false);
}

//!Слот обработки нажатия на чекбокс Пуск
void HitTargetsTabForm::slotToggleLaunchCB() {
    if (ui->launchChB->isChecked()) {
        ui->launchTimeDTE->setEnabled(true);
        if (ui->explosionChB->isChecked()) {
            ui->explosionChB->setChecked(false);
        }
        else {
            ui->launchTimeDTE->setDateTime(QDateTime::currentDateTime());
        }
    }
    else {
        ui->launchTimeDTE->setEnabled(false);
    }
}

//!Слот обработки нажатия на чекбокс Взрыв
void HitTargetsTabForm::slotToggleExplosionCB() {
    if (ui->explosionChB->isChecked()) {
        ui->launchTimeDTE->setEnabled(true);
        if (ui->launchChB->isChecked()) {
            ui->launchChB->setChecked(false);
        }
        else {
            ui->launchTimeDTE->setDateTime(QDateTime::currentDateTime());
        }
    }
    else {
        ui->launchTimeDTE->setEnabled(false);
    }
}

//!Метод заполнения информацией общих для новой и редактируемой цели виджетов
void HitTargetsTabForm::addCommonFormData() {
    getDataSourceBatteries();
    getHitTargets();
    getCoverDegrees();
    getDamageDegrees();
    getRocketTypes();
}

/*!
 * Метод создает строку, формирующую линию (ST_MakeLine)
 * из имеющихся двух точек, для записи в БД
 * \return сформированная строка
 */
QString HitTargetsTabForm::getMakeLineString() {
    QString selectHexCoordsQuery = "SELECT own_forces.coordinates_input(?), own_forces.coordinates_input(?)";

    QSqlQuery query = QSqlQuery(db);
    query.prepare(selectHexCoordsQuery);
    query.addBindValue(ui->coordinateLE->text());
    QLineEdit* coord = (QLineEdit*)ui->extraCoordinatesLayout->itemAt(0)->layout()->itemAt(1)->widget();
    query.addBindValue(coord->text());
    query.exec();

    query.next();
    QStringList strList;
    for (int i = 0; i < query.record().count(); i++) {
        strList << query.value(i).toString();
    }

    QString makeLineString = "ST_MakeLine(";
    makeLineString.append("'").append(strList.first()).append("', ");
    makeLineString.append("'").append(strList.last()).append("')");
    return makeLineString;
}

/*!
 * Метод создает строку, формирующую полигон (ST_MakePolygon)
 * из имеющихся трех и более точек, для записи в БД
 * \return сформированная строка
 */
QString HitTargetsTabForm::getMakePolygonString() {
    QString selectHexCoordsQuery = "SELECT ";
    QString funcString = "own_forces.coordinates_input(?)";
    for (int i = 0; i < ui->extraCoordinatesLayout->count() + 1; i++) {
        selectHexCoordsQuery.append(funcString).append(", ");
    }
    selectHexCoordsQuery.append(funcString);
//    qDebug() << "selectHexCoordsQuery: " << selectHexCoordsQuery << endl;

    QSqlQuery query = QSqlQuery(db);
    query.prepare(selectHexCoordsQuery);
    query.addBindValue(ui->coordinateLE->text());
    for (int i = 0; i < ui->extraCoordinatesLayout->count(); i++) {
        QLineEdit* coord = (QLineEdit*)ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(1)->widget();
        query.addBindValue(coord->text());
    }
    query.addBindValue(ui->coordinateLE->text());
    query.exec();

    query.next();
    QStringList strList;
    for (int i = 0; i < query.record().count(); i++) {
        strList << query.value(i).toString();
    }
//    qDebug() << "strList: " << strList << endl;

    QString makePolygonString = "ST_MakePolygon(ST_MakeLine(ARRAY[";
    makePolygonString.append("'").append(strList.first()).append("', ");
    for (int i = 0; i < ui->extraCoordinatesLayout->count(); i++) {
        makePolygonString.append("'").append(strList.at(i+1)).append("'").append(", ");
    }
    makePolygonString.append("'").append(strList.last()).append("']))");
//    qDebug() << "makePolygonString: " << makePolygonString;
    return makePolygonString;
}

//!Метод заполнения комбобокса списком батарей
void HitTargetsTabForm::getDataSourceBatteries() {
    QSqlQuery query = QSqlQuery(db);
    QString selectQuery = "SELECT object_number, termname, combat_hierarchy "
                          "FROM own_forces.combatstructure "
                          "JOIN reference_data.terms ON termhierarchy = object_name "
                          "WHERE combat_hierarchy IN (SELECT combat_hierarchy "
                          "                           FROM own_forces.combatstructure "
                          "                           WHERE nlevel(combat_hierarchy) = 1 "
                          "                                 AND date_delete is null) ";
    query.prepare(selectQuery);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    while (query.next()) {
        ui->dataSourceBatteryCB->addItem(tr("%1").arg(query.value(0).toInt()) + " " + query.value(1).toString(),
                                         query.value(2).toString());
    }
}

//!Метод заполнения комбобокса списком боевых машин определенной батареи
void HitTargetsTabForm::getDataSourceWeaponry() {
    QSqlQuery query = QSqlQuery(db);
    QString selectQuery = "SELECT object_number, termname, combat_hierarchy "
                          "FROM own_forces.combatstructure "
                          "JOIN reference_data.terms ON termhierarchy = object_name "
                          "WHERE subltree(combat_hierarchy, 0, 1) = ? "
                          "     AND nlevel(combat_hierarchy) = 2 ";
    query.prepare(selectQuery);
    query.addBindValue(ui->dataSourceBatteryCB->currentData());
    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    while (query.next()) {
        ui->dataSourceWeaponryCB->addItem(tr("%1").arg(query.value(0).toInt()) + " " + query.value(1).toString(),
                                          query.value(2).toString());
    }
}

//!Метод выбора текущей батареи
void HitTargetsTabForm::setCurrentDataSourceBattery() {
    int index = ui->dataSourceBatteryCB->findData(combatHierarchy.left(1));
    ui->dataSourceBatteryCB->setCurrentIndex(index);
}

//!Метод выбора текущей боевой машины
void HitTargetsTabForm::setCurrentDataSourceWeaponry() {
    int index = ui->dataSourceWeaponryCB->findData(combatHierarchy);
    ui->dataSourceWeaponryCB->setCurrentIndex(index);
}

//!Метод заполнения комбобокса списком поражаемых целей (на интерфейсе поле Наименование)
void HitTargetsTabForm::getHitTargets() {
    QSqlQuery query = QSqlQuery(db);
    QString selectQuery = "SELECT termhierarchy, termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '90.*' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << query.lastError();
    }

    while (query.next()) {
        ui->targetNameCB->addItem(query.value(1).toString(), query.value(0).toString());
    }
}

//!Метод заполнения комбобокса списком степеней укрытия
void HitTargetsTabForm::getCoverDegrees() {
    QSqlQuery query = QSqlQuery(db);
    QString selectQuery = "SELECT termhierarchy, termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '95.20.*' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << query.lastError();
    }

    while (query.next()) {
        ui->coverDegreeCB->addItem(query.value(1).toString(), query.value(0).toString());
    }
}

//!Метод заполнения комбобокса списком степеней поражения
void HitTargetsTabForm::getDamageDegrees() {
    QSqlQuery query = QSqlQuery(db);
    QString selectQuery = "SELECT termhierarchy, termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '95.10.*' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << query.lastError();
    }

    while (query.next()) {
        ui->damageDegreeCB->addItem(query.value(1).toString(), query.value(0).toString());
    }
}

//!Метод заполнения комбобокса списком типов ракет
void HitTargetsTabForm::getRocketTypes() {
    QSqlQuery query = QSqlQuery(db);
    QString selectQuery = "SELECT termhierarchy, termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '51.50.*' "
                          "     AND termhierarchy != '51.50.30' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << query.lastError();
    }

    while (query.next()) {
        ui->rocketTypeCB->addItem(query.value(1).toString(), query.value(0).toString());
    }
}


HitTargetsTabForm::~HitTargetsTabForm()
{
    delete ui;
}
