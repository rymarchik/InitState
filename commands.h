#ifndef COMMANDS_H
#define COMMANDS_H

#include <QtWidgets>
#include <QtSql>
#include <QDateTime>
#include "basetoolclass.h"
#include "commandsaddform.h"

class Commands : public BaseToolClass
{
    Q_OBJECT

public:
    Commands(QSqlDatabase db, QTreeWidget *navigatorTree,
             QTableWidget *navigatorReciversTable, QTreeWidget *changesTree,
             QTableWidget *changesReciversTable, QWidget* parent = 0);
    void fillNavigator();
    void fillChanges();
    QWidget *onAdd();
    QWidget *onEdit();
    bool onDelete();
    bool onSave(int);

private:
    QTreeWidget* navigatorTree;
    QTreeWidget* changesTree;
    QWidget* contentWidget;
    int *signalsList;

    void addCommand(QTreeWidgetItem *parent, QString name1, QDateTime date, QDateTime date2);
    void addDocument(QTreeWidgetItem *parent, QString name1, QDateTime date, QString name3, QString name4);

private slots:
    void showRecivers();

};

#endif // COMMANDS_H
