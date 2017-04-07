#ifndef COMMANDS_H
#define COMMANDS_H

#include "baseTabWidget.h"
#include <QtWidgets>
#include <QtSql>
#include <QDateTime>

class Commands : public BaseTabWidget
{
    Q_OBJECT

public:
    Commands(QWidget* parent = 0);

    void fillNavigator();
    void fillChanges();
    void onAdd();
    void onEdit();
    bool onDelete();
    bool onSave();

    QTreeWidget *navigatorTree;
    QTreeWidget *changesTree;

private:
    void makeNoteCommand(QTreeWidgetItem *parent, QString name1, QDateTime date, QDateTime date2);
    void makeNoteDoc(QTreeWidgetItem *parent, QString name1, QDateTime date, QString name3, QString name4);

public slots:

};

#endif // COMMANDS_H
