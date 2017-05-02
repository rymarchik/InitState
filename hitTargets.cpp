#include "hitTargets.h"
#include "hitTargetsTabForm.h"
#include "utility.h"

HitTargets::HitTargets(QSqlDatabase db, QTableWidget *navigatorTable,
                       QTableWidget *navigatorReceiversTable, QTableWidget *changesTable,
                       QTableWidget *changesReceiversTable, QWidget *parent):
    navigatorTable(navigatorTable),
    changesTable(changesTable),
    BaseToolClass(db, navigatorReceiversTable, changesReceiversTable, parent)
{
}

void HitTargets::fillNavigator() {
    Utility::initializeTableSettings(navigatorTable);
    Utility::initializeTableSettings(navigatorReceiversTable);
//    Utility::closeNewEditTab(this);

    navigatorTable->setRowCount(0);
    navigatorReceiversTable->setRowCount(0);

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

    navigatorTable->setRowCount(query.size());

    int i = 0;
    while (query.next()) {
        navigatorTable->setItem(i, 0, new QTableWidgetItem(tr("%1").arg(query.value(0).toInt())));
        navigatorTable->setItem(i, 1, new QTableWidgetItem(query.value(1).toString()));
        navigatorTable->setItem(i, 2, new QTableWidgetItem(tr("%1").arg(query.value(2).toInt()) + " " +
                                                                query.value(3).toString() + "/" +
                                                                tr("%1").arg(query.value(4).toInt()) + " " +
                                                                query.value(5).toString()));
        i++;
    }

    navigatorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    navigatorReceiversTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void HitTargets::fillChanges() {
    Utility::initializeTableSettings(changesTable);
    Utility::initializeTableSettings(changesReceiversTable);
//    Utility::closeNewEditTab(this);

    changesTable->setRowCount(0);
    changesReceiversTable->setRowCount(0);

    changesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    changesReceiversTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

QWidget *HitTargets::onAdd() {
    HitTargetsTabForm* form = new HitTargetsTabForm;
    form->onAddSetup();
    return form;
}

QWidget *HitTargets::onEdit() {
    HitTargetsTabForm* form = new HitTargetsTabForm;
    form->getDataSourceBatteryCB()->addItem(navigatorTable->item(navigatorTable->currentRow(), 2)->text().split('/').last());
    form->getDataSourceWeaponryCB()->addItem(navigatorTable->item(navigatorTable->currentRow(), 2)->text().split('/').first());
    form->getTargetNumberLE()->setText(navigatorTable->item(navigatorTable->currentRow(), 0)->text());
    form->getTargetNumberCB()->addItem(navigatorTable->item(navigatorTable->currentRow(), 1)->text());
    form->onEditSetup();
    return form;
}

bool HitTargets::onDelete() {
    return 0;
}

bool HitTargets::onSave() {
    return 0;
}

/*
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
*/
