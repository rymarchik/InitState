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
class GPSModule;
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
    MapModule* map;
    GPSModule* gps;
    NorthSearchUnit* northSearchUnit;

    QTimer* clockTimer;
    QTime* time;

    int idManager = 10; //костыль

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
    //! Сигнал, предупреждающий об истечении времени на таймере
    void readyReadAzimuthResult();

public slots:
    void slotData();                         //Установка значений для выбранной записи
    void slotStatusStatement();              //Положение о состоянии
    void slotDataOnRockets();                //Данные о ракетах
    void slotTableMunition();                //Данные об имуществе и обеспеченности
    void slotSave();                         //Сохранение изменений в закладке

    //! Слот обработки открытия карты
    void slotMapOpen();
    //! Слот обработки закрытия карты
    void slotMapClose();
    //! Слот открытия порта для GPS-приемника
    void slotOpenPortGPS();
    //! Слот закрытия порта для GPS-приемника
    void slotClosePortGPS();
    //! Слот открытия порта для АПС
    void slotOpenPortAPS();
    //! Слот закрытия порта для АПС
    void slotClosePortAPS();

    /*!
    \brief Слот обработки нажатия на кнопку Съем координат

    Посылает запрос карте на переход в режим съема координат
    */
    void slotPickCoordinates();
    /*!
    Слот обработки полученных координат, снятых с карты
    \param[in] data массив данных, содержащий информацию о координатах
    */
    void receiveMetricsNetwork(QByteArray& data);
    /*!
    \brief Слот обработки нажатия на кнопку Поиск севера

    Посылает запрос АПС на поиск севера и запускает трехминутный таймер
    */
    void slotAzimuthSearch();
    //! Слот чтения порта АПС по истечению времени и получение результата
    void slotAzimuthResult();
    //! Слот работы таймера
    void slotShowRemainingTime();

private slots:
    void slotTableMunitionSignal();
};

#endif // BATTLEORDERCHANGESBM_H
