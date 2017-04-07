#include "hitTargets.h"
//#include "ui_hittargets.h"
#include "utility.h"

HitTargets::HitTargets(QSqlDatabase DB, QWidget* parent) : db(DB), QToolBox(parent) //BaseTabWidget(parent)
{
/*
    contentWidget = loadUiFile();

    dataSourceBatteryCB = findChild<QComboBox*>("dataSourceBatteryCB");
    dataSourceWeaponryCB = findChild<QComboBox*>("dataSourceWeaponryCB");
    targetNumberLE = findChild<QLineEdit*>("targetNumberLE");
    targetNameCB = findChild<QComboBox*>("targetNameCB");
    importanceLE = findChild<QLineEdit*>("importanceLE");
    detectionTimeDTE = findChild<QDateTimeEdit*>("detectionTimeDTE");

    randomRB = findChild<QRadioButton*>("randomRB");
    squareRB = findChild<QRadioButton*>("squareRB");
    roundRB = findChild<QRadioButton*>("roundRB");
    coordinateLE = findChild<QLineEdit*>("coordinateLE");
    heightLE = findChild<QLineEdit*>("heightLE");
    extraCoordinatesLayout = findChild<QVBoxLayout*>("extraCoordinatesLayout");
    addPointBtn = findChild<QPushButton*>("addPointBtn");
    removePointBtn = findChild<QPushButton*>("removePointBtn");
    frontLbl = findChild<QLabel*>("frontLbl");
    depthLbl = findChild<QLabel*>("depthLbl");
    deviationLbl = findChild<QLabel*>("deviationLbl");
    radiusLbl = findChild<QLabel*>("radiusLbl");
    frontLE = findChild<QLineEdit*>("frontLE");
    depthLE = findChild<QLineEdit*>("depthLE");
    deviationLE = findChild<QLineEdit*>("deviationLE");
    radiusLE = findChild<QLineEdit*>("radiusLE");
    coverDegreeCB = findChild<QComboBox*>("coverDegreeCB");

    launchChB = findChild<QCheckBox*>("launchChB");
    explosionChB = findChild<QCheckBox*>("explosionChB");
    launchTimeDTE = findChild<QDateTimeEdit*>("launchTimeDTE");
    damageDegreeCB = findChild<QComboBox*>("damageDegreeCB");
    rocketTypeCB = findChild<QComboBox*>("rocketTypeCB");
    quantityLE = findChild<QLineEdit*>("quantityLE");

    connect(dataSourceBatteryCB, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeDataSourceBattery(int)));

    connect(randomRB, SIGNAL(toggled(bool)), this, SLOT(slotToggleRandomRB()));
    connect(squareRB, SIGNAL(toggled(bool)), this, SLOT(slotToggleSquareRB()));
    connect(roundRB, SIGNAL(toggled(bool)), this, SLOT(slotToggleRoundRB()));
    connect(addPointBtn, SIGNAL(clicked()), this, SLOT(slotAddPoint()));
    connect(removePointBtn, SIGNAL(clicked()), this, SLOT(slotRemovePoint()));

    connect(launchChB, SIGNAL(clicked()), this, SLOT(slotToggleLaunchCB()));
    connect(explosionChB, SIGNAL(clicked()), this, SLOT(slotToggleExplosionCB()));
*/
}
/*
QWidget* HitTargets::loadUiFile() {
    QUiLoader loader;

    QFile file(":/hittargets.ui");
    file.open(QFile::ReadOnly);

    QWidget *formWidget = loader.load(&file, this);
    file.close();

    return formWidget;
}
*/
void HitTargets::fillNavigator(QTableWidget *navigatorTableWidget)
{
//    Utility::closeNewEditTab(this);
//    navigatorUpperTable->clear();
    navigatorTableWidget->clear();

    QSqlQuery query = QSqlQuery(db);
    QString selectPattern = "SELECT tp.target_number, t.termname, cs1.object_number, t1.termname, "
                            "cs2.object_number, t2.termname FROM obj_targets.target_params tp JOIN "
                            "reference_data.terms t ON tp.target_name = t.termhierarchy JOIN "
                            "own_forces.combatstructure cs1 ON tp.weaponry = cs1.combat_hierarchy JOIN "
                            "own_forces.combatstructure cs2 ON tp.platoon = cs2.combat_hierarchy JOIN "
                            "reference_data.terms t1 ON cs1.object_name = t1.termhierarchy JOIN "
                            "reference_data.terms t2 ON cs2.object_name = t2.termhierarchy WHERE "
                            "delete_time is null";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    navigatorTableWidget->setColumnCount(3);
    navigatorTableWidget->setRowCount(query.size());

    int i = 0;
    while (query.next()) {
        navigatorTableWidget->setItem(i, 0, new QTableWidgetItem(tr("%1").arg(query.value(0).toInt())));
        navigatorTableWidget->setItem(i, 1, new QTableWidgetItem(query.value(1).toString()));
        navigatorTableWidget->setItem(i, 2, new QTableWidgetItem(tr("%1").arg(query.value(2).toInt()) + " " +
                                                                query.value(3).toString() + "/" +
                                                                tr("%1").arg(query.value(4).toInt()) + " " +
                                                                query.value(5).toString()));
        i++;
    }
/*
    QStringList navigatorUpperTableHeaders;
    navigatorUpperTableHeaders << "№" << "Наименование" << "Источник данных";
    navigatorUpperTable->setHorizontalHeaderLabels(navigatorUpperTableHeaders);
    navigatorUpperTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    navigatorLowerTable->setColumnCount(4);
    QStringList navigatorLowerTableHeaders;
    navigatorLowerTableHeaders << "Получатель" << "Время данных" << "Состояние отправки" << "Состояние передачи";
    navigatorLowerTable->setHorizontalHeaderLabels(navigatorLowerTableHeaders);
    navigatorLowerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch)
*/
}
/*
void HitTargets::fillChanges() {
    Utility::closeNewEditTab(this);
    changesUpperTable->clear();
    changesLowerTable->clear();

    changesUpperTable->setColumnCount(5);
    QStringList changesUpperTableHeaders;
    changesUpperTableHeaders << "№" << "Наименование" << "Источник" << "Событие" << "Время события";
    changesUpperTable->setHorizontalHeaderLabels(changesUpperTableHeaders);
    changesUpperTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    changesLowerTable->setColumnCount(4);
    QStringList changesLowerTableHeaders;
    changesLowerTableHeaders << "Получатель" << "Время данных" << "Состояние отправки" << "Состояние передачи";
    changesLowerTable->setHorizontalHeaderLabels(changesLowerTableHeaders);
    changesLowerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void HitTargets::slotAddPoint() {
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
    extraCoordinatesLayout->addLayout(coordStringLayout);

    if (extraCoordinatesLayout->count() > 2)
        removePointBtn->setEnabled(true);
}

void HitTargets::slotRemovePoint() {
    QLayout* layout = extraCoordinatesLayout->itemAt(extraCoordinatesLayout->count() - 1)->layout();
    for (int i = 3; i >= 0; i--) {
        delete layout->takeAt(i)->widget();
        delete layout->takeAt(i);
    }
    extraCoordinatesLayout->removeItem(layout);

    if (extraCoordinatesLayout->count() < 3)
        removePointBtn->setEnabled(false);
}

void HitTargets::addFilledPoints() {
    QSqlQuery query;
    QString selectPattern = "SELECT ST_X(geom), ST_Y(geom), ST_Z(geom) FROM (SELECT "
                            "(ST_dumppoints(target_location)).geom FROM obj_targets.target_params "
                            "JOIN reference_data.terms ON termhierarchy = target_name "
                            "WHERE target_number = %1 AND termname = '%2') AS foo";
    QString selectQuery = selectPattern.arg(targetNumberLE->text()).arg(targetNameCB->currentText());

    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    query.next();
    coordinateLE->setText(Utility::convertCoordToDegreeFormat(query.value(0).toDouble()) +
                          Utility::convertCoordToDegreeFormat(query.value(1).toDouble()));
    heightLE->setText(QString::number(query.value(2).toDouble()));


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
        extraCoordinatesLayout->addLayout(coordStringLayout);
    }
    if (extraCoordinatesLayout->count() > 2)
        removePointBtn->setEnabled(true);
}

void HitTargets::slotChangeDataSourceBattery(int n) {
    if (n != -1) {
        if (!this->tabText(this->currentIndex()).contains("Цель")) {
            dataSourceWeaponryCB->clear();
            dataSourceWeaponryCB->addItems(getDataSourceWeaponry());
        }
    }
}

void HitTargets::slotToggleRandomRB() {
    addPointBtn->setHidden(false);
    removePointBtn->setHidden(false);

    for (int i = 0; i < extraCoordinatesLayout->count(); i++) {
        for (int j = 0; j < 4; j++) {
            extraCoordinatesLayout->itemAt(i)->layout()->itemAt(j)->widget()->show();
        }
    }

    depthLbl->setHidden(true);
    frontLbl->setHidden(true);
    deviationLbl->setHidden(true);
    depthLE->setHidden(true);
    frontLE->setHidden(true);
    deviationLE->setHidden(true);

    radiusLbl->setHidden(true);
    radiusLE->setHidden(true);
}

void HitTargets::slotToggleSquareRB() {
    addPointBtn->setHidden(true);
    removePointBtn->setHidden(true);

    for (int i = 0; i < extraCoordinatesLayout->count(); i++) {
        for (int j = 0; j < 4; j++) {
            extraCoordinatesLayout->itemAt(i)->layout()->itemAt(j)->widget()->hide();
        }
    }

    depthLbl->setHidden(false);
    frontLbl->setHidden(false);
    deviationLbl->setHidden(false);
    depthLE->setHidden(false);
    frontLE->setHidden(false);
    deviationLE->setHidden(false);

    radiusLbl->setHidden(true);
    radiusLE->setHidden(true);
}

void HitTargets::slotToggleRoundRB() {
    addPointBtn->setHidden(true);
    removePointBtn->setHidden(true);

    for (int i = 0; i < extraCoordinatesLayout->count(); i++) {
        for (int j = 0; j < 4; j++) {
            extraCoordinatesLayout->itemAt(i)->layout()->itemAt(j)->widget()->hide();
        }
    }

    depthLbl->setHidden(true);
    frontLbl->setHidden(true);
    deviationLbl->setHidden(true);
    depthLE->setHidden(true);
    frontLE->setHidden(true);
    deviationLE->setHidden(true);

    radiusLbl->setHidden(false);
    radiusLE->setHidden(false);
}

void HitTargets::slotToggleLaunchCB() {
    if (launchChB->isChecked()) {
        launchTimeDTE->setEnabled(true);
        if (explosionChB->isChecked()) {
            explosionChB->setChecked(false);
        }
    }
    else {
        launchTimeDTE->setEnabled(false);
    }
}

void HitTargets::slotToggleExplosionCB() {
    if (explosionChB->isChecked()) {
        launchTimeDTE->setEnabled(true);
        if (launchChB->isChecked()) {
            launchChB->setChecked(false);
        }
    }
    else {
        launchTimeDTE->setEnabled(false);
    }
}

void HitTargets::reinitializeFormData() {
    Utility::closeNewEditTab(this);

    dataSourceBatteryCB->clear();
    dataSourceWeaponryCB->clear();
    targetNumberLE->clear();
    targetNameCB->clear();
    importanceLE->clear();

    Utility::clearLayout(extraCoordinatesLayout);
    coordinateLE->clear();
    heightLE->clear();
    removePointBtn->setEnabled(false);
    frontLE->clear();
    depthLE->clear();
    deviationLE->clear();
    radiusLE->clear();
    coverDegreeCB->clear();
    coverDegreeCB->addItems(getCoverDegrees());

    launchChB->setChecked(false);
    explosionChB->setChecked(false);
    launchTimeDTE->setEnabled(false);
    damageDegreeCB->clear();
    rocketTypeCB->clear();
    quantityLE->clear();
    launchTimeDTE->setDateTime(QDateTime::currentDateTime());
    damageDegreeCB->addItems(getDamageDegrees());
    rocketTypeCB->addItems(getRocketTypes());
}

void HitTargets::onAdd() {
    reinitializeFormData();
    this->addTab(contentWidget, "Новый");
    this->setCurrentWidget(contentWidget);

    dataSourceBatteryCB->addItems(getDataSourceBatteries());
    targetNameCB->addItems(getHitTargets());
    detectionTimeDTE->setDateTime(QDateTime::currentDateTime());

    dataSourceBatteryCB->setEnabled(true);
    dataSourceWeaponryCB->setEnabled(true);
    targetNumberLE->setEnabled(true);
    targetNameCB->setEnabled(true);

    if (randomRB->isChecked() == true) {
        slotToggleRandomRB();
    }
    else {
        randomRB->setChecked(true);
    }
    slotAddPoint();
    slotAddPoint();
}

void HitTargets::onEdit() {
    reinitializeFormData();
    this->addTab(contentWidget, "Цель № " + navigatorUpperTable->
                 item(navigatorUpperTable->currentRow(), 0)->text());
    this->setCurrentWidget(contentWidget);

    dataSourceBatteryCB->addItem(navigatorUpperTable->item(navigatorUpperTable->currentRow(), 2)->text().split('/').last());
    dataSourceWeaponryCB->addItem(navigatorUpperTable->item(navigatorUpperTable->currentRow(), 2)->text().split('/').first());
    targetNumberLE->setText(navigatorUpperTable->item(navigatorUpperTable->currentRow(), 0)->text());
    targetNameCB->addItem(navigatorUpperTable->item(navigatorUpperTable->currentRow(), 1)->text());

    dataSourceBatteryCB->setEnabled(false);
    dataSourceWeaponryCB->setEnabled(false);
    targetNumberLE->setEnabled(false);
    targetNameCB->setEnabled(false);

    QSqlQuery query;
    QString selectPattern = "SELECT importance, target_time, target_geometry, t2.termname FROM "
                            "obj_targets.target_params JOIN reference_data.terms t1 ON "
                            "t1.termhierarchy = target_name JOIN reference_data.terms t2 ON "
                            "t2.termhierarchy = cover_degree WHERE target_number = %1 AND "
                            "t1.termname = '%2' ";
    QString selectQuery = selectPattern.arg(targetNumberLE->text()).arg(targetNameCB->currentText());

    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    query.next();
    importanceLE->setText(query.value(0).toString());
    detectionTimeDTE->setDateTime(query.value(1).toDateTime());

    switch (query.value(2).toInt()) {
    case 0: {
        if (randomRB->isChecked() == true)
                slotToggleRandomRB();
        else
            randomRB->setChecked(true);

        addFilledPoints();
        break;
    }
    case 1: {
        slotAddPoint();
        slotAddPoint();
        if (squareRB->isChecked() == true)
            slotToggleSquareRB();
        else
            squareRB->setChecked(true);

        QSqlQuery squareQuery;
        QString squarePatternQuery = "SELECT ST_X(target_location), ST_Y(target_location), "
                                     "ST_Z(target_location), front, depth, deviation FROM "
                                     "obj_targets.target_params JOIN reference_data.terms ON "
                                     "termhierarchy = target_name WHERE target_number = %1 AND "
                                     "termname = '%2'";
        QString squareFullQuery = squarePatternQuery.arg(targetNumberLE->text()).arg(targetNameCB->currentText());

        if (!squareQuery.exec(squareFullQuery)) {
            qDebug() << "Unable to make select operation!" << squareQuery.lastError();
        }
        squareQuery.next();

        coordinateLE->setText(Utility::convertCoordToDegreeFormat(squareQuery.value(0).toDouble()) +
                              Utility::convertCoordToDegreeFormat(squareQuery.value(1).toDouble()));
        heightLE->setText(QString::number(squareQuery.value(2).toDouble()));
        frontLE->setText(QString::number(squareQuery.value(3).toInt()));
        depthLE->setText(QString::number(squareQuery.value(4).toInt()));
        deviationLE->setText(QString::number(squareQuery.value(5).toInt()));
        break;
    }
    case 2: {
        slotAddPoint();
        slotAddPoint();
        if (roundRB->isChecked() == true)
            slotToggleRoundRB();
        else
            roundRB->setChecked(true);

        QSqlQuery roundQuery;
        QString roundPatternQuery = "SELECT ST_X(target_location), ST_Y(target_location), "
                                    "ST_Z(target_location), radius FROM obj_targets.target_params "
                                    "JOIN reference_data.terms ON termhierarchy = target_name "
                                    "WHERE target_number = %1 AND termname = '%2'";
        QString roundFullQuery = roundPatternQuery.arg(targetNumberLE->text()).arg(targetNameCB->currentText());

        if (!roundQuery.exec(roundFullQuery)) {
            qDebug() << "Unable to make select operation!" << roundQuery.lastError();
        }
        roundQuery.next();

        coordinateLE->setText(Utility::convertCoordToDegreeFormat(roundQuery.value(0).toDouble()) +
                              Utility::convertCoordToDegreeFormat(roundQuery.value(1).toDouble()));
        heightLE->setText(QString::number(roundQuery.value(2).toDouble()));
        radiusLE->setText(QString::number(roundQuery.value(3).toInt()));
        break;
    }
    }

    coverDegreeCB->setCurrentText(query.value(3).toString());
}

bool HitTargets::onDelete() {
    QSqlQuery query;
    QString deleteQuery = "UPDATE obj_targets.target_params SET delete_time = now() WHERE "
                            "target_number = ? AND target_name = (SELECT termhierarchy FROM "
                            "reference_data.terms WHERE termname = ?)";
    query.prepare(deleteQuery);
    QString targetNumber = navigatorUpperTable->item(navigatorUpperTable->currentRow(), 0)->text();
    QString targetName = navigatorUpperTable->item(navigatorUpperTable->currentRow(), 1)->text();
    query.addBindValue(targetNumber);
    query.addBindValue(targetName);

    QMessageBox warningDialog(QMessageBox::Warning, "Подтверждение удаления",
                              "Действительно хотить удалить этот элемент?\n" +
                              targetNumber + " " + targetName, QMessageBox::Yes | QMessageBox::No);
    warningDialog.setButtonText(QMessageBox::Yes, "Да");
    warningDialog.setButtonText(QMessageBox::No, "Отмена");

    if (warningDialog.exec() == QMessageBox::Yes) {
        if (!query.exec()) {
            qDebug() << "Unable to make delete operation\n" << query.lastError();
            QMessageBox::critical(this, "Ошибка", "Удалить данные не удалось!");
        }
        else {
            QMessageBox::information(this, "Уведомление", "Удаление прошло успешно!");
            return true;
        }
    }
    return false;
}

bool HitTargets::onSave() {
    if (targetNumberLE->text().isEmpty() || importanceLE->text().isEmpty() ||
            !coordinateLE->text().contains(QRegExp("\\d+")) || heightLE->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
        return false;
    }

    QSqlQuery query;
    QString makePolygonString = "ST_MakePolygon(ST_MakeLine(ARRAY[";
    QString makePointPattern = "ST_MakePoint(%1, %2, %3)";
    QString makePointString = makePointPattern.arg(Utility::convertCoordToDecimalFormat(coordinateLE->text()).at(0))
            .arg(Utility::convertCoordToDecimalFormat(coordinateLE->text()).at(1)).arg(heightLE->text());
    QString makePoint4Polygon = makePointPattern.append(", ");

    makePolygonString.append(makePointString).append(", ");
    for (int i = 0; i < extraCoordinatesLayout->count(); i++) {
        QLineEdit* coord = (QLineEdit*)extraCoordinatesLayout->itemAt(i)->layout()->itemAt(1)->widget();
        QLineEdit* height = (QLineEdit*)extraCoordinatesLayout->itemAt(i)->layout()->itemAt(3)->widget();
        QList<double> coordinates = Utility::convertCoordToDecimalFormat(coord->text());
        makePolygonString.append(makePoint4Polygon.arg(coordinates.at(0))
                                 .arg(coordinates.at(1)).arg(height->text()));
    }
    makePolygonString.append(makePointString).append("]))");

    if (this->tabText(this->currentIndex()).contains("Новый")) {
        QString insertPattern;
        QString insertQuery;

        QString batteryPattern = "SELECT combat_hierarchy FROM own_forces.combatstructure JOIN "
                                 "reference_data.terms ON termhierarchy = object_name WHERE "
                                 "object_number = %1 AND termname = '%2' AND type_army = '22.10' "
                                 "AND type_mode = 0 AND date_delete is null";
        QString weaponryPattern = "SELECT combat_hierarchy FROM own_forces.combatstructure JOIN "
                                  "reference_data.terms ON termhierarchy = object_name WHERE "
                                  "object_number = %1 AND termname = '%2' AND type_mode = 0 "
                                  "AND date_delete is null AND subltree(combat_hierarchy, 0, 1) = (%3)";
        QString batteryTargetNumber = dataSourceBatteryCB->currentText().split(' ').first();
        QString batteryTargetName = dataSourceBatteryCB->currentText().remove(0, batteryTargetNumber.size() + 1);
        QString weaponryTargetNumber = dataSourceWeaponryCB->currentText().split(' ').first();
        QString weaponryTargetName = dataSourceWeaponryCB->currentText().remove(0, weaponryTargetNumber.size() + 1);

        QString dataSourceBatteryString = batteryPattern.arg(batteryTargetNumber).arg(batteryTargetName);
        QString dataSourceWeaponryString = weaponryPattern.arg(weaponryTargetNumber)
                .arg(weaponryTargetName).arg(dataSourceBatteryString);

        if (randomRB->isChecked()) {
            for (int i = 0; i < extraCoordinatesLayout->count(); i++) {
                QLineEdit* coord = (QLineEdit*)extraCoordinatesLayout->itemAt(i)->layout()->itemAt(1)->widget();
                QLineEdit* height = (QLineEdit*)extraCoordinatesLayout->itemAt(i)->layout()->itemAt(3)->widget();
                if (!coord->text().contains(QRegExp("\\d+")) || height->text().isEmpty()) {
                    QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                    return false;
                }
            }
            insertPattern = "INSERT INTO obj_targets.target_params (target_number, target_name, "
                            "importance, target_time, target_geometry, target_location, cover_degree, "
                            "platoon, weaponry) VALUES (?, (SELECT termhierarchy FROM reference_data.terms "
                            "WHERE termname = ?), ?, ?, 0, %1, (SELECT termhierarchy FROM "
                            "reference_data.terms WHERE termname = ?), (%2), (%3))";
            insertQuery = insertPattern.arg(makePolygonString)
                    .arg(dataSourceBatteryString).arg(dataSourceWeaponryString);

            query.prepare(insertQuery);
            query.addBindValue(targetNumberLE->text());
            query.addBindValue(targetNameCB->currentText());
            query.addBindValue(importanceLE->text());
            query.addBindValue(detectionTimeDTE->text());
            query.addBindValue(coverDegreeCB->currentText());
        }
        else if (squareRB->isChecked()) {
            if (frontLE->text().isEmpty() || depthLE->text().isEmpty() || deviationLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            insertPattern = "INSERT INTO obj_targets.target_params (target_number, target_name, "
                            "importance, target_time, target_geometry, target_location, front, "
                            "depth, deviation, cover_degree, platoon, weaponry) VALUES "
                            "(?, (SELECT termhierarchy FROM reference_data.terms WHERE "
                            "termname = ?), ?, ?, 1, %1, ?, ?, ?, (SELECT termhierarchy FROM "
                            "reference_data.terms WHERE termname = ?), (%2), (%3))";
            insertQuery = insertPattern.arg(makePointString)
                    .arg(dataSourceBatteryString).arg(dataSourceWeaponryString);

            query.prepare(insertQuery);
            query.addBindValue(targetNumberLE->text());
            query.addBindValue(targetNameCB->currentText());
            query.addBindValue(importanceLE->text());
            query.addBindValue(detectionTimeDTE->text());
            query.addBindValue(frontLE->text());
            query.addBindValue(depthLE->text());
            query.addBindValue(deviationLE->text());
            query.addBindValue(coverDegreeCB->currentText());
        }
        else if (roundRB->isChecked()) {
            if (radiusLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            insertPattern = "INSERT INTO obj_targets.target_params (target_number, target_name, "
                            "importance, target_time, target_geometry, target_location, "
                            "radius, cover_degree, platoon, weaponry) VALUES "
                            "(?, (SELECT termhierarchy FROM reference_data.terms WHERE "
                            "termname = ?), ?, ?, 2, %1, ?, (SELECT termhierarchy FROM "
                            "reference_data.terms WHERE termname = ?), (%2), (%3))";
            insertQuery = insertPattern.arg(makePointString)
                    .arg(dataSourceBatteryString).arg(dataSourceWeaponryString);

            query.prepare(insertQuery);
            query.addBindValue(targetNumberLE->text());
            query.addBindValue(targetNameCB->currentText());
            query.addBindValue(importanceLE->text());
            query.addBindValue(detectionTimeDTE->text());
            query.addBindValue(radiusLE->text());
            query.addBindValue(coverDegreeCB->currentText());
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
    else if (this->tabText(this->currentIndex()).contains("Цель")) {
        QString updatePattern;
        QString updateQuery;
        if (randomRB->isChecked()) {
            for (int i = 0; i < extraCoordinatesLayout->count(); i++) {
                QLineEdit* coord = (QLineEdit*)extraCoordinatesLayout->itemAt(i)->layout()->itemAt(1)->widget();
                QLineEdit* height = (QLineEdit*)extraCoordinatesLayout->itemAt(i)->layout()->itemAt(3)->widget();
                if (!coord->text().contains(QRegExp("\\d+")) || height->text().isEmpty()) {
                    QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                    return false;
                }
            }
            updatePattern = "UPDATE obj_targets.target_params SET importance = ?, target_time = ?, "
                            "target_geometry = 0, target_location = %1, front = null, depth = null, "
                            "deviation = null, radius = null, cover_degree = "
                            "(SELECT termhierarchy FROM reference_data.terms WHERE termname = ?), "
                            "update_time = now() WHERE target_number = ? AND target_name = "
                            "(SELECT termhierarchy FROM reference_data.terms WHERE termname = ?)";
            updateQuery = updatePattern.arg(makePolygonString);

            query.prepare(updateQuery);
            query.addBindValue(importanceLE->text());
            query.addBindValue(detectionTimeDTE->text());
            query.addBindValue(coverDegreeCB->currentText());
            query.addBindValue(targetNumberLE->text());
            query.addBindValue(targetNameCB->currentText());
        }
        else if (squareRB->isChecked()) {
            if (frontLE->text().isEmpty() || depthLE->text().isEmpty() || deviationLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            updatePattern = "UPDATE obj_targets.target_params SET importance = ?, target_time = ?, "
                            "target_geometry = 1, target_location = %1, front = ?, depth = ?, "
                            "deviation = ?, radius = null, cover_degree = "
                            "(SELECT termhierarchy FROM reference_data.terms WHERE termname = ?), "
                            "update_time = now() WHERE target_number = ? AND target_name = "
                            "(SELECT termhierarchy FROM reference_data.terms WHERE termname = ?)";
            updateQuery = updatePattern.arg(makePointString);

            query.prepare(updateQuery);
            query.addBindValue(importanceLE->text());
            query.addBindValue(detectionTimeDTE->text());
            query.addBindValue(frontLE->text());
            query.addBindValue(depthLE->text());
            query.addBindValue(deviationLE->text());
            query.addBindValue(coverDegreeCB->currentText());
            query.addBindValue(targetNumberLE->text());
            query.addBindValue(targetNameCB->currentText());
        }
        else if (roundRB->isChecked()) {
            if (radiusLE->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка", "Заполнены не все поля!");
                return false;
            }
            updatePattern = "UPDATE obj_targets.target_params SET importance = ?, target_time = ?, "
                            "target_geometry = 2, target_location = %1, front = null, depth = null, "
                            "deviation = null, radius = ?, cover_degree = "
                            "(SELECT termhierarchy FROM reference_data.terms WHERE termname = ?), "
                            "update_time = now() WHERE target_number = ? AND target_name = "
                            "(SELECT termhierarchy FROM reference_data.terms WHERE termname = ?)";
            updateQuery = updatePattern.arg(makePointString);

            query.prepare(updateQuery);
            query.addBindValue(importanceLE->text());
            query.addBindValue(detectionTimeDTE->text());
            query.addBindValue(radiusLE->text());
            query.addBindValue(coverDegreeCB->currentText());
            query.addBindValue(targetNumberLE->text());
            query.addBindValue(targetNameCB->currentText());
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

void HitTargets::setCoordinateLE(QString str) {
    coordinateLE->setText(str);
}

QString HitTargets::constructMessage() {
    QString msg;
    QString targetNumber = navigatorUpperTable->item(navigatorUpperTable->currentRow(), 0)->text();
    QString targetName = navigatorUpperTable->item(navigatorUpperTable->currentRow(), 1)->text();

    QSqlQuery query;
    QString selectPattern = "SELECT target_geometry FROM obj_targets.target_params "
                            "JOIN reference_data.terms ON termhierarchy = target_name "
                            "WHERE target_number = %1 AND termname = '%2'";
    QString selectQuery = selectPattern.arg(targetNumber).arg(targetName);

    if (!query.exec(selectQuery)) {
        qDebug() << query.lastError();
    }
    query.next();

    switch (query.value(0).toInt()) {
    case 0: {
        QSqlQuery randomQuery;
        QString randomPatternQuery = "SELECT ST_X(geom), ST_Y(geom), ST_Z(geom) FROM (SELECT "
                                "(ST_dumppoints(target_location)).geom FROM obj_targets.target_params "
                                "JOIN reference_data.terms ON termhierarchy = target_name "
                                "WHERE target_number = %1 AND termname = '%2') AS foo";
        QString randomFullQuery = randomPatternQuery.arg(targetNumber).arg(targetName);

        if (!randomQuery.exec(randomFullQuery)) {
            qDebug() << "Unable to make select operation!" << query.lastError();
        }

        msg.append("0\n");
        for (int i = 0; i < randomQuery.size() - 1; i++) {
            randomQuery.next();
            msg.append(QString::number(randomQuery.value(0).toDouble()) + " ");
            msg.append(QString::number(randomQuery.value(1).toDouble()) + " ");
            msg.append(QString::number(randomQuery.value(2).toDouble()) + "\n");
        }
        break;
    }
    case 1: {
        QSqlQuery squareQuery;
        QString squarePatternQuery = "SELECT ST_X(target_location), ST_Y(target_location), "
                                     "ST_Z(target_location), front, depth, deviation FROM "
                                     "obj_targets.target_params JOIN reference_data.terms ON "
                                     "termhierarchy = target_name WHERE target_number = %1 AND "
                                     "termname = '%2'";
        QString squareFullQuery = squarePatternQuery.arg(targetNumber).arg(targetName);

        if (!squareQuery.exec(squareFullQuery)) {
            qDebug() << "Unable to make select operation!" << squareQuery.lastError();
        }

        squareQuery.next();
        msg.append("1\n");
        msg.append(QString::number(squareQuery.value(0).toDouble()) + " ");
        msg.append(QString::number(squareQuery.value(1).toDouble()) + " ");
        msg.append(QString::number(squareQuery.value(2).toDouble()) + "\n");
        msg.append(QString::number(squareQuery.value(3).toInt()) + "\n");
        msg.append(QString::number(squareQuery.value(4).toInt()) + "\n");
        msg.append(QString::number(squareQuery.value(5).toInt()) + "\n");
        break;
    }
    case 2: {
        QSqlQuery roundQuery;
        QString roundPatternQuery = "SELECT ST_X(target_location), ST_Y(target_location), "
                                    "ST_Z(target_location), radius FROM obj_targets.target_params "
                                    "JOIN reference_data.terms ON termhierarchy = target_name "
                                    "WHERE target_number = %1 AND termname = '%2'";
        QString roundFullQuery = roundPatternQuery.arg(targetNumberLE->text()).arg(targetNameCB->currentText());

        if (!roundQuery.exec(roundFullQuery)) {
            qDebug() << "Unable to make select operation!" << roundQuery.lastError();
        }

        roundQuery.next();
        msg.append("2\n");
        msg.append(QString::number(roundQuery.value(0).toDouble()) + " ");
        msg.append(QString::number(roundQuery.value(1).toDouble()) + " ");
        msg.append(QString::number(roundQuery.value(2).toDouble()) + "\n");
        msg.append(QString::number(roundQuery.value(3).toInt()) + "\n");
        break;
    }
    }

    return msg;
}

QStringList HitTargets::getDataSourceBatteries() {
    QSqlQuery query;
    QString selectQuery = "SELECT object_number, termname FROM own_forces.combatstructure JOIN "
                          "reference_data.terms ON termhierarchy = object_name WHERE "
                          "combat_hierarchy IN (SELECT combat_hierarchy FROM own_forces.combatstructure "
                          "WHERE nlevel(combat_hierarchy) = 1 AND type_army = '22.10' AND type_mode = 0 "
                          "AND date_delete is null) ORDER BY object_number";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(tr("%1").arg(query.value(0).toInt()) + " " + query.value(1).toString());
    }
    return list;
}

QStringList HitTargets::getDataSourceWeaponry() {
    QString targetNumber = dataSourceBatteryCB->currentText().split(' ').first();
    QString targetName = dataSourceBatteryCB->currentText().remove(0, targetNumber.size() + 1);

    QSqlQuery query;
    QString selectPattern = "SELECT object_number, termname FROM own_forces.combatstructure JOIN "
                            "reference_data.terms ON termhierarchy = object_name WHERE "
                            "subltree(combat_hierarchy, 0, 1) = (SELECT combat_hierarchy FROM "
                            "own_forces.combatstructure JOIN reference_data.terms ON termhierarchy = object_name "
                            "WHERE termname = '%2' AND object_number = %1 AND type_army = '22.10' AND "
                            "type_mode = 0 AND date_delete is null) AND nlevel(combat_hierarchy) = 2 "
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

QStringList HitTargets::getHitTargets() {
    QSqlQuery query;
    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '90.*' AND "
                          "nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    return list;
}

QStringList HitTargets::getCoverDegrees() {
    QSqlQuery query;
    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '95.20.*' AND "
                          "nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    return list;
}

QStringList HitTargets::getDamageDegrees() {
    QSqlQuery query;
    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '95.10.*' AND "
                          "nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    return list;
}

QStringList HitTargets::getRocketTypes() {
    QSqlQuery query;
    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '51.50.*' AND "
                          "termhierarchy != '51.50.30' AND nlevel(termhierarchy) = 3";
    if (!query.exec(selectQuery)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }

    QStringList list;
    while (query.next()) {
        list.append(query.value(0).toString());
    }
    return list;
}
*/
