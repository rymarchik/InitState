#ifndef POSITIONS_H
#define POSITIONS_H

#include "baseTabWidget.h"
#include <QtWidgets>

class Positions : public BaseTabWidget
{
    Q_OBJECT

public:
    Positions(QWidget* parent = 0);

    void fillNavigator();
    void fillChanges();
    void onAdd();
    void onEdit();
    bool onDelete();
    bool onSave();

private:
    QTreeWidget* navigatorUpperTree;

public slots:

};

#endif // POSITIONS_H
