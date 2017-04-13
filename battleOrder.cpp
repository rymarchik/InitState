#include "battleOrder.h"
#include "utility.h"

#include <QDebug>

BattleOrder::BattleOrder(QSqlDatabase db, QTreeWidget *navigatorTree,
                         QTableWidget *navigatorReciversTable, QTableWidget *changesTable,
                         QTableWidget *changesReciversTable, QWidget *parent)
          : navigatorTree(navigatorTree),
            changesTable(changesTable),
            BaseToolClass(db, navigatorReciversTable, changesReciversTable, parent)
{

}

void BattleOrder::fillNavigator()
{
     QStringList list;
     navigatorTree->clear();
     navigatorTree->setHeaderLabel("");

     QTreeWidgetItem *rootItem, *classifItem;
     QHash <QString,QTreeWidgetItem*> hashItems;

     //list.clear();
     list << "Наименование" << "Состояние" << "Режим";
     rootItem = new QTreeWidgetItem(list);

     navigatorTree->setHeaderLabels(list);
     classifItem = rootItem;

     //qDebug() << db;
     QSqlQuery sql = QSqlQuery(db);

     if (!sql.exec("SELECT c.combat_hierarchy, c.object_number||' '||t1.termname "
                   "FROM own_forces.combatstructure c "
                   "LEFT JOIN reference_data.terms t1 ON object_name = t1.termhierarchy "
                   "LEFT JOIN reference_data.terms t2 ON type_army   = t2.termhierarchy "
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

         item->setText(0,sql.value(1).toString().trimmed());

         //настройка ширины колонки(0, //1, 2):
         //navigatorTreeWidget->resizeColumnToContents(0);

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

     //скрыть 4-ую колонку:
     navigatorTree->hideColumn(3);

     navigatorTree->sortByColumn(0, Qt::AscendingOrder);

     QApplication::restoreOverrideCursor();
     classifItem->setExpanded(true);
}

void BattleOrder::fillChanges()
{
    qDebug()<<"changes";
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
