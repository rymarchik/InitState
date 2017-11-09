#include "hitTargets.h"
#include "mapModule.h"

HitTargets::HitTargets(QSqlDatabase db, QString combatHierarchy, QTableWidget *navigatorTable,
                       QTableWidget *navigatorReceiversTable, QTableWidget *changesTable,
                       QTableWidget *changesReceiversTable, MapModule* map, QWidget *parent) :
    combatHierarchy(combatHierarchy),
    navigatorTable(navigatorTable),
    changesTable(changesTable),
    map(map),
    BaseToolClass(db, navigatorReceiversTable, changesReceiversTable, parent)
{

}

//! Метод заполнения верхней таблицы вкладки Навигатор
void HitTargets::fillNavigator() {
    Utility::initializeTableSettings(navigatorTable);
    Utility::initializeTableSettings(navigatorReceiversTable);

    navigatorTable->setRowCount(0);
    navigatorReceiversTable->setRowCount(0);

    QSqlQuery query = QSqlQuery(db);
    QString selectPattern = "SELECT ot.target_number, t.termname, cs.object_number, t1.termname, "
                            "   cs2.object_number, t2.termname, ot.id_target "
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
        navigatorTable->setItem(i, 3, new QTableWidgetItem(tr("%1").arg(query.value(6).toInt())));
        i++;
    }
    navigatorTable->hideColumn(3);

    navigatorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    navigatorReceiversTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

//! Метод заполнения верхней таблицы вкладки Изменения
void HitTargets::fillChanges() {
    Utility::initializeTableSettings(changesTable);
    Utility::initializeTableSettings(changesReceiversTable);

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
    HitTargetsTabForm* form = new HitTargetsTabForm(db, combatHierarchy, map);
    formList.append(form);
    form->onAddSetup();
    return form;
}

/*!
Метод создания и инициализации формы HitTargetsForm для редактируемой поражаемой цели
\return возвращает созданную форму HitTargetsForm
*/
QWidget* HitTargets::onEdit() {
    HitTargetsTabForm* form = new HitTargetsTabForm(db, combatHierarchy, map);
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

bool HitTargets::onSend() {
    return true;
}

/*!
Метод удаления поражаемой цели
\return возвращает true, если удаление прошло успешно, иначе false
*/
bool HitTargets::onDelete() {  
    QSqlQuery query = QSqlQuery(db);
    QString targetNumber = navigatorTable->item(navigatorTable->currentRow(), 0)->text();
    QString targetName = navigatorTable->item(navigatorTable->currentRow(), 1)->text();
    int targetId = navigatorTable->item(navigatorTable->currentRow(), 3)->text().toInt();
    NetworkObjectManager manager = map->getObjectManager();

    QMessageBox warningDialog(QMessageBox::Warning, "Подтверждение удаления",
                              "Действительно хотить удалить этот элемент?\n" +
                              targetNumber + " " + targetName, QMessageBox::Yes | QMessageBox::No);
    warningDialog.setButtonText(QMessageBox::Yes, "Да");
    warningDialog.setButtonText(QMessageBox::No, "Отмена");

    if (warningDialog.exec() == QMessageBox::Yes) {
        db.transaction();

        QString deleteObject = "UPDATE targets.obj_targets "
                               "SET date_delete = now(), id_manager = ? "
                               "WHERE id_target = ?";
        query.prepare(deleteObject);
        query.addBindValue(idManager);
        query.addBindValue(targetId);
        query.exec();

        QString selectMapObjectId = "SELECT id_terrain_objective "
                                    "FROM map_objects.object_params "
                                    "WHERE CAST(military_object AS INTEGER) = ? "
                                    "      AND date_delete is null";
        query.prepare(selectMapObjectId);
        query.addBindValue(targetId);
        query.exec();
        query.next();
        int mapObjectId = query.value(0).toInt();

        for (int i = 0; i < manager.listObject.size(); i++) {
            qDebug() << QString::number(manager.listObject[i].data.m_OBJECT_ID);
            if (manager.listObject[i].data.m_OBJECT_ID == mapObjectId) {
                //! Запрос на удаление цели из таблицы карты
                QString deleteFromMapObjects = "UPDATE map_objects.object_params "
                                               "SET visibility = false, date_delete = now() "
                                               "WHERE id_terrain_objective = ?";
                query.prepare(deleteFromMapObjects);
                query.addBindValue(mapObjectId);
                if (!query.exec()) {
                    qDebug() << query.lastError();
                }
                manager.listObject.removeAt(i);
                map->setObjectManager(manager);
                break;
            }
        }

        if (!db.commit()) {
            QMessageBox::critical(this, "Ошибка", "Удалить данные не удалось!");
        }
        else {
            QByteArray mas = NetworkModule::Instance().maskData(NETWORK_SEND_MAP, map->getObjectManager().serialize());
            NetworkModule::Instance().sendDataFromMap(mas);
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
