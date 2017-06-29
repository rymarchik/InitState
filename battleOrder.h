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

    battleOrderChangesBM *m_changesBM;
    void fillNavigator();
    void fillChanges();
    QWidget *onAdd();
    QWidget *onEdit();
    bool onDelete();
    bool onSave(int);    
    QString getBattleOrderName();

private:
    QTreeWidget*  navigatorTree;
    QTableWidget* changesTable;

    int attribute = 0;
    QString ID;

private slots:    
    void showAttribute(); //признак для текущей выбранной записи
};

#endif // BATTLEORDER_H
