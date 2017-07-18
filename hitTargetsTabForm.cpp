#include "hitTargetsTabForm.h"
#include "ui_hittargets.h"
#include "utility.h"

#include "mapsrc/networkmodule.h"
#include "mapsrc/NetworkObject.h"

HitTargetsTabForm::HitTargetsTabForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HitTargets)
{
    ui->setupUi(this);
    ui->coordinateLE->setInputMask("99°99'99.99\''A 999°99'99.99\''A 9999.9;_");    
    mapProc = new QProcess(this);
    connect(&NetworkModule::Instance(),SIGNAL(receiveMetricsNetwork(QByteArray&)),this,SLOT(receiveMetricsNetwork(QByteArray&)));
}

QString HitTargetsTabForm::getTargetNumberString() {
    return ui->targetNumberLE->text();
}

QString HitTargetsTabForm::getTargetNameString() {
    return ui->targetNameCB->currentText();
}

void HitTargetsTabForm::slotPickCoordinates() {
    QString mapPath = "D:/Volat/Qt/KARTA/sample/BIN";
    if (mapProc->state() != QProcess::Running ) {
        mapProc->setWorkingDirectory(mapPath);
        mapProc->start(mapPath + QString("/Karta.exe"));
        mapProc->waitForStarted();
    }

    QString title1 = "КАРТА-2017 - [Окно КартыD:/Volat/Qt/KARTA/sample/maps/100000.rag]";
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

QString HitTargetsTabForm::getParsedCoordinates(double lat, double lon, double alt) {
    QSqlQuery query;
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

void HitTargetsTabForm::onAddSetup() {
    addCommonFormData();
    ui->dataSourceBatteryCB->addItems(getDataSourceBatteries());
    getHitTargets();
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

void HitTargetsTabForm::onEditSetup(QTableWidget* table) {
    addCommonFormData();
    ui->dataSourceBatteryCB->addItem(table->item(table->currentRow(), 2)->text().split('/').last());
    ui->dataSourceWeaponryCB->addItem(table->item(table->currentRow(), 2)->text().split('/').first());
    ui->targetNumberLE->setText(table->item(table->currentRow(), 0)->text());
    getHitTargets();
    ui->targetNameCB->setCurrentText(table->item(table->currentRow(), 1)->text());
//    ui->targetNameCB->addItem(table->item(table->currentRow(), 1)->text());

    ui->dataSourceBatteryCB->setEnabled(false);
    ui->dataSourceWeaponryCB->setEnabled(false);
    ui->targetNumberLE->setEnabled(false);
    ui->targetNameCB->setEnabled(false);

    QSqlQuery query;
    QString selectPattern = "SELECT importance, target_time, target_geometry, t2.termname "
                            "FROM obj_targets.target_params "
                            "JOIN reference_data.terms t1 ON t1.termhierarchy = target_name "
                            "JOIN reference_data.terms t2 ON t2.termhierarchy = cover_degree "
                            "WHERE target_number = %1 "
                            "       AND t1.termname = '%2' "
                            "       AND delete_time is null";
    QString selectQuery = selectPattern.arg(ui->targetNumberLE->text()).arg(ui->targetNameCB->currentText());

    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    query.next();
    ui->importanceLE->setText(query.value(0).toString());
    ui->detectionTimeDTE->setDateTime(query.value(1).toDateTime());

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

        QSqlQuery squareQuery;
        QString squarePatternQuery = "SELECT own_forces.coordinates_output(target_location), "
                                     "      front, depth, deviation "
                                     "FROM obj_targets.target_params "
                                     "JOIN reference_data.terms ON termhierarchy = target_name "
                                     "WHERE target_number = %1 "
                                     "      AND termname = '%2'"
                                     "      AND delete_time is null";
        QString squareFullQuery = squarePatternQuery.arg(ui->targetNumberLE->text()).arg(ui->targetNameCB->currentText());

        if (!squareQuery.exec(squareFullQuery)) {
            qDebug() << "Unable to make select operation!" << squareQuery.lastError();
        }
        squareQuery.next();

        ui->coordinateLE->setText(squareQuery.value(0).toString());
        ui->frontLE->setText(QString::number(squareQuery.value(1).toInt()));
        ui->depthLE->setText(QString::number(squareQuery.value(2).toInt()));
        ui->deviationLE->setText(QString::number(squareQuery.value(3).toInt()));
        break;
    }
    case 2: {
        slotAddPoint();
        if (ui->roundRB->isChecked() == true)
            slotToggleRoundRB();
        else
            ui->roundRB->setChecked(true);

        QSqlQuery roundQuery;
        QString roundPatternQuery = "SELECT own_forces.coordinates_output(target_location), radius "
                                    "FROM obj_targets.target_params "
                                    "JOIN reference_data.terms ON termhierarchy = target_name "
                                    "WHERE target_number = %1 "
                                    "       AND termname = '%2' "
                                    "       AND delete_time is null";
        QString roundFullQuery = roundPatternQuery.arg(ui->targetNumberLE->text()).arg(ui->targetNameCB->currentText());

        if (!roundQuery.exec(roundFullQuery)) {
            qDebug() << "Unable to make select operation!" << roundQuery.lastError();
        }
        roundQuery.next();

        ui->coordinateLE->setText(roundQuery.value(0).toString());
        ui->radiusLE->setText(QString::number(roundQuery.value(1).toInt()));
        break;
    }
    }
    ui->coverDegreeCB->setCurrentText(query.value(3).toString());
}

bool HitTargetsTabForm::onSaveSetup() {
    if (ui->targetNumberLE->text().isEmpty() || ui->importanceLE->text().isEmpty() ||
            !ui->coordinateLE->text().contains(QRegExp("\\d+"))) {
        QMessageBox::warning(this, "Ошибка", "Не заполнено поле с координатами!");
        return false;
    }

    QSqlQuery query;

    if (ui->dataSourceBatteryCB->isEnabled()) { //i.e. add tab
        QString insertPattern;
        QString insertQuery;

        QString batteryPattern = "SELECT combat_hierarchy "
                                 "FROM own_forces.combatstructure "
                                 "JOIN reference_data.terms ON termhierarchy = object_name "
                                 "WHERE object_number = %1 "
                                 "      AND termname = '%2' "
                                 "      AND type_army = '22.10' "
                                 "      AND date_delete is null";
        QString weaponryPattern = "SELECT combat_hierarchy "
                                  "FROM own_forces.combatstructure "
                                  "JOIN reference_data.terms ON termhierarchy = object_name "
                                  "WHERE object_number = %1 "
                                  "     AND termname = '%2' "
                                  "     AND date_delete is null "
                                  "     AND subltree(combat_hierarchy, 0, 1) = (%3)";
        QString batteryTargetNumber = ui->dataSourceBatteryCB->currentText().split(' ').first();
        QString batteryTargetName = ui->dataSourceBatteryCB->currentText().remove(0, batteryTargetNumber.size() + 1);
        QString weaponryTargetNumber = ui->dataSourceWeaponryCB->currentText().split(' ').first();
        QString weaponryTargetName = ui->dataSourceWeaponryCB->currentText().remove(0, weaponryTargetNumber.size() + 1);

        QString dataSourceBatteryString = batteryPattern.arg(batteryTargetNumber).arg(batteryTargetName);
        QString dataSourceWeaponryString = weaponryPattern.arg(weaponryTargetNumber)
                .arg(weaponryTargetName).arg(dataSourceBatteryString);

        if (ui->randomRB->isChecked()) {
            for (int i = 0; i < ui->extraCoordinatesLayout->count(); i++) {
                QLineEdit* coord = (QLineEdit*)ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(1)->widget();
                if (!coord->text().contains(QRegExp("\\d+"))) {
                    QMessageBox::warning(this, "Ошибка", "Не заполнены все поля с координатами!");
                    return false;
                }
            }
            insertPattern = "INSERT INTO obj_targets.target_params (target_number, target_name, "
                            "       importance, target_time, target_geometry, target_location, "
                            "       cover_degree, platoon, weaponry) "
                            "VALUES (?, ?, ?, ?, ?, %1, ?, (%2), (%3))";
            qDebug() << "extraCoordinatesLayout: " << ui->extraCoordinatesLayout->count();
            if (ui->extraCoordinatesLayout->count() >= 2) {
                insertQuery = insertPattern.arg(getMakePolygonString())
                        .arg(dataSourceBatteryString).arg(dataSourceWeaponryString);
            }
            else {
                insertQuery = insertPattern.arg(getMakeLineString())
                        .arg(dataSourceBatteryString).arg(dataSourceWeaponryString);
            }

            query.prepare(insertQuery);
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentData());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(0);
            query.addBindValue(ui->coverDegreeCB->currentData());
        }
        else if (ui->squareRB->isChecked()) {
            if (ui->frontLE->text().isEmpty() || ui->depthLE->text().isEmpty() || ui->deviationLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            insertPattern = "INSERT INTO obj_targets.target_params (target_number, target_name, "
                            "       importance, target_time, target_geometry, target_location, "
                            "       front, depth, deviation, cover_degree, platoon, weaponry) "
                            "VALUES (?, ?, ?, ?, ?, own_forces.coordinates_input(?), ?, ?, ?, ?, (%1), (%2)) ";
            insertQuery = insertPattern.arg(dataSourceBatteryString).arg(dataSourceWeaponryString);

            query.prepare(insertQuery);
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentData());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(1);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->frontLE->text());
            query.addBindValue(ui->depthLE->text());
            query.addBindValue(ui->deviationLE->text());
//
//            query.addBindValue(ui->coverDegreeCB->currentText());

//            qDebug() << Utility::getCoords(ui->coordinateLE->text());
           query.addBindValue(ui->coverDegreeCB->currentData());
        }
        else if (ui->roundRB->isChecked()) {
            if (ui->radiusLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Не заполнено поле с радиусом!");
                return false;
            }
            insertPattern = "INSERT INTO obj_targets.target_params (target_number, target_name, "
                            "       importance, target_time, target_geometry, target_location, "
                            "       radius, cover_degree, platoon, weaponry) "
                            "VALUES (?, ?, ?, ?, ?, own_forces.coordinates_input(?), ?, ?, (%1), (%2))";
            insertQuery = insertPattern.arg(dataSourceBatteryString).arg(dataSourceWeaponryString);

            query.prepare(insertQuery);
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentData());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(2);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->radiusLE->text());
            query.addBindValue(ui->coverDegreeCB->currentData());
        }
        if (!query.exec()) {
            qDebug() << "Unable to make insert operation\n" << query.lastError();
            QMessageBox::critical(this, "Ошибка", "Добавить данные не удалось!");
        }
        else {
            QMessageBox::information(this, "Уведомление", "Добавление прошло успешно!");
            return true;
        }
    }
    else if (!ui->dataSourceBatteryCB->isEnabled()) { //i.e. edit tab
        QString updatePattern;
        QString updateQuery;
        if (ui->randomRB->isChecked()) {
            for (int i = 0; i < ui->extraCoordinatesLayout->count(); i++) {
                QLineEdit* coord = (QLineEdit*)ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(1)->widget();
                if (!coord->text().contains(QRegExp("\\d+"))) {
                    QMessageBox::warning(this, "Ошибка", "Не заполнены все поля с координатами!");
                    return false;
                }
            }
            updatePattern = "UPDATE obj_targets.target_params "
                            "SET importance = ?, target_time = ?, target_geometry = ?, "
                            "       target_location = %1, front = null, depth = null, "
                            "       deviation = null, radius = null, cover_degree = ?, "
                            "       update_time = now() "
                            "WHERE target_number = ? "
                            "       AND target_name = ?";
            qDebug() << "extraCoordinatesLayout: " << ui->extraCoordinatesLayout->count();
            if (ui->extraCoordinatesLayout->count() >= 2) {
                updateQuery = updatePattern.arg(getMakePolygonString());
            }
            else {
                updateQuery = updatePattern.arg(getMakeLineString());
            }

            query.prepare(updateQuery);
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(0);
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentData());
        }
        else if (ui->squareRB->isChecked()) {
            if (ui->frontLE->text().isEmpty() || ui->depthLE->text().isEmpty() || ui->deviationLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            updateQuery = "UPDATE obj_targets.target_params "
                          "SET importance = ?, target_time = ?, target_geometry = ?, "
                          "     target_location = own_forces.coordinates_input(?), "
                          "     front = ?, depth = ?, deviation = ?, radius = null, "
                          "     cover_degree = ?, update_time = now() "
                          "WHERE target_number = ? "
                          "     AND target_name = ?";
            query.prepare(updateQuery);
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(1);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->frontLE->text());
            query.addBindValue(ui->depthLE->text());
            query.addBindValue(ui->deviationLE->text());
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentData());
        }
        else if (ui->roundRB->isChecked()) {
            if (ui->radiusLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Не заполнено поле с радиусом!");
                return false;
            }
            updateQuery = "UPDATE obj_targets.target_params "
                          "SET importance = ?, target_time = ?, target_geometry = 2, "
                          "     target_location = own_forces.coordinates_input(?), "
                          "     front = null, depth = null, deviation = null, "
                          "     radius = ?, cover_degree = ?, update_time = now() "
                          "WHERE target_number = ? "
                          "     AND target_name = ?";
            query.prepare(updateQuery);
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(2);
            query.addBindValue(ui->coordinateLE->text());
            query.addBindValue(ui->radiusLE->text());
            query.addBindValue(ui->coverDegreeCB->currentData());
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentData());
        }
        if (!query.exec()) {
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

void HitTargetsTabForm::addFilledPoints() {
    QSqlQuery query;

    QString selectHexCoordsPattern = "SELECT (ST_dumppoints(target_location)).geom "
                                     "FROM obj_targets.target_params "
                                     "JOIN reference_data.terms ON termhierarchy = target_name "
                                     "WHERE target_number = %1 "
                                     "      AND termname = '%2'"
                                     "      AND delete_time is null";
    QString selectHexCoordsQuery = selectHexCoordsPattern.arg(ui->targetNumberLE->text())
            .arg(ui->targetNameCB->currentText());

    if (!query.exec(selectHexCoordsQuery)) {
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

void HitTargetsTabForm::slotChangeDataSourceBattery(int n) {
    if (n != -1) {
        ui->dataSourceWeaponryCB->clear();
        ui->dataSourceWeaponryCB->addItems(getDataSourceWeaponry());
    }
}

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

void HitTargetsTabForm::addCommonFormData() {
    getCoverDegrees();
    ui->launchTimeDTE->setDateTime(QDateTime::currentDateTime());
    getDamageDegrees();
    getRocketTypes();
}

QString HitTargetsTabForm::getMakeLineString() {
    QString selectHexCoordsQuery = "SELECT own_forces.coordinates_input(?), own_forces.coordinates_input(?)";

    QSqlQuery query;
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

QString HitTargetsTabForm::getMakePolygonString() {
    QString selectHexCoordsQuery = "SELECT ";
    QString funcString = "own_forces.coordinates_input(?)";
    for (int i = 0; i < ui->extraCoordinatesLayout->count() + 1; i++) {
        selectHexCoordsQuery.append(funcString).append(", ");
    }
    selectHexCoordsQuery.append(funcString);
//    qDebug() << "selectHexCoordsQuery: " << selectHexCoordsQuery << endl;

    QSqlQuery query;
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

QStringList HitTargetsTabForm::getDataSourceBatteries() {
    QSqlQuery query;
    QString selectQuery = "SELECT object_number, termname "
                          "FROM own_forces.combatstructure "
                          "JOIN reference_data.terms ON termhierarchy = object_name "
                          "WHERE combat_hierarchy IN (SELECT combat_hierarchy "
                          "                             FROM own_forces.combatstructure "
                          "                             WHERE nlevel(combat_hierarchy) = 1 "
                          "                                 AND type_army = '22.10' "
                          "                                 AND date_delete is null) "
                          "ORDER BY object_number";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(tr("%1").arg(query.value(0).toInt()) + " " + query.value(1).toString());
    }
    return list;
}

QStringList HitTargetsTabForm::getDataSourceWeaponry() {
    QString targetNumber = ui->dataSourceBatteryCB->currentText().split(' ').first();
    QString targetName = ui->dataSourceBatteryCB->currentText().remove(0, targetNumber.size() + 1);

    QSqlQuery query;
    QString selectPattern = "SELECT object_number, termname "
                            "FROM own_forces.combatstructure "
                            "JOIN reference_data.terms ON termhierarchy = object_name "
                            "WHERE subltree(combat_hierarchy, 0, 1) = (SELECT combat_hierarchy "
                            "                                           FROM own_forces.combatstructure "
                            "                                           JOIN reference_data.terms "
                            "                                           ON termhierarchy = object_name "
                            "                                           WHERE termname = '%2' "
                            "                                               AND object_number = %1 "
                            "                                               AND type_army = '22.10' "
                            "                                               AND date_delete is null) "
                            "   AND nlevel(combat_hierarchy) = 2 "
                            "ORDER BY object_number";
    QString selectQuery = selectPattern.arg(targetNumber).arg(targetName);
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(tr("%1").arg(query.value(0).toInt()) + " " + query.value(1).toString());
    }
    return list;
}

void HitTargetsTabForm::getHitTargets() {
    QSqlQuery query;
    QString selectQuery = "SELECT termhierarchy, termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '90.*' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    while (query.next()) {
        ui->targetNameCB->addItem(query.value(1).toString(), query.value(0).toString());
    }
}

void HitTargetsTabForm::getCoverDegrees() {
    QSqlQuery query;
    QString selectQuery = "SELECT termhierarchy, termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '95.20.*' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    while (query.next()) {
        ui->coverDegreeCB->addItem(query.value(1).toString(), query.value(0).toString());
    }
}

void HitTargetsTabForm::getDamageDegrees() {
    QSqlQuery query;
    QString selectQuery = "SELECT termhierarchy, termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '95.10.*' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    while (query.next()) {
        ui->damageDegreeCB->addItem(query.value(1).toString(), query.value(0).toString());
    }
}

void HitTargetsTabForm::getRocketTypes() {
    QSqlQuery query;
    QString selectQuery = "SELECT termhierarchy, termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '51.50.*' "
                          "     AND termhierarchy != '51.50.30' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    while (query.next()) {
        ui->rocketTypeCB->addItem(query.value(1).toString(), query.value(0).toString());
    }
}


HitTargetsTabForm::~HitTargetsTabForm()
{
    delete ui;
}
