#ifndef BATTLEORDER_H
#define BATTLEORDER_H

#include "basetoolclass.h"
#include "battleOrderChangesMBU.h"
#include "battleOrderChangesBM.h"
#include "battleOrderChangesTZM.h"

#include <QtWidgets>
#include <QtSql>
#include <QStringList>
#include <QMessageBox>

class BattleOrder : public BaseToolClass
{
    Q_OBJECT

public:
    BattleOrder(QSqlDatabase db, QTreeWidget *navigatorTree,
                QTableWidget *navigatorReciversTable, QTableWidget *changesTable,
                QTableWidget *changesReciversTable, QWidget* parent = 0);

    void fillNavigator();
    void fillChanges();
    void removeForm(int);
    QWidget *onAdd();
    QWidget *onEdit();
    bool onDelete();
    bool onSave(int);
    bool onSend();

private:
    QTreeWidget*  navigatorTree;
    QTableWidget* changesTable;

    int attribute = 0;
    QString ID;

private slots:
    void showAttribute(); //признак для текущей выбранной записи
};

#endif // BATTLEORDER_H
