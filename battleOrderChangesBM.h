#ifndef BATTLEORDERCHANGESBM_H
#define BATTLEORDERCHANGESBM_H

#include <QWidget>
#include <QDebug>
#include <QtSql>

namespace Ui {
class battleOrderChangesBM;
}

class battleOrderChangesBM : public QWidget
{
    Q_OBJECT

public:
    //explicit
    battleOrderChangesBM(QSqlDatabase db, QString combatHierarchy, QWidget *parent = 0);
    ~battleOrderChangesBM();

private:
    Ui::battleOrderChangesBM *ui;

    QSqlDatabase db;
    QString combatHierarchy;

    void getData();
    void setData();
};

#endif // BATTLEORDERCHANGESBM_H
