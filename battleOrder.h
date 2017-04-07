#ifndef BATTLEORDER_H
#define BATTLEORDER_H

#include <baseTabWidget.h>
#include <QtWidgets>
#include <QtSql>
#include <QStringList>

class BattleOrder : public QToolBox
{
    Q_OBJECT

public:
    BattleOrder(QSqlDatabase DB = QSqlDatabase::database(), QWidget* parent = 0);
    //~BattleOrder();
    QSqlDatabase db;

    void fillNavigator(QTreeWidget *navigatorTreeWidget);
    void fillChanges(QTableWidget *changesTableWidget);

public slots:
};

#endif // BATTLEORDER_H
