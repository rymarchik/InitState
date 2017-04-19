#ifndef BASETOOLCLASS_H
#define BASETOOLCLASS_H

#include <QtWidgets>
#include <QtSql>
#include <QStringList>

class BaseToolClass : public QToolBox
{
public:
    BaseToolClass(QSqlDatabase db, QTableWidget *navigatorReceiversTable,
                  QTableWidget *changesReceiversTable, QWidget* parent);
    virtual void fillNavigator() = 0;
    virtual void fillChanges() = 0;
    virtual QWidget *onAdd() = 0;
    virtual QWidget *onEdit() = 0;
    virtual bool onDelete() = 0;
    virtual bool onSave() = 0;

protected:
    QSqlDatabase  db;
    QTableWidget* navigatorReceiversTable;
    QTableWidget* changesReceiversTable;
};

#endif // BASETOOLCLASS_H
