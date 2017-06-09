#include "battleOrder.h"
#include "utility.h"

#include <QDebug>

BattleOrder::BattleOrder(QSqlDatabase db, QTreeWidget *navigatorTree,
                         QTableWidget *navigatorReciversTable, QTableWidget *changesTable,
                         QTableWidget *changesReciversTable, QWidget *parent) :
    navigatorTree(navigatorTree),
    changesTable(changesTable),
    BaseToolClass(db, navigatorReciversTable, changesReciversTable, parent)
{
    connect(this->navigatorTree, SIGNAL(itemSelectionChanged()), this, SLOT(showAttribute()));
}

void BattleOrder::fillNavigator()
{
    navigatorTree->clear();
    navigatorTree->header()->setVisible(true);

    navigatorTree->hideColumn(3); //скрыть колонку "ID"
    navigatorTree->hideColumn(4); //скрыть колонку "ID"
    navigatorTree->rootIsDecorated();

    QStringList list;
    QTreeWidgetItem *rootItem, *classifItem;
    QHash <QString,QTreeWidgetItem*> hashItems;

    rootItem = new QTreeWidgetItem(list);
    classifItem = rootItem;


    QSqlQuery sql = QSqlQuery(db);
    if (!sql.exec(  "SELECT c.combat_hierarchy, c.object_number||' '||t1.termname, t3.termname, t4.termname, "
                    "       CASE t1.termhierarchy    "
                    "            WHEN '53.10' THEN 1 "  //'MBU'
                    "            WHEN '53.20' THEN 2 "  //'BM'
                    "            WHEN '53.30' THEN 3 "  //'TZM'
                    "            ELSE 0 "
                    "       END as attribute "
                    "FROM own_forces.combatstructure c "
                    "LEFT JOIN own_forces.currentmode cur ON cur.combat_hierarchy = c.combat_hierarchy "
                    "LEFT JOIN own_forces.combatobject_manner m ON m.combat_hierarchy = c.combat_hierarchy "
                    "LEFT JOIN reference_data.terms t1 ON c.object_name = t1.termhierarchy "
                    "LEFT JOIN reference_data.terms t2 ON c.type_army   = t2.termhierarchy "
                    "LEFT JOIN reference_data.terms t3 ON m.manner_tid  = t3.termhierarchy "
                    "LEFT JOIN reference_data.terms t4 ON cur.currentmode_tid = t4.termhierarchy "
                    "WHERE c.date_delete is NULL "))
    {
        db.rollback();
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("warning"), sql.lastError().text());
        return;
    }

    while(sql.next())
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();

        item->setText(0, sql.value(1).toString().trimmed());
        item->setText(1, sql.value(2).toString().trimmed());
        item->setText(2, sql.value(3).toString().trimmed());
        item->setText(3, sql.value(4).toString().trimmed());
        item->setText(4, sql.value(0).toString().trimmed());

        QString termhierarchy = sql.value(0).toString().trimmed();
        termhierarchy.contains(".", Qt::CaseInsensitive);
        QString sParentCode;
        if (!termhierarchy.contains(".", Qt::CaseInsensitive))
        {
            navigatorTree->addTopLevelItem(item);
            classifItem->addChild(item);
        }
        else
        {
            QString sCode = termhierarchy.right(2);
            int lendth;
            lendth = termhierarchy.length();
            lendth = lendth-3;
            sParentCode=termhierarchy.left(lendth);
            QHash <QString,QTreeWidgetItem*>::const_iterator i = hashItems.find(sParentCode);
            if (i != hashItems.end())
            {
                rootItem = i.value();
                rootItem->addChild(item);
                navigatorTree->collapseItem(rootItem);
                navigatorTree->expandItem( rootItem );
            }
        }
        hashItems[termhierarchy] = item;
    }

    db.commit();

    navigatorTree->sortByColumn(0, Qt::AscendingOrder);

    QApplication::restoreOverrideCursor();
    classifItem->setExpanded(true);    
}

void BattleOrder::fillChanges()
{
    changesTable->setShowGrid(false); // Выключаем сетку
    changesTable->hideColumn(0);

    QSqlQuery sql = QSqlQuery(db);
    if (!sql.exec(  "SELECT c.combat_hierarchy, concat(c.object_number || ' ' || t0.termname, '/', c2.object_number || ' ' || t1.termname), t3.termname, ' ', now() "
                    "FROM own_forces.combatstructure c "
                    "LEFT JOIN own_forces.combatstructure c2    ON c2.combat_hierarchy = subltree(c.combat_hierarchy,0,1) AND c2.combat_hierarchy ~ '*{1}' "
                    "LEFT JOIN own_forces.currentmode cur       ON cur.combat_hierarchy = c.combat_hierarchy "
                    "LEFT JOIN own_forces.combatobject_manner m ON m.combat_hierarchy = c.combat_hierarchy "
                    "LEFT JOIN reference_data.terms t0          ON c.object_name = t0.termhierarchy "
                    "LEFT JOIN reference_data.terms t1          ON c2.object_name = t1.termhierarchy "
                    "LEFT JOIN reference_data.terms t2          ON m.manner_tid  = t2.termhierarchy "
                    "LEFT JOIN reference_data.terms t3          ON cur.currentmode_tid = t3.termhierarchy "
                    "WHERE c.date_delete IS NULL "
                    "      AND c.combat_hierarchy ~ '!.*{1}'" ))
    {
        db.rollback();
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("warning"), sql.lastError().text());
        return;
    }

    for(int i = 0; sql.next(); i++)
    {
        changesTable->insertRow(i);

        changesTable->setItem(i, 0, new QTableWidgetItem(sql.value(0).toString()));
        changesTable->setItem(i, 1, new QTableWidgetItem(sql.value(1).toString()));
        changesTable->setItem(i, 2, new QTableWidgetItem(sql.value(2).toString()));
        changesTable->setItem(i, 3, new QTableWidgetItem(sql.value(3).toString()));
        changesTable->setItem(i, 4, new QTableWidgetItem(sql.value(4).toString()));
    }
    db.commit();
}

void BattleOrder::removeForm(int)
{

}

QWidget *BattleOrder::onAdd()
{
    return 0;
}

QWidget *BattleOrder::onEdit()
{
    switch (attribute) {
    case 0:
        QMessageBox::critical(0,  ("Ошибка"), ("Выберите запись для просмотра/редактирования !"), QMessageBox::Ok);
        return 0;
        break;
    case 1: //Диалог MBU
        return new battleOrderChangesMBU(db, ID);
        break;
    case 2: //Диалог BM
        return new battleOrderChangesBM(db, ID);
        break;
    case 3: //Диалог TZM
        return new battleOrderChangesTZM;
        break;
    }
    return 0;
}

bool BattleOrder::onDelete()
{
    return 0;
}

bool BattleOrder::onSave(int)
{
    return 0;
}

bool BattleOrder::onSend()
{
    return 0;
}

void BattleOrder::showAttribute()
{
    attribute = navigatorTree->currentItem()->text(3).toInt();  //содержимое скрытого столбца "Признак"
    ID = navigatorTree->currentItem()->text(4);                 //содержимое скрытого столбца "ID"
}
