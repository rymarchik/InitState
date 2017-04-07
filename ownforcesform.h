#ifndef OWNFORCESFORM_H
#define OWNFORCESFORM_H

#include <QWidget>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>

namespace Ui {
class OwnForcesForm;
}

class OwnForcesForm : public QWidget
{
    Q_OBJECT

public:
    explicit OwnForcesForm(QWidget *parent = 0);
    ~OwnForcesForm();

    //QSqlDatabase db;
    //OwnForces    l_rszo;

    //void connectionDB(QSqlDatabase DB);

private:
    Ui::OwnForcesForm *ui;
};

#endif // OWNFORCESFORM_H

