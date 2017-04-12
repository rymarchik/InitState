#ifndef BASETOOLCLASS_H
#define BASETOOLCLASS_H

#include <QtWidgets>
#include <QtSql>
#include <QStringList>

class BaseToolClass : public QToolBox
{
public:
    BaseToolClass(QSqlDatabase db, QTableWidget *navigatorReciversTable,
                  QTableWidget *changesReciversTable, QWidget* parent = 0);
    virtual QWidget* onAdd();
    virtual QWidget* onEdit();
    virtual bool onDelete();
    virtual bool onSave();

protected:
    QSqlDatabase  db;
    QTableWidget* navigatorReciversTable;
    QTableWidget* changesReciversTable;
};

#endif // BASETOOLCLASS_H
