#include "hitTargets.h"

HitTargets::HitTargets(QSqlDatabase db, QTableWidget *navigatorTable,
                       QTableWidget *navigatorReceiversTable, QTableWidget *changesTable,
                       QTableWidget *changesReceiversTable, QWidget *parent) :
    navigatorTable(navigatorTable),
    changesTable(changesTable),
    BaseToolClass(db, navigatorReceiversTable, changesReceiversTable, parent)
{

}

//! Метод заполнения верхней таблицы вкладки Навигатор
void HitTargets::fillNavigator() {
    Utility::initializeTableSettings(navigatorTable);
    Utility::initializeTableSettings(navigatorReceiversTable);
//    Utility::closeNewEditTab(this);

    navigatorTable->setRowCount(0);
    navigatorReceiversTable->setRowCount(0);

    QSqlQuery query = QSqlQuery(db);
    QString selectPattern = "SELECT ot.target_number, t.termname, cs.object_number, t1.termname, "
                            "   cs2.object_number, t2.termname "
                            "FROM targets.obj_targets ot "
                            "JOIN reference_data.terms t ON ot.target_name = t.termhierarchy "
                            "JOIN own_forces.combatstructure cs ON ot.combat_hierarchy = cs.combat_hierarchy "
                            "JOIN own_forces.combatstructure cs2 ON subltree(ot.combat_hierarchy, 0, 1) = cs2.combat_hierarchy "
                            "JOIN reference_data.terms t1 ON cs.object_name = t1.termhierarchy "
                            "JOIN reference_data.terms t2 ON cs2.object_name = t2.termhierarchy "
                            "WHERE ot.date_delete is null";
    if (!query.exec(selectPattern)) {
        qDebug() << query.lastError();
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

//! Метод заполнения верхней таблицы вкладки Изменения
void HitTargets::fillChanges() {
    Utility::initializeTableSettings(changesTable);
    Utility::initializeTableSettings(changesReceiversTable);
//    Utility::closeNewEditTab(this);

    changesTable->setRowCount(0);
    changesReceiversTable->setRowCount(0);

    changesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    changesReceiversTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

/*!
Метод создания и инициализации формы HitTargetsForm для новой поражаемой цели
\return возвращает созданную форму HitTargetsForm
*/
QWidget* HitTargets::onAdd() {
    HitTargetsTabForm* form = new HitTargetsTabForm(db);
    formList.append(form);
    form->onAddSetup();
    return form;
}

/*!
Метод создания и инициализации формы HitTargetsForm для редактируемой поражаемой цели
\return возвращает созданную форму HitTargetsForm
*/
QWidget* HitTargets::onEdit() {
    HitTargetsTabForm* form = new HitTargetsTabForm(db);
    formList.append(form);
    form->onEditSetup(navigatorTable);
    return form;
}

/*!
Метод сохранения созданной или редактируемой формы HitTargetsForm
\param[in] n индекс сохраняемой формы HitTargetsForm
\return возвращает true, если сохранение прошло успешно, иначе false
*/
bool HitTargets::onSave(int index) {
    HitTargetsTabForm* form = formList.at(index - 2);
    return form->onSaveSetup();
}

bool HitTargets::onSend()
{
    return true;
}

/*!
Метод удаления поражаемой цели
\return возвращает true, если удаление прошло успешно, иначе false
*/
bool HitTargets::onDelete() {
    QSqlQuery query = QSqlQuery(db);
    QString deleteQuery = "UPDATE targets.obj_targets ot "
                          "SET date_delete = now() "
                          "FROM own_forces.combatstructure cs "
                          "WHERE ot.combat_hierarchy = cs.combat_hierarchy "
                          "         AND target_number = ? "
                          "         AND target_name = ? "
                          "         AND type_army = ? "
                          "         AND date_delete is null";
    query.prepare(deleteQuery);
    QString targetNumber = navigatorTable->item(navigatorTable->currentRow(), 0)->text();
    QString targetName = navigatorTable->item(navigatorTable->currentRow(), 1)->text();
    query.addBindValue(targetNumber);
    query.addBindValue(Utility::convertReferenceNameTOCode(db, targetName));
    query.addBindValue("22.10");

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

/*!
Метод удаления формы HitTargetsForm по индексу из списка форм formList
\param[in] n индекс формы HitTargetsForm
*/
void HitTargets::removeForm(int index) {
    formList.removeAt(index - 2);
}

/*!
\brief Метод удаления формы HitTargetsForm из списка форм formList

Используется при удалении поражаемой цели прямо из вкладки Навигатор,
когда неизвестен индекс формы выбранной цели
\return возвращает индекс удаленной формы HitTargetsForm
*/
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

/*!
Метод формирования названия вкладки
\return возвращает строку с названием вкладки
*/
QString HitTargets::getTargetNumber() {
    return "Цель № " + navigatorTable->item(navigatorTable->currentRow(), 0)->text();
}
