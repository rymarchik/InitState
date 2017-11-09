#ifndef GPSMODULE_H
#define GPSMODULE_H

#include <QObject>
#include <QtSql>
#include <QSerialPort>
#include <QSerialPortInfo>

class GPSModule : public QObject
{
    Q_OBJECT
public:
    explicit GPSModule(QSqlDatabase db, QString combatHierarchy, QObject *parent = 0);

    /*!
    Метод задания имени порта
    \param[in] portName имя порта
    */
    void setPortName(QString);
    /*!
    Метод записи в БД координат местоположения
    \param[in] latitude широта
    \param[in] longitude долгота
    \param[in] altitude высота
    */
    void updateDatabaseGeoInfo(double latitude, double longitude, double altitude);
    /*!
    Метод получения последнего добавленного в БД значения азимута
    \return значение азимута
    */
    int getLastDirectionValue();

private:
    QSqlDatabase db;
    QString combatHierarchy;
    QSerialPort* serial;
    QTimer* readPortTimer;

    //костыли
    int idManager = 10;
    QString combatHierarchyTemp = "1.11";

public slots:
    //! Слот открытия порта
    void openSerialPort();
    //! Слот закрытия порта
    void closeSerialPort();

private slots:
    //! Слот обработки приходящих от GPS-приемника данных
    void slotParseInput();
};

#endif // GPSMODULE_H
