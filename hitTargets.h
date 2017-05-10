#ifndef HITTARGETS_H
#define HITTARGETS_H

#include <QtWidgets>
#include <QtSql>
#include "basetoolclass.h"
#include "hittargetstabform.h"

class HitTargets : public BaseToolClass
{
    Q_OBJECT

public:
    HitTargets(QSqlDatabase db, QTableWidget *navigatorTable,
               QTableWidget *navigatorReceiversTable, QTableWidget *changesTable,
               QTableWidget *changesReceiversTable, QWidget* parent = 0);
    void fillNavigator();
    void fillChanges();
    QWidget *onAdd();
    QWidget *onEdit();
    bool onDelete();
    bool onSave(int);

    void removeForm(int);
    int removeFormFromNavigator();
    QString getTargetNumber();

private:
    QTableWidget* navigatorTable;
    QTableWidget* changesTable;
    QList<HitTargetsTabForm*> formList;

public slots:

};

#endif // HITTARGETS_H
