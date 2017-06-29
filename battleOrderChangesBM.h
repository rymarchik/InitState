#ifndef BATTLEORDERCHANGESBM_H
#define BATTLEORDERCHANGESBM_H

#include <QWidget>
#include <QDebug>
#include <QtSql>

#include "battleOrderDialogBM.h"

namespace Ui {
class battleOrderChangesBM;
}

class battleOrderDialogBM;

class battleOrderChangesBM : public QWidget
{
    Q_OBJECT

public:
    explicit battleOrderChangesBM(QSqlDatabase db, QString combatHierarchy, QWidget *parent = 0);
    virtual ~battleOrderChangesBM();

//private:
    Ui::battleOrderChangesBM *ui;

    QSqlDatabase db;
    QString combatHierarchy;    
    battleOrderDialogBM *m_DialogBM ;

//private:
    void comboBoxTypeRockets(int num, QString str);
    void comboBoxState(int num, QString str);
    void comboBoxManner(QString str);
    void setData();

    QString getManner();
    QString getCoordinates();
    QString getAzimuth();
    QString getTypeRocket(int number);
    QString getStateRocket(int number);

signals:
    void signalRecordDB();

public slots:
    void slotData();                         //Установка значений для выбранной записи
    void slotStatusStatement();              //Положение о состоянии
    void slotDataOnRockets();                //Данные о ракетах
    void slotTableMunition();                //Данные об имуществе и обеспеченности
    void slotSave();                         //Сохранение изменений в закладке

private slots:
    void slotTableMunitionSignal();
};

#endif // BATTLEORDERCHANGESBM_H
