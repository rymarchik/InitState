#ifndef BATTLEORDER_H
#define BATTLEORDER_H

#include <QtWidgets>
#include <QtSql>
#include <QStringList>
#include "basetoolclass.h"

class BattleOrder : public BaseToolClass
{
    Q_OBJECT

public:
    BattleOrder(QSqlDatabase db, QTreeWidget *navigatorTree,
                QTableWidget *navigatorReciversTable, QTableWidget *changesTable,
                QTableWidget *changesReciversTable, QWidget* parent = 0);
    void fillNavigator();
    void fillChanges();
    QWidget *onAdd();
    QWidget *onEdit();
    bool onDelete();
    bool onSave();

private:
    QTreeWidget*  navigatorTree;
    QTableWidget* changesTable;
};

#endif // BATTLEORDER_H
