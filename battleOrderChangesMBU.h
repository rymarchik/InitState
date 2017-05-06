#ifndef BATTLEORDERCHANGESMBU_H
#define BATTLEORDERCHANGESMBU_H

#include <QWidget>
#include <QDebug>
#include <QtSql>
//#include <QSqlDatabase>


namespace Ui {
class battleOrderChangesMBU;
}

class battleOrderChangesMBU : public QWidget
{
    Q_OBJECT

public:
    //explicit
    battleOrderChangesMBU(QSqlDatabase db, QString combatHierarchy, QWidget *parent = 0);
    ~battleOrderChangesMBU();
    //battleOrderChangesMBU *battleOrderForm;

private:
    Ui::battleOrderChangesMBU *ui;

    QSqlDatabase db;
    QString combatHierarchy;

    int m_Mode;

    int getMode();

    void getData();
    void setData();
};

#endif // BATTLEORDERCHANGESMBU_H
