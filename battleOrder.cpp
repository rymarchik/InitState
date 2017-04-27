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

}

void BattleOrder::fillNavigator()
{
    navigatorTree->clear();
    navigatorTree->header()->setVisible(true);

    QStringList list;
    QTreeWidgetItem *rootItem, *classifItem;
    QHash <QString,QTreeWidgetItem*> hashItems;

    rootItem = new QTreeWidgetItem(list);
    classifItem = rootItem;

    QSqlQuery sql = QSqlQuery(db);
    if (!sql.exec(  "select c.combat_hierarchy, c.object_number||' '||t1.termname, t3.termname, t4.termname "
                    "from own_forces.combatstructure c "
                    "left join own_forces.currentmode cur on cur.combat_hierarchy = c.combat_hierarchy "
                    "left join own_forces.combatobject_manner m on m.combat_hierarchy = c.combat_hierarchy "
                    "LEFT JOIN reference_data.terms t1 ON c.object_name = t1.termhierarchy "
                    "LEFT JOIN reference_data.terms t2 ON c.type_army   = t2.termhierarchy "
                    "LEFT JOIN reference_data.terms t3 ON m.manner_tid  = t3.termhierarchy "
                    "LEFT JOIN reference_data.terms t4 ON cur.currentmode_tid = t4.termhierarchy "
                    "where c.date_delete is NULL "))
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
                //ui->treeWidget->collapseItem(rootItem);
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

QWidget *BattleOrder::onAdd()
{
    return 0;
}

QWidget *BattleOrder::onEdit()
{
    return 0;
}

bool BattleOrder::onDelete()
{
    return 0;
}

bool BattleOrder::onSave()
{
    return 0;
}
