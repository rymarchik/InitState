#ifndef BATTLEORDERDIALOG_H
#define BATTLEORDERDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDebug>
#include <QtSql>
#include <QSqlError>
#include <QMessageBox>

namespace Ui {
class battleOrderDialog;
}

class battleOrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit battleOrderDialog(QSqlDatabase db, QString combatHierarchy, QString munitionName, int numDialog, QWidget *parent = 0);
    virtual ~battleOrderDialog();

    QSqlDatabase db;
    QString combatHierarchy;
    QString munition;
    int numDlg;

    void setDisabledTextEdit(bool x, bool y);
    void getCountPlace(int numDialog);
    void getComboBoxName(QString str = "");

    QString getMunitionName();
    int getCount();
    QString getPlace();

private:
    Ui::battleOrderDialog *ui;

signals:
    void signalRecordDB();

public slots:
    void slotSendSignal() { emit signalRecordDB(); }

private slots:
    void slotAccept();
};

#endif // BATTLEORDERDIALOG_H
