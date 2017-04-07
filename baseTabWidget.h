#ifndef BASETABWIDGET_H
#define BASETABWIDGET_H

#include <QtWidgets>

class BaseTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    BaseTabWidget(QWidget* parent = 0);

    QTableWidget* getNavigatorUpperTable();
    void setNavigatorTab();
    void setChangesTab();
    virtual void fillNavigator() = 0;
    virtual void fillChanges() = 0;
    virtual void onAdd() = 0;
    virtual void onEdit() = 0;
    virtual bool onDelete() = 0;
    virtual bool onSave() = 0;

protected:
    QTableWidget* navigatorUpperTable;
    QTableWidget* navigatorLowerTable;
    QTableWidget* changesUpperTable;
    QTableWidget* changesLowerTable;

    QWidget* navigatorTabWidget;
    QWidget* changesTabWidget;
    QVBoxLayout* navigatorTabLayout;
    QVBoxLayout* changesTabLayout;

    QFont font;

public slots:

};

#endif // BASETABWIDGET_H
