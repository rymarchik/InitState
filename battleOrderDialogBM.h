#ifndef BATTLEORDERDIALOGBM_H
#define BATTLEORDERDIALOGBM_H

#include <QDialog>
#include <QString>
#include <QDebug>
#include <QtSql>

#include "battleOrderDialogBM.h"
#include "battleOrderDialog.h"

namespace Ui {
class battleOrderDialogBM;
}

class battleOrderDialog;

class battleOrderDialogBM : public QDialog
{
    Q_OBJECT

public:    
    //explicit
    battleOrderDialogBM(QSqlDatabase db, QString combatHierarchy, QWidget *parent);
    virtual ~battleOrderDialogBM();

    QString combatHierarchy;
    QString munition;

private:
    Ui::battleOrderDialogBM *ui;    
    QSqlDatabase db;

    battleOrderDialog *m_Dialog;
    int numDialog = 0;    

signals:
    void signalRecordDB();

public slots:
    void slotSend() { emit signalRecordDB(); }

private:
    void showDialog(int num);

private slots:
    void slotAdd();
    void slotEdit();
    void slotDelete();
};

#endif // BATTLEORDERDIALOGBM_H
