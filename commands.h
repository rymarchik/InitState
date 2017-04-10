#ifndef COMMANDS_H
#define COMMANDS_H

#include <QtWidgets>
#include <QtSql>
#include <QDateTime>
#include <QtUiTools>

class Commands : public QToolBox
{
    Q_OBJECT

public:
    Commands(QSqlDatabase db, QWidget* parent = 0);
    void setTables(QTreeWidget *navigatorUpperTree, QTableWidget *navigatorLowerTable);
    void fillNavigator();
    void fillChanges();
    void onAdd();
    void onEdit();
    bool onDelete();
    bool onSave();

private:
    QSqlDatabase db;
    QTreeWidget* navigatorUpperTree;
    QTableWidget* navigatorLowerTable;
    QWidget* contentWidget;
    int *signalsList;

    void addCommand(QTreeWidgetItem *parent, QString name1, QDateTime date, QDateTime date2);
    void addDocument(QTreeWidgetItem *parent, QString name1, QDateTime date, QString name3, QString name4);

private slots:
    void showRecivers();

};

#endif // COMMANDS_H
