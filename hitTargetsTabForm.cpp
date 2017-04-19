#include "hitTargetsTabForm.h"
#include "ui_hittargets.h"

HitTargetsTabForm::HitTargetsTabForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HitTargets)
{
    ui->setupUi(this);


//    ui->timeCreateDTE->setDateTime(QDateTime::currentDateTime());
//    ui->timeExecDTE->setDateTime(QDateTime::currentDateTime());
//    ui->timeDocRegister->setDateTime(QDateTime::currentDateTime());
//    ui->stackedWidget->setCurrentIndex(0);
//    ui->tableWidget_4->setRowCount(0);
//    setCommandsSignals();
//    setAttributeExecution();



//    connect(ui->timeExecBox, SIGNAL(clicked(bool)), this, SLOT(changeEnabledTimerExec()));
//    connect(ui->menuBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeContent()));
//    connect(ui->addParamBut, SIGNAL(clicked()), addParamForm, SLOT(show()));
//    connect(addParamForm, SIGNAL(sendData(QString, QString)), this, SLOT(recieveData(QString, QString)));
//    connect(ui->addReciverBut, SIGNAL(clicked()), this, SLOT(addRecivers()));


}

//void HitTargets::slotAddPoint() {
//    QLabel* newCoordLbl = new QLabel("Координата");
//    QLabel* newHeightLbl = new QLabel("Высота, м");
//    QLineEdit* newCoordLE = new QLineEdit;
//    QLineEdit* newHeightLE = new QLineEdit;

//    newCoordLbl->setFont(font);
//    newHeightLbl->setFont(font);
//    newCoordLE->setInputMask("B99° 99' 99.99\" \\N     B99° 99' 99.99\" E;_");

//    QHBoxLayout* coordStringLayout = new QHBoxLayout;
//    coordStringLayout->addWidget(newCoordLbl);
//    coordStringLayout->addWidget(newCoordLE);
//    coordStringLayout->addWidget(newHeightLbl);
//    coordStringLayout->addWidget(newHeightLE);
//    extraCoordinatesLayout->addLayout(coordStringLayout);

//    if (extraCoordinatesLayout->count() > 2)
//        removePointBtn->setEnabled(true);
//}

//void HitTargets::slotRemovePoint() {
//    QLayout* layout = extraCoordinatesLayout->itemAt(extraCoordinatesLayout->count() - 1)->layout();
//    for (int i = 3; i >= 0; i--) {
//        delete layout->takeAt(i)->widget();
//        delete layout->takeAt(i);
//    }
//    extraCoordinatesLayout->removeItem(layout);

//    if (extraCoordinatesLayout->count() < 3)
//        removePointBtn->setEnabled(false);
//}

//void HitTargets::addFilledPoints() {
//    QSqlQuery query;
//    QString selectPattern = "SELECT ST_X(geom), ST_Y(geom), ST_Z(geom) FROM (SELECT "
//                            "(ST_dumppoints(target_location)).geom FROM obj_targets.target_params "
//                            "JOIN reference_data.terms ON termhierarchy = target_name "
//                            "WHERE target_number = %1 AND termname = '%2') AS foo";
//    QString selectQuery = selectPattern.arg(targetNumberLE->text()).arg(targetNameCB->currentText());

//    if (!query.exec(selectQuery)) {
//        qDebug() << "Unable to make select operation!" << query.lastError();
//    }

//    query.next();
//    coordinateLE->setText(Utility::convertCoordToDegreeFormat(query.value(0).toDouble()) +
//                          Utility::convertCoordToDegreeFormat(query.value(1).toDouble()));
//    heightLE->setText(QString::number(query.value(2).toDouble()));


//    QHBoxLayout* coordStringLayout;
//    QLabel* newCoordLbl;
//    QLabel* newHeightLbl;
//    QLineEdit* newCoordLE;
//    QLineEdit* newHeightLE;

//    for (int i = 0; i < query.size() - 2; i++) {
//        query.next();

//        newCoordLbl = new QLabel("Координата");
//        newHeightLbl = new QLabel("Высота, м");
//        newCoordLbl->setFont(font);
//        newHeightLbl->setFont(font);

//        newCoordLE = new QLineEdit(Utility::convertCoordToDegreeFormat(query.value(0).toDouble()) +
//                                   Utility::convertCoordToDegreeFormat(query.value(1).toDouble()));
//        newCoordLE->setInputMask("B99° 99' 99.99\" \\N     B99° 99' 99.99\" E;_");
//        newHeightLE = new QLineEdit(QString::number(query.value(2).toDouble()));

//        coordStringLayout = new QHBoxLayout;
//        coordStringLayout->addWidget(newCoordLbl);
//        coordStringLayout->addWidget(newCoordLE);
//        coordStringLayout->addWidget(newHeightLbl);
//        coordStringLayout->addWidget(newHeightLE);
//        extraCoordinatesLayout->addLayout(coordStringLayout);
//    }
//    if (extraCoordinatesLayout->count() > 2)
//        removePointBtn->setEnabled(true);
//}

//void HitTargets::slotChangeDataSourceBattery(int n) {
//    if (n != -1) {
//        if (!this->tabText(this->currentIndex()).contains("Цель")) {
//            dataSourceWeaponryCB->clear();
//            dataSourceWeaponryCB->addItems(getDataSourceWeaponry());
//        }
//    }
//}

//void HitTargets::slotToggleRandomRB() {
//    addPointBtn->setHidden(false);
//    removePointBtn->setHidden(false);

//    for (int i = 0; i < extraCoordinatesLayout->count(); i++) {
//        for (int j = 0; j < 4; j++) {
//            extraCoordinatesLayout->itemAt(i)->layout()->itemAt(j)->widget()->show();
//        }
//    }

//    depthLbl->setHidden(true);
//    frontLbl->setHidden(true);
//    deviationLbl->setHidden(true);
//    depthLE->setHidden(true);
//    frontLE->setHidden(true);
//    deviationLE->setHidden(true);

//    radiusLbl->setHidden(true);
//    radiusLE->setHidden(true);
//}

//void HitTargets::slotToggleSquareRB() {
//    addPointBtn->setHidden(true);
//    removePointBtn->setHidden(true);

//    for (int i = 0; i < extraCoordinatesLayout->count(); i++) {
//        for (int j = 0; j < 4; j++) {
//            extraCoordinatesLayout->itemAt(i)->layout()->itemAt(j)->widget()->hide();
//        }
//    }

//    depthLbl->setHidden(false);
//    frontLbl->setHidden(false);
//    deviationLbl->setHidden(false);
//    depthLE->setHidden(false);
//    frontLE->setHidden(false);
//    deviationLE->setHidden(false);

//    radiusLbl->setHidden(true);
//    radiusLE->setHidden(true);
//}

//void HitTargets::slotToggleRoundRB() {
//    addPointBtn->setHidden(true);
//    removePointBtn->setHidden(true);

//    for (int i = 0; i < extraCoordinatesLayout->count(); i++) {
//        for (int j = 0; j < 4; j++) {
//            extraCoordinatesLayout->itemAt(i)->layout()->itemAt(j)->widget()->hide();
//        }
//    }

//    depthLbl->setHidden(true);
//    frontLbl->setHidden(true);
//    deviationLbl->setHidden(true);
//    depthLE->setHidden(true);
//    frontLE->setHidden(true);
//    deviationLE->setHidden(true);

//    radiusLbl->setHidden(false);
//    radiusLE->setHidden(false);
//}

//void HitTargets::slotToggleLaunchCB() {
//    if (launchChB->isChecked()) {
//        launchTimeDTE->setEnabled(true);
//        if (explosionChB->isChecked()) {
//            explosionChB->setChecked(false);
//        }
//    }
//    else {
//        launchTimeDTE->setEnabled(false);
//    }
//}

//void HitTargets::slotToggleExplosionCB() {
//    if (explosionChB->isChecked()) {
//        launchTimeDTE->setEnabled(true);
//        if (launchChB->isChecked()) {
//            launchChB->setChecked(false);
//        }
//    }
//    else {
//        launchTimeDTE->setEnabled(false);
//    }
//}

//void HitTargets::reinitializeFormData() {
//    Utility::closeNewEditTab(this);

//    dataSourceBatteryCB->clear();
//    dataSourceWeaponryCB->clear();
//    targetNumberLE->clear();
//    targetNameCB->clear();
//    importanceLE->clear();

//    Utility::clearLayout(extraCoordinatesLayout);
//    coordinateLE->clear();
//    heightLE->clear();
//    removePointBtn->setEnabled(false);
//    frontLE->clear();
//    depthLE->clear();
//    deviationLE->clear();
//    radiusLE->clear();
//    coverDegreeCB->clear();
//    coverDegreeCB->addItems(getCoverDegrees());

//    launchChB->setChecked(false);
//    explosionChB->setChecked(false);
//    launchTimeDTE->setEnabled(false);
//    damageDegreeCB->clear();
//    rocketTypeCB->clear();
//    quantityLE->clear();
//    launchTimeDTE->setDateTime(QDateTime::currentDateTime());
//    damageDegreeCB->addItems(getDamageDegrees());
//    rocketTypeCB->addItems(getRocketTypes());
//}



//void HitTargets::setCoordinateLE(QString str) {
//    coordinateLE->setText(str);
//}

//QString HitTargets::constructMessage() {
//    QString msg;
//    QString targetNumber = navigatorUpperTable->item(navigatorUpperTable->currentRow(), 0)->text();
//    QString targetName = navigatorUpperTable->item(navigatorUpperTable->currentRow(), 1)->text();

//    QSqlQuery query;
//    QString selectPattern = "SELECT target_geometry FROM obj_targets.target_params "
//                            "JOIN reference_data.terms ON termhierarchy = target_name "
//                            "WHERE target_number = %1 AND termname = '%2'";
//    QString selectQuery = selectPattern.arg(targetNumber).arg(targetName);

//    if (!query.exec(selectQuery)) {
//        qDebug() << query.lastError();
//    }
//    query.next();

//    switch (query.value(0).toInt()) {
//    case 0: {
//        QSqlQuery randomQuery;
//        QString randomPatternQuery = "SELECT ST_X(geom), ST_Y(geom), ST_Z(geom) FROM (SELECT "
//                                "(ST_dumppoints(target_location)).geom FROM obj_targets.target_params "
//                                "JOIN reference_data.terms ON termhierarchy = target_name "
//                                "WHERE target_number = %1 AND termname = '%2') AS foo";
//        QString randomFullQuery = randomPatternQuery.arg(targetNumber).arg(targetName);

//        if (!randomQuery.exec(randomFullQuery)) {
//            qDebug() << "Unable to make select operation!" << query.lastError();
//        }

//        msg.append("0\n");
//        for (int i = 0; i < randomQuery.size() - 1; i++) {
//            randomQuery.next();
//            msg.append(QString::number(randomQuery.value(0).toDouble()) + " ");
//            msg.append(QString::number(randomQuery.value(1).toDouble()) + " ");
//            msg.append(QString::number(randomQuery.value(2).toDouble()) + "\n");
//        }
//        break;
//    }
//    case 1: {
//        QSqlQuery squareQuery;
//        QString squarePatternQuery = "SELECT ST_X(target_location), ST_Y(target_location), "
//                                     "ST_Z(target_location), front, depth, deviation FROM "
//                                     "obj_targets.target_params JOIN reference_data.terms ON "
//                                     "termhierarchy = target_name WHERE target_number = %1 AND "
//                                     "termname = '%2'";
//        QString squareFullQuery = squarePatternQuery.arg(targetNumber).arg(targetName);

//        if (!squareQuery.exec(squareFullQuery)) {
//            qDebug() << "Unable to make select operation!" << squareQuery.lastError();
//        }

//        squareQuery.next();
//        msg.append("1\n");
//        msg.append(QString::number(squareQuery.value(0).toDouble()) + " ");
//        msg.append(QString::number(squareQuery.value(1).toDouble()) + " ");
//        msg.append(QString::number(squareQuery.value(2).toDouble()) + "\n");
//        msg.append(QString::number(squareQuery.value(3).toInt()) + "\n");
//        msg.append(QString::number(squareQuery.value(4).toInt()) + "\n");
//        msg.append(QString::number(squareQuery.value(5).toInt()) + "\n");
//        break;
//    }
//    case 2: {
//        QSqlQuery roundQuery;
//        QString roundPatternQuery = "SELECT ST_X(target_location), ST_Y(target_location), "
//                                    "ST_Z(target_location), radius FROM obj_targets.target_params "
//                                    "JOIN reference_data.terms ON termhierarchy = target_name "
//                                    "WHERE target_number = %1 AND termname = '%2'";
//        QString roundFullQuery = roundPatternQuery.arg(targetNumberLE->text()).arg(targetNameCB->currentText());

//        if (!roundQuery.exec(roundFullQuery)) {
//            qDebug() << "Unable to make select operation!" << roundQuery.lastError();
//        }

//        roundQuery.next();
//        msg.append("2\n");
//        msg.append(QString::number(roundQuery.value(0).toDouble()) + " ");
//        msg.append(QString::number(roundQuery.value(1).toDouble()) + " ");
//        msg.append(QString::number(roundQuery.value(2).toDouble()) + "\n");
//        msg.append(QString::number(roundQuery.value(3).toInt()) + "\n");
//        break;
//    }
//    }

//    return msg;
//}

//QStringList HitTargets::getDataSourceBatteries() {
//    QSqlQuery query;
//    QString selectQuery = "SELECT object_number, termname FROM own_forces.combatstructure JOIN "
//                          "reference_data.terms ON termhierarchy = object_name WHERE "
//                          "combat_hierarchy IN (SELECT combat_hierarchy FROM own_forces.combatstructure "
//                          "WHERE nlevel(combat_hierarchy) = 1 AND type_army = '22.10' AND type_mode = 0 "
//                          "AND date_delete is null) ORDER BY object_number";
//    if (!query.exec(selectQuery)) {
//        qDebug() << "Unable to make select operation!" << query.lastError();
//    }

//    QStringList list;
//    while (query.next()) {
//        list.append(tr("%1").arg(query.value(0).toInt()) + " " + query.value(1).toString());
//    }
//    return list;
//}

//QStringList HitTargets::getDataSourceWeaponry() {
//    QString targetNumber = dataSourceBatteryCB->currentText().split(' ').first();
//    QString targetName = dataSourceBatteryCB->currentText().remove(0, targetNumber.size() + 1);

//    QSqlQuery query;
//    QString selectPattern = "SELECT object_number, termname FROM own_forces.combatstructure JOIN "
//                            "reference_data.terms ON termhierarchy = object_name WHERE "
//                            "subltree(combat_hierarchy, 0, 1) = (SELECT combat_hierarchy FROM "
//                            "own_forces.combatstructure JOIN reference_data.terms ON termhierarchy = object_name "
//                            "WHERE termname = '%2' AND object_number = %1 AND type_army = '22.10' AND "
//                            "type_mode = 0 AND date_delete is null) AND nlevel(combat_hierarchy) = 2 "
//                            "ORDER BY object_number";
//    QString selectQuery = selectPattern.arg(targetNumber).arg(targetName);
//    if (!query.exec(selectQuery)) {
//        qDebug() << "Unable to make select operation!" << query.lastError();
//    }

//    QStringList list;
//    while (query.next()) {
//        list.append(tr("%1").arg(query.value(0).toInt()) + " " + query.value(1).toString());
//    }
//    return list;
//}

//QStringList HitTargets::getHitTargets() {
//    QSqlQuery query;
//    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '90.*' AND "
//                          "nlevel(termhierarchy) = 3";
//    if (!query.exec(selectQuery)) {
//        qDebug() << "Unable to make select operation!" << query.lastError();
//    }

//    QStringList list;
//    while (query.next()) {
//        list.append(query.value(0).toString());
//    }
//    return list;
//}

//QStringList HitTargets::getCoverDegrees() {
//    QSqlQuery query;
//    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '95.20.*' AND "
//                          "nlevel(termhierarchy) = 3";
//    if (!query.exec(selectQuery)) {
//        qDebug() << "Unable to make select operation!" << query.lastError();
//    }

//    QStringList list;
//    while (query.next()) {
//        list.append(query.value(0).toString());
//    }
//    return list;
//}

//QStringList HitTargets::getDamageDegrees() {
//    QSqlQuery query;
//    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '95.10.*' AND "
//                          "nlevel(termhierarchy) = 3";
//    if (!query.exec(selectQuery)) {
//        qDebug() << "Unable to make select operation!" << query.lastError();
//    }

//    QStringList list;
//    while (query.next()) {
//        list.append(query.value(0).toString());
//    }
//    return list;
//}

//QStringList HitTargets::getRocketTypes() {
//    QSqlQuery query;
//    QString selectQuery = "SELECT termname FROM reference_data.terms WHERE termhierarchy ~ '51.50.*' AND "
//                          "termhierarchy != '51.50.30' AND nlevel(termhierarchy) = 3";
//    if (!query.exec(selectQuery)) {
//        qDebug() << "Unable to make select operation!" << query.lastError();
//    }

//    QStringList list;
//    while (query.next()) {
//        list.append(query.value(0).toString());
//    }
//    return list;
//}


HitTargetsTabForm::~HitTargetsTabForm()
{
    delete ui;
}
