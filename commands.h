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
    bool onSave(int number);

private:
    const QString OWN_NAME="Машина боевого управления";
    QTreeWidget* navigatorTree;
    QTreeWidget* changesTree;
    QList<CommandsAddForm *> addWidgetList;
    int saveMode; //0 новый, 1 изменить

    void addCommand(QTreeWidgetItem *parent, QString name1, QDateTime date, QDateTime date2, QString code);
    void addDocument(QTreeWidgetItem *parent, QString name1, QDateTime date, QString name3, QString name4, QString code);
    QString convertCodeToReferenceName(QString code);
    QString convertReferenceNameTOCode(QString referenceName);
    bool saveCommand(QString object, QString command, QString time);
    bool saveDocument(QString object, QString command, QString time);
    bool deleteCommand(QString id);
    bool deleteDocument(QString id);
private slots:
    void showRecivers();

};

#endif // COMMANDS_H
