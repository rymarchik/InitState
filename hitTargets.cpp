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

QWidget* HitTargets::onAdd() {
    HitTargetsTabForm* form = new HitTargetsTabForm;
    formList.append(form);
    form->onAddSetup();
    return form;
}

QWidget* HitTargets::onEdit() {
    HitTargetsTabForm* form = new HitTargetsTabForm;
    formList.append(form);
    form->onEditSetup(navigatorTable);
    return form;
}

bool HitTargets::onSave(int index) {
    HitTargetsTabForm* form = formList.at(index - 2);
    return form->onSaveSetup();
}

bool HitTargets::onDelete() {
    QSqlQuery query;
    QString deleteQuery = "UPDATE obj_targets.target_params SET delete_time = now() WHERE "
                          "target_number = ? AND target_name = (SELECT termhierarchy FROM "
                          "reference_data.terms WHERE termname = ?)";
    query.prepare(deleteQuery);
    QString targetNumber = navigatorTable->item(navigatorTable->currentRow(), 0)->text();
    QString targetName = navigatorTable->item(navigatorTable->currentRow(), 1)->text();
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

void HitTargets::removeForm(int index) {
    formList.removeAt(index - 2);
}

int HitTargets::removeFormFromNavigator() {
    foreach (HitTargetsTabForm* form, formList) {
        QString targetNumber = form->getTargetNumberString();
        QString targetName = form->getTargetNameString();
        if (targetNumber == navigatorTable->item(navigatorTable->currentRow(), 0)->text() &&
                targetName == navigatorTable->item(navigatorTable->currentRow(), 1)->text()) {
            int i = formList.indexOf(form);
            formList.removeAt(i);
            return i + 2;
        }
    }
    return -1;
}

QString HitTargets::getTargetNumber() {
    return "Цель № " + navigatorTable->item(navigatorTable->currentRow(), 0)->text();
}
