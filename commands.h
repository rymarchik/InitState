#ifndef COMMANDS_H
#define COMMANDS_H

#include <QtWidgets>
#include <QtSql>
#include <QDateTime>
#include <QtUiTools>
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
    bool onSave();

private:
    const QString OWN_NAME="Машина боевого управления";
    QTreeWidget* navigatorTree;
    QTreeWidget* changesTree;
    CommandsAddForm *addWidget;
    int saveMode; //0 новый, 1 изменить

    void addCommand(QTreeWidgetItem *parent, QString name1, QDateTime date, QDateTime date2, QString code);
    void addDocument(QTreeWidgetItem *parent, QString name1, QDateTime date, QString name3, QString name4);

    bool saveCommand(QString object, QString command, QString time);
private slots:
    void showRecivers();

};

#endif // COMMANDS_H
