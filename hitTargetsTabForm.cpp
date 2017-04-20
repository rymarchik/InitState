#include "hitTargetsTabForm.h"
#include "ui_hittargets.h"
#include "utility.h"

HitTargetsTabForm::HitTargetsTabForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HitTargets)
{
    ui->setupUi(this);
//    connect(dataSourceBatteryCB, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeDataSourceBattery(int)));

//    connect(randomRB, SIGNAL(toggled(bool)), this, SLOT(slotToggleRandomRB()));
//    connect(squareRB, SIGNAL(toggled(bool)), this, SLOT(slotToggleSquareRB()));
//    connect(roundRB, SIGNAL(toggled(bool)), this, SLOT(slotToggleRoundRB()));
//    connect(addPointBtn, SIGNAL(clicked()), this, SLOT(slotAddPoint()));
//    connect(removePointBtn, SIGNAL(clicked()), this, SLOT(slotRemovePoint()));

//    connect(launchChB, SIGNAL(clicked()), this, SLOT(slotToggleLaunchCB()));
//    connect(explosionChB, SIGNAL(clicked()), this, SLOT(slotToggleExplosionCB()));
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
    QString selectPattern = "SELECT ST_X(geom), ST_Y(geom), ST_Z(geom) FROM (SELECT "
                            "(ST_dumppoints(target_location)).geom FROM obj_targets.target_params "
                            "JOIN reference_data.terms ON termhierarchy = target_name "
                            "WHERE target_number = %1 AND termname = '%2') AS foo";
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
//    if (n != -1) {
//        if (!this->tabText(this->currentIndex()).contains("Цель")) {
//            dataSourceWeaponryCB->clear();
//            dataSourceWeaponryCB->addItems(getDataSourceWeaponry());
//        }
//    }
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

    ui->dataSourceBatteryCB->clear();
    ui->dataSourceWeaponryCB->clear();
    ui->targetNumberLE->clear();
    ui->targetNameCB->clear();
    ui->importanceLE->clear();

    Utility::clearLayout(ui->extraCoordinatesLayout);
    ui->coordinateLE->clear();
    ui->heightLE->clear();
    ui->removePointBtn->setEnabled(false);
    ui->frontLE->clear();
    ui->depthLE->clear();
    ui->deviationLE->clear();
    ui->radiusLE->clear();
    ui->coverDegreeCB->clear();
    ui->coverDegreeCB->addItems(getCoverDegrees());

    ui->launchChB->setChecked(false);
    ui->explosionChB->setChecked(false);
    ui->launchTimeDTE->setEnabled(false);
    ui->damageDegreeCB->clear();
    ui->rocketTypeCB->clear();
    ui->quantityLE->clear();
    ui->launchTimeDTE->setDateTime(QDateTime::currentDateTime());
    ui->damageDegreeCB->addItems(getDamageDegrees());
    ui->rocketTypeCB->addItems(getRocketTypes());
}


QStringList HitTargetsTabForm::getDataSourceBatteries() {
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

QStringList HitTargetsTabForm::getDataSourceWeaponry() {
    QString targetNumber = ui->dataSourceBatteryCB->currentText().split(' ').first();
    QString targetName = ui->dataSourceBatteryCB->currentText().remove(0, targetNumber.size() + 1);

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

QStringList HitTargetsTabForm::getHitTargets() {
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

QStringList HitTargetsTabForm::getCoverDegrees() {
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

QStringList HitTargetsTabForm::getDamageDegrees() {
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

QStringList HitTargetsTabForm::getRocketTypes() {
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


HitTargetsTabForm::~HitTargetsTabForm()
{
    delete ui;
}
