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
class MapModule;
class NorthSearchUnit;

class battleOrderChangesBM : public QWidget
{
    Q_OBJECT

public:
    explicit battleOrderChangesBM(QSqlDatabase db, QString combatHierarchy, MapModule* map, QWidget *parent = 0);
    virtual ~battleOrderChangesBM();

//private:
    Ui::battleOrderChangesBM *ui;

    QSqlDatabase db;
    QString combatHierarchy;    
    battleOrderDialogBM *m_DialogBM ;
    NorthSearchUnit* northSearchUnit;
    MapModule* map;

    QTimer* clockTimer;
    QTime* time;

    /*!
    Метод преобразования широты, долготы и высоты, выраженных десятичной дробью,
    в градусы, минуты и секунды
    \param[in] lat широта
    \param[in] lon долгота
    \param[in] alt высота
    \return возвращает строку с координатами в виде градусов, минут и секунд и высотой
    */
    QString getParsedCoordinates(double lat, double lon, double alt);

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
    void readyReadAzimuthResult();

public slots:
    void slotData();                         //Установка значений для выбранной записи
    void slotStatusStatement();              //Положение о состоянии
    void slotDataOnRockets();                //Данные о ракетах
    void slotTableMunition();                //Данные об имуществе и обеспеченности
    void slotSave();                         //Сохранение изменений в закладке

    /*!
    \brief Слот обработки нажатия на кнопку Съем координат

    Открывает карту и посылает запрос на переход в режим съема координат
    */
    void slotPickCoordinates();
    /*!
    Слот обработки полученных координат, снятых с карты
    \param[in] data массив данных, содержащий информацию о координатах
    */
    void receiveMetricsNetwork(QByteArray& data);
    void slotAzimuthSearch();
    void slotAzimuthResult();
    void slotShowRemainingTime();

private slots:
    void slotTableMunitionSignal();
};

#endif // BATTLEORDERCHANGESBM_H
