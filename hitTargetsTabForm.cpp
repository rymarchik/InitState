#include "hitTargetsTabForm.h"
#include "ui_hittargets.h"
#include "utility.h"

HitTargetsTabForm::HitTargetsTabForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HitTargets)
{
    ui->setupUi(this);
}

QString HitTargetsTabForm::getTargetNumberString() {
    return ui->targetNumberLE->text();
}

QString HitTargetsTabForm::getTargetNameString() {
    return ui->targetNameCB->currentText();
}

void HitTargetsTabForm::onAddSetup() {
    reinitializeFormData();

    ui->dataSourceBatteryCB->addItems(getDataSourceBatteries());
    ui->targetNameCB->addItems(getHitTargets());
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
    slotAddPoint();
}

void HitTargetsTabForm::onEditSetup(QTableWidget* table) {
    reinitializeFormData();
//    this->addTab(contentWidget, "Цель № " + navigatorUpperTable->
//                 item(navigatorUpperTable->currentRow(), 0)->text());
    ui->dataSourceBatteryCB->addItem(table->item(table->currentRow(), 2)->text().split('/').last());
    ui->dataSourceWeaponryCB->addItem(table->item(table->currentRow(), 2)->text().split('/').first());
    ui->targetNumberLE->setText(table->item(table->currentRow(), 0)->text());
    ui->targetNameCB->addItem(table->item(table->currentRow(), 1)->text());

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
                            "       AND t1.termname = '%2' ";
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
        slotAddPoint();
        if (ui->squareRB->isChecked() == true)
            slotToggleSquareRB();
        else
            ui->squareRB->setChecked(true);

        QSqlQuery squareQuery;
        QString squarePatternQuery = "SELECT ST_X(target_location), ST_Y(target_location), "
                                     "      ST_Z(target_location), front, depth, deviation "
                                     "FROM obj_targets.target_params "
                                     "JOIN reference_data.terms ON termhierarchy = target_name "
                                     "WHERE target_number = %1 "
                                     "      AND termname = '%2'";
        QString squareFullQuery = squarePatternQuery.arg(ui->targetNumberLE->text()).arg(ui->targetNameCB->currentText());

        if (!squareQuery.exec(squareFullQuery)) {
            qDebug() << "Unable to make select operation!" << squareQuery.lastError();
        }
        squareQuery.next();

        ui->coordinateLE->setText(Utility::convertCoordToDegreeFormat(squareQuery.value(0).toDouble()) +
                              Utility::convertCoordToDegreeFormat(squareQuery.value(1).toDouble()));
        ui->heightLE->setText(QString::number(squareQuery.value(2).toDouble()));
        ui->frontLE->setText(QString::number(squareQuery.value(3).toInt()));
        ui->depthLE->setText(QString::number(squareQuery.value(4).toInt()));
        ui->deviationLE->setText(QString::number(squareQuery.value(5).toInt()));
        break;
    }
    case 2: {
        slotAddPoint();
        slotAddPoint();
        if (ui->roundRB->isChecked() == true)
            slotToggleRoundRB();
        else
            ui->roundRB->setChecked(true);

        QSqlQuery roundQuery;
        QString roundPatternQuery = "SELECT ST_X(target_location), ST_Y(target_location), "
                                    "       ST_Z(target_location), radius "
                                    "FROM obj_targets.target_params "
                                    "JOIN reference_data.terms ON termhierarchy = target_name "
                                    "WHERE target_number = %1 "
                                    "       AND termname = '%2'";
        QString roundFullQuery = roundPatternQuery.arg(ui->targetNumberLE->text()).arg(ui->targetNameCB->currentText());

        if (!roundQuery.exec(roundFullQuery)) {
            qDebug() << "Unable to make select operation!" << roundQuery.lastError();
        }
        roundQuery.next();

        ui->coordinateLE->setText(Utility::convertCoordToDegreeFormat(roundQuery.value(0).toDouble()) +
                              Utility::convertCoordToDegreeFormat(roundQuery.value(1).toDouble()));
        ui->heightLE->setText(QString::number(roundQuery.value(2).toDouble()));
        ui->radiusLE->setText(QString::number(roundQuery.value(3).toInt()));
        break;
    }
    }
    ui->coverDegreeCB->setCurrentText(query.value(3).toString());
}

bool HitTargetsTabForm::onSaveSetup() {
    if (ui->targetNumberLE->text().isEmpty() || ui->importanceLE->text().isEmpty() ||
            !ui->coordinateLE->text().contains(QRegExp("\\d+")) || ui->heightLE->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
        return false;
    }

    QSqlQuery query;
    QString makePolygonString = "ST_MakePolygon(ST_MakeLine(ARRAY[";
    QString makePointPattern = "ST_MakePoint(%1, %2, %3)";
    QString makePointString = makePointPattern.arg(Utility::convertCoordToDecimalFormat(ui->coordinateLE->text()).at(0))
            .arg(Utility::convertCoordToDecimalFormat(ui->coordinateLE->text()).at(1)).arg(ui->heightLE->text());
    QString makePoint4Polygon = makePointPattern.append(", ");

    makePolygonString.append(makePointString).append(", ");
    for (int i = 0; i < ui->extraCoordinatesLayout->count(); i++) {
        QLineEdit* coord = (QLineEdit*)ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(1)->widget();
        QLineEdit* height = (QLineEdit*)ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(3)->widget();
        QList<double> coordinates = Utility::convertCoordToDecimalFormat(coord->text());
        makePolygonString.append(makePoint4Polygon.arg(coordinates.at(0))
                                 .arg(coordinates.at(1)).arg(height->text()));
    }
    makePolygonString.append(makePointString).append("]))");

    if (ui->dataSourceBatteryCB->isEnabled()) { //i.e. add tab
        QString insertPattern;
        QString insertQuery;

        QString batteryPattern = "SELECT combat_hierarchy "
                                 "FROM own_forces.combatstructure "
                                 "JOIN reference_data.terms ON termhierarchy = object_name "
                                 "WHERE object_number = %1 "
                                 "      AND termname = '%2' "
                                 "      AND type_army = '22.10' "
                                 "      AND type_mode = 0 "
                                 "      AND date_delete is null";
        QString weaponryPattern = "SELECT combat_hierarchy "
                                  "FROM own_forces.combatstructure "
                                  "JOIN reference_data.terms ON termhierarchy = object_name "
                                  "WHERE object_number = %1 "
                                  "     AND termname = '%2' "
                                  "     AND type_mode = 0 "
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
                QLineEdit* height = (QLineEdit*)ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(3)->widget();
                if (!coord->text().contains(QRegExp("\\d+")) || height->text().isEmpty()) {
                    QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                    return false;
                }
            }
            insertPattern = "INSERT INTO obj_targets.target_params (target_number, target_name, "
                            "       importance, target_time, target_geometry, target_location, "
                            "       cover_degree, platoon, weaponry) "
                            "VALUES (?, (SELECT termhierarchy FROM reference_data.terms "
                            "WHERE termname = ?), ?, ?, 0, %1, (SELECT termhierarchy FROM "
                            "reference_data.terms WHERE termname = ?), (%2), (%3))";
            insertQuery = insertPattern.arg(makePolygonString)
                    .arg(dataSourceBatteryString).arg(dataSourceWeaponryString);

            query.prepare(insertQuery);
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentText());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->coverDegreeCB->currentText());
        }
        else if (ui->squareRB->isChecked()) {
            if (ui->frontLE->text().isEmpty() || ui->depthLE->text().isEmpty() || ui->deviationLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            insertPattern = "INSERT INTO obj_targets.target_params (target_number, target_name, "
                            "       importance, target_time, target_geometry, target_location, "
                            "       front, depth, deviation, cover_degree, platoon, weaponry) "
                            "VALUES (?, (SELECT termhierarchy FROM reference_data.terms WHERE "
                            "termname = ?), ?, ?, 1, %1, ?, ?, ?, (SELECT termhierarchy FROM "
                            "reference_data.terms WHERE termname = ?), (%2), (%3))";
            insertQuery = insertPattern.arg(makePointString)
                    .arg(dataSourceBatteryString).arg(dataSourceWeaponryString);

            query.prepare(insertQuery);
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentText());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->frontLE->text());
            query.addBindValue(ui->depthLE->text());
            query.addBindValue(ui->deviationLE->text());
            query.addBindValue(ui->coverDegreeCB->currentText());
        }
        else if (ui->roundRB->isChecked()) {
            if (ui->radiusLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            insertPattern = "INSERT INTO obj_targets.target_params (target_number, target_name, "
                            "       importance, target_time, target_geometry, target_location, "
                            "       radius, cover_degree, platoon, weaponry) "
                            "VALUES (?, (SELECT termhierarchy FROM reference_data.terms WHERE "
                            "termname = ?), ?, ?, 2, %1, ?, (SELECT termhierarchy FROM "
                            "reference_data.terms WHERE termname = ?), (%2), (%3))";
            insertQuery = insertPattern.arg(makePointString)
                    .arg(dataSourceBatteryString).arg(dataSourceWeaponryString);

            query.prepare(insertQuery);
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentText());
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->radiusLE->text());
            query.addBindValue(ui->coverDegreeCB->currentText());
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
                QLineEdit* height = (QLineEdit*)ui->extraCoordinatesLayout->itemAt(i)->layout()->itemAt(3)->widget();
                if (!coord->text().contains(QRegExp("\\d+")) || height->text().isEmpty()) {
                    QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                    return false;
                }
            }
            updatePattern = "UPDATE obj_targets.target_params "
                            "SET importance = ?, target_time = ?, target_geometry = 0, "
                            "       target_location = %1, front = null, depth = null, "
                            "       deviation = null, radius = null, cover_degree = "
                            "           (SELECT termhierarchy "
                            "           FROM reference_data.terms "
                            "           WHERE termname = ?), "
                            "       update_time = now() "
                            "WHERE target_number = ? "
                            "       AND target_name = "
                            "           (SELECT termhierarchy "
                            "           FROM reference_data.terms "
                            "           WHERE termname = ?)";
            updateQuery = updatePattern.arg(makePolygonString);

            query.prepare(updateQuery);
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->coverDegreeCB->currentText());
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentText());
        }
        else if (ui->squareRB->isChecked()) {
            if (ui->frontLE->text().isEmpty() || ui->depthLE->text().isEmpty() || ui->deviationLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            updatePattern = "UPDATE obj_targets.target_params "
                            "SET importance = ?, target_time = ?, target_geometry = 1, "
                            "       target_location = %1, front = ?, depth = ?, "
                            "       deviation = ?, radius = null, cover_degree = "
                            "           (SELECT termhierarchy "
                            "           FROM reference_data.terms "
                            "           WHERE termname = ?), "
                            "       update_time = now() "
                            "WHERE target_number = ? "
                            "       AND target_name = "
                            "           (SELECT termhierarchy "
                            "           FROM reference_data.terms "
                            "           WHERE termname = ?)";
            updateQuery = updatePattern.arg(makePointString);

            query.prepare(updateQuery);
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->frontLE->text());
            query.addBindValue(ui->depthLE->text());
            query.addBindValue(ui->deviationLE->text());
            query.addBindValue(ui->coverDegreeCB->currentText());
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentText());
        }
        else if (ui->roundRB->isChecked()) {
            if (ui->radiusLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            updatePattern = "UPDATE obj_targets.target_params "
                            "SET importance = ?, target_time = ?, target_geometry = 2, "
                            "       target_location = %1, front = null, depth = null, "
                            "       deviation = null, radius = ?, cover_degree = "
                            "           (SELECT termhierarchy "
                            "           FROM reference_data.terms "
                            "           WHERE termname = ?), "
                            "       update_time = now() "
                            "WHERE target_number = ? "
                            "       AND target_name = "
                            "           (SELECT termhierarchy "
                            "           FROM reference_data.terms "
                            "           WHERE termname = ?)";
            updateQuery = updatePattern.arg(makePointString);

            query.prepare(updateQuery);
            query.addBindValue(ui->importanceLE->text());
            query.addBindValue(ui->detectionTimeDTE->text());
            query.addBindValue(ui->radiusLE->text());
            query.addBindValue(ui->coverDegreeCB->currentText());
            query.addBindValue(ui->targetNumberLE->text());
            query.addBindValue(ui->targetNameCB->currentText());
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
    QLabel* newHeightLbl = new QLabel("Высота, м");
    QLineEdit* newCoordLE = new QLineEdit;
    QLineEdit* newHeightLE = new QLineEdit;

    newCoordLbl->setFont(font);
    newHeightLbl->setFont(font);
    newCoordLE->setInputMask("B99° 99' 99.99\" \\N     B99° 99' 99.99\" E;_");

    QHBoxLayout* coordStringLayout = new QHBoxLayout;
    coordStringLayout->addWidget(newCoordLbl);
    coordStringLayout->addWidget(newCoordLE);
    coordStringLayout->addWidget(newHeightLbl);
    coordStringLayout->addWidget(newHeightLE);
    ui->extraCoordinatesLayout->addLayout(coordStringLayout);

    if (ui->extraCoordinatesLayout->count() > 2)
        ui->removePointBtn->setEnabled(true);
}

void HitTargetsTabForm::slotRemovePoint() {
    QLayout* layout = ui->extraCoordinatesLayout->itemAt(ui->extraCoordinatesLayout->count() - 1)->layout();
    for (int i = 3; i >= 0; i--) {
        delete layout->takeAt(i)->widget();
        delete layout->takeAt(i);
    }
    ui->extraCoordinatesLayout->removeItem(layout);

    if (ui->extraCoordinatesLayout->count() < 3)
        ui->removePointBtn->setEnabled(false);
}

void HitTargetsTabForm::addFilledPoints() {
    QSqlQuery query;
    QString selectPattern = "SELECT ST_X(geom), ST_Y(geom), ST_Z(geom) "
                            "FROM (SELECT (ST_dumppoints(target_location)).geom "
                            "       FROM obj_targets.target_params "
                            "       JOIN reference_data.terms ON termhierarchy = target_name "
                            "       WHERE target_number = %1 "
                            "           AND termname = '%2') AS foo";
    QString selectQuery = selectPattern.arg(ui->targetNumberLE->text()).arg(ui->targetNameCB->currentText());

    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    query.next();
    ui->coordinateLE->setText(Utility::convertCoordToDegreeFormat(query.value(0).toDouble()) +
                          Utility::convertCoordToDegreeFormat(query.value(1).toDouble()));
    ui->heightLE->setText(QString::number(query.value(2).toDouble()));


    QHBoxLayout* coordStringLayout;
    QLabel* newCoordLbl;
    QLabel* newHeightLbl;
    QLineEdit* newCoordLE;
    QLineEdit* newHeightLE;

    for (int i = 0; i < query.size() - 2; i++) {
        query.next();

        newCoordLbl = new QLabel("Координата");
        newHeightLbl = new QLabel("Высота, м");
        newCoordLbl->setFont(font);
        newHeightLbl->setFont(font);

        newCoordLE = new QLineEdit(Utility::convertCoordToDegreeFormat(query.value(0).toDouble()) +
                                   Utility::convertCoordToDegreeFormat(query.value(1).toDouble()));
        newCoordLE->setInputMask("B99° 99' 99.99\" \\N     B99° 99' 99.99\" E;_");
        newHeightLE = new QLineEdit(QString::number(query.value(2).toDouble()));

        coordStringLayout = new QHBoxLayout;
        coordStringLayout->addWidget(newCoordLbl);
        coordStringLayout->addWidget(newCoordLE);
        coordStringLayout->addWidget(newHeightLbl);
        coordStringLayout->addWidget(newHeightLE);
        ui->extraCoordinatesLayout->addLayout(coordStringLayout);
    }
    if (ui->extraCoordinatesLayout->count() > 2)
        ui->removePointBtn->setEnabled(true);
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
        for (int j = 0; j < 4; j++) {
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
        for (int j = 0; j < 4; j++) {
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
        for (int j = 0; j < 4; j++) {
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

void HitTargetsTabForm::reinitializeFormData() {
//    Utility::closeNewEditTab(this);

//    ui->dataSourceBatteryCB->clear();
//    ui->dataSourceWeaponryCB->clear();
//    ui->targetNumberLE->clear();
//    ui->targetNameCB->clear();
//    ui->importanceLE->clear();

//    Utility::clearLayout(ui->extraCoordinatesLayout);
//    ui->coordinateLE->clear();
//    ui->heightLE->clear();
//    ui->removePointBtn->setEnabled(false);
//    ui->frontLE->clear();
//    ui->depthLE->clear();
//    ui->deviationLE->clear();
//    ui->radiusLE->clear();
//    ui->coverDegreeCB->clear();
    ui->coverDegreeCB->addItems(getCoverDegrees());

//    ui->launchChB->setChecked(false);
//    ui->explosionChB->setChecked(false);
//    ui->launchTimeDTE->setEnabled(false);
//    ui->damageDegreeCB->clear();
//    ui->rocketTypeCB->clear();
//    ui->quantityLE->clear();
    ui->launchTimeDTE->setDateTime(QDateTime::currentDateTime());
    ui->damageDegreeCB->addItems(getDamageDegrees());
    ui->rocketTypeCB->addItems(getRocketTypes());
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
                          "                                 AND type_mode = 0 "
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
                            "                                               AND type_mode = 0 "
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

QStringList HitTargetsTabForm::getHitTargets() {
    QSqlQuery query;
    QString selectQuery = "SELECT termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '90.*' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    return list;
}

QStringList HitTargetsTabForm::getCoverDegrees() {
    QSqlQuery query;
    QString selectQuery = "SELECT termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '95.20.*' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    return list;
}

QStringList HitTargetsTabForm::getDamageDegrees() {
    QSqlQuery query;
    QString selectQuery = "SELECT termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '95.10.*' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    return list;
}

QStringList HitTargetsTabForm::getRocketTypes() {
    QSqlQuery query;
    QString selectQuery = "SELECT termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy ~ '51.50.*' "
                          "     AND termhierarchy != '51.50.30' "
                          "     AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    return list;
}


HitTargetsTabForm::~HitTargetsTabForm()
{
    delete ui;
}
