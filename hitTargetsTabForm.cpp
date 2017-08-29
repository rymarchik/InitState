#include "hitTargetsTabForm.h"
#include "ui_hittargets.h"
#include "utility.h"

#include "mapsrc/networkmodule.h"
#include "mapsrc/NetworkObject.h"

HitTargetsTabForm::HitTargetsTabForm(QSqlDatabase db, QWidget *parent) :
    db(db),
    QWidget(parent),
    ui(new Ui::HitTargets)
{
    ui->setupUi(this);
    ui->coordinateLE->setInputMask("99°99'99.99\''A 999°99'99.99\''A 9999.9;_");    
    mapProc = new QProcess(this);
    connect(&NetworkModule::Instance(),SIGNAL(receiveMetricsNetwork(QByteArray&)),this,SLOT(receiveMetricsNetwork(QByteArray&)));
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

/*!
\brief Слот обработки нажатия на кнопку Съем координат

Открывает карту и посылает запрос на переход в режим съема координат,
учитывая выбор геометрии цели (ломаная, прямоугольник или круг)
*/
void HitTargetsTabForm::slotPickCoordinates() {
    if (mapProc->state() != QProcess::Running ) {
        mapProc->setWorkingDirectory(mapPath + "/BIN");
        mapProc->start(mapProc->workingDirectory() + QString("/Karta.exe"));
    }
/*
    QString title1 = "КАРТА-2017 - [Окно Карты" + mapPath + "/maps/100000.rag]";
    LPCWSTR title = (const wchar_t*) title1.utf16();
    HWND hwnd = FindWindow(0,title);
    SetForegroundWindow(hwnd);
*/
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
    ui->detectionTimeDTE->setDateTime(QDateTime::currentDateTime());

    ui->dataSourceBatteryCB->setEnabled(true);
    ui->dataSourceWeaponryCB->setEnabled(true);
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

    ui->dataSourceBatteryCB->setEnabled(false);
    ui->dataSourceWeaponryCB->setEnabled(false);
    ui->targetNumberLE->setEnabled(false);
    ui->targetNameCB->setEnabled(false);

    QSqlQuery query = QSqlQuery(db);
    QString selectQuery = "SELECT importance, target_time, method_location, t.termname "
                          "FROM targets.targets_param tp "
                          "JOIN targets.obj_targets ot ON tp.id_target = ot.id_target "
                          "JOIN reference_data.terms t ON tp.manner_tid = t.termhierarchy "
                          "WHERE ot.target_number = ? "
                          "     AND ot.target_name = ? "
                          "     AND ot.combat_hierarchy = ? "
                          "     AND ot.date_delete IS null";
    query.prepare(selectQuery);
    query.addBindValue(ui->targetNumberLE->text());
    query.addBindValue(ui->targetNameCB->currentData());
    query.addBindValue(ui->dataSourceWeaponryCB->currentData());

    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    query.next();
    ui->importanceLE->setText(query.value(0).toString());
    ui->detectionTimeDTE->setDateTime(query.value(1).toDateTime());
    ui->coverDegreeCB->setCurrentText(query.value(3).toString());

    switch (query.value(2).toInt()) {
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
                              "FROM targets.targets_param tp "
                              "JOIN targets.obj_targets ot ON tp.id_target = ot.id_target "
                              "WHERE ot.target_number = ? "
                              "     AND ot.target_name = ? "
                              "     AND ot.combat_hierarchy = ? "
                              "     AND ot.date_delete IS null";
        query.prepare(squareQuery);
        query.addBindValue(ui->targetNumberLE->text());
        query.addBindValue(ui->targetNameCB->currentData());
        query.addBindValue(ui->dataSourceWeaponryCB->currentData());

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
                             "FROM targets.targets_param tp "
                             "JOIN targets.obj_targets ot ON tp.id_target = ot.id_target "
                             "WHERE ot.target_number = ? "
                             "     AND ot.target_name = ? "
                             "     AND ot.combat_hierarchy = ? "
                             "     AND ot.date_delete IS null";
       query.prepare(roundQuery);
       query.addBindValue(ui->targetNumberLE->text());
       query.addBindValue(ui->targetNameCB->currentData());
       query.addBindValue(ui->dataSourceWeaponryCB->currentData());

        if (!query.exec()) {
            qDebug() << query.lastError();
        }
        query.next();

        ui->coordinateLE->setText(query.value(0).toString());
        ui->radiusLE->setText(QString::number(query.value(1).toInt()));
        break;
    }
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
            !ui->coordinateLE->text().contains(QRegExp("\\d+"))) {
        QMessageBox::warning(this, "Ошибка", "Не заполнено поле с координатами!");
        return false;
    }

    QSqlQuery query = QSqlQuery(db);

    if (ui->dataSourceBatteryCB->isEnabled()) { //i.e. add tab
        db.transaction();

        QString insertObjTargets = "INSERT INTO targets.obj_targets (target_number, target_name, "
                                   "    combat_hierarchy, date_add, id_manager) "
                                   "VALUES (?, ?, ?, now(), ?)";
        query.prepare(insertObjTargets);
        query.addBindValue(ui->targetNumberLE->text());
        query.addBindValue(ui->targetNameCB->currentData());
        query.addBindValue(ui->dataSourceWeaponryCB->currentData());
        query.addBindValue(1);
        query.exec();

        QString selectJustAddedId = "SELECT MAX(id_target) "
                                    "FROM targets.obj_targets ";
        query.prepare(selectJustAddedId);
        query.exec();
        query.next();
        int targetID = query.value(0).toInt();

        QString insertTargetsParam;

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
            if (ui->extraCoordinatesLayout->count() >= 2) {
                //!Запрос на вставку в БД поражаемой цели, представленной полигоном
                insertTargetsParam = insertTargetsParamPatt.arg(getMakePolygonString());
            }
            else {
                //!Запрос на вставку в БД поражаемой цели, представленной линией
                insertTargetsParam = insertTargetsParamPatt.arg(getMakeLineString());
            }

            query.prepare(insertTargetsParam);
            query.addBindValue(targetID);
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(0);
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(1);
            query.exec();
        }
        else if (ui->squareRB->isChecked()) {
            if (ui->frontLE->text().isEmpty() || ui->depthLE->text().isEmpty() || ui->deviationLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            //!Запрос на вставку в БД поражаемой цели, представленной прямоугольником
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
            query.addBindValue(ui->deviationLE->text());
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(1);
            query.exec();
        }
        else if (ui->roundRB->isChecked()) {
            if (ui->radiusLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Не заполнено поле с радиусом!");
                return false;
            }
            //!Запрос на вставку в БД поражаемой цели, представленной кругом
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
            query.exec();
        }
        if (!db.commit()) {
            qDebug() << query.lastError();
            QMessageBox::critical(this, "Ошибка", "Добавить данные не удалось!");
        }
        else {
            QMessageBox::information(this, "Уведомление", "Добавление прошло успешно!");
            return true;
        }
    }
    else if (!ui->dataSourceBatteryCB->isEnabled()) { //i.e. edit tab
        db.transaction();

        QString updateTargetsParam;

        if (ui->randomRB->isChecked()) {
            for (int i = 0; i < ui->extraCoordinatesLayout->count(); i++) {
                QLineEdit* coord = (QLineEdit*)ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(1)->widget();
                if (!coord->text().contains(QRegExp("\\d+"))) {
                    QMessageBox::warning(this, "Ошибка", "Не заполнены все поля с координатами!");
                    return false;
                }
            }
            QString updateTargetsParamPatt = "UPDATE targets.targets_param tp "
                                             "SET target_time = ?, importance = ?, method_location = ?, "
                                             "       obj_location = %1, front = null, depth = null, "
                                             "       deviation = null, radius = null, manner_tid = ?, "
                                             "       tid = ?, date_edit = now() "
                                             "FROM targets.obj_targets ot "
                                             "WHERE tp.id_target = ot.id_target "
                                             "       AND ot.target_number = ? "
                                             "       AND ot.target_name = ? "
                                             "       AND ot.combat_hierarchy = ? "
                                             "       AND ot.date_delete IS null";
            qDebug() << "extraCoordinatesLayout: " << ui->extraCoordinatesLayout->count();
            if (ui->extraCoordinatesLayout->count() >= 2) {
                //!Запрос на обновление в БД поражаемой цели, представленной полигоном
                updateTargetsParam = updateTargetsParamPatt.arg(getMakePolygonString());
            }
            else {
                //!Запрос на обновление в БД поражаемой цели, представленной линией
                updateTargetsParam = updateTargetsParamPatt.arg(getMakeLineString());
            }

            query.prepare(updateTargetsParam);
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(0);
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentData());
            query.addBindValue(ui->dataSourceWeaponryCB->currentData());
            query.exec();
        }
        else if (ui->squareRB->isChecked()) {
            if (ui->frontLE->text().isEmpty() || ui->depthLE->text().isEmpty() || ui->deviationLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            //!Запрос на обновление в БД поражаемой цели, представленной прямоугольником
            updateTargetsParam = "UPDATE targets.targets_param tp "
                                 "SET target_time = ?, importance = ?, method_location = ?, "
                                 "       obj_location = own_forces.coordinates_input(?), front = ?, "
                                 "       depth = ?, deviation = ?, radius = null, manner_tid = ?, "
                                 "       tid = ?, date_edit = now() "
                                 "FROM targets.obj_targets ot "
                                 "WHERE tp.id_target = ot.id_target "
                                 "       AND ot.target_number = ? "
                                 "       AND ot.target_name = ? "
                                 "       AND ot.combat_hierarchy = ? "
                                 "       AND ot.date_delete IS null";

            query.prepare(updateTargetsParam);
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(1);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->frontLE->text());
            query.addBindValue(ui->depthLE->text());
            query.addBindValue(ui->deviationLE->text());
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentData());
            query.addBindValue(ui->dataSourceWeaponryCB->currentData());
            query.exec();
        }
        else if (ui->roundRB->isChecked()) {
            if (ui->radiusLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Не заполнено поле с радиусом!");
                return false;
            }
            //!Запрос на обновление в БД поражаемой цели, представленной кругом
            updateTargetsParam = "UPDATE targets.targets_param tp "
                                 "SET target_time = ?, importance = ?, method_location = ?, "
                                 "       obj_location = own_forces.coordinates_input(?), front = null, "
                                 "       depth = null, deviation = null, radius = ?, manner_tid = ?, "
                                 "       tid = ?, date_edit = now() "
                                 "FROM targets.obj_targets ot "
                                 "WHERE tp.id_target = ot.id_target "
                                 "       AND ot.target_number = ? "
                                 "       AND ot.target_name = ? "
                                 "       AND ot.combat_hierarchy = ? "
                                 "       AND ot.date_delete IS null";

            query.prepare(updateTargetsParam);
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(2);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->radiusLE->text());
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(Utility::getTid(db));
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentData());
            query.addBindValue(ui->dataSourceWeaponryCB->currentData());
            query.exec();
        }
        if (!db.commit()) {
            qDebug() << "Unable to make update operation\n" << query.lastError();
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
    qDebug() << hexCoords;


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

    for (int i = 0; i < query.record().count(); i++) {
        qDebug() << query.value(i).toString();
    }

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
    ui->launchTimeDTE->setDateTime(QDateTime::currentDateTime());
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
                          "                                 AND type_army = ? "
                          "                                 AND date_delete is null) "
                          "ORDER BY object_number";
    query.prepare(selectQuery);
    query.addBindValue("22.10");
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
                          "     AND nlevel(combat_hierarchy) = 2 "
                          "ORDER BY object_number";
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
