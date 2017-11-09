/**
\file hitTargetsTabForm.h
\brief Содержит описание класса HitTargetsTabForm для поражаемых целей
\date 19.07.2017
\author	Рымарчик А.
*/

#pragma once

#include <QtWidgets>
#include <QtSql>
#include <QFont>

namespace Ui {
class HitTargets;
}
class MapModule;

class HitTargetsTabForm : public QWidget
{
    Q_OBJECT

public:
    explicit HitTargetsTabForm(QSqlDatabase db, QString combatHierarchy, MapModule* map, QWidget *parent = 0);
    ~HitTargetsTabForm();
    Ui::HitTargets *ui;

    //!Метод инициализации формы для новой поражаемой цели
    void onAddSetup();
    /*!
    Метод инициализации формы для редактируемой поражаемой цели
    \param[in] table верхняя таблица вкладки Навигатор
    */
    void onEditSetup(QTableWidget* table);
    /*!
    \brief Метод сохранения заполненной формы в БД

    Форма сохраняется как для новой, так и для редактируемой цели.
    В обоих случаях координаты ложатся в БД четырьмя разными способами:
    как линия, как ломаная, как прямоугольник и как круг (4 insert и 4 update запроса)
    \return возвращает true, если сохранение прошло успешно, иначе false
    */
    bool onSaveSetup();

    //!Метод заполнения информацией общих для новой и редактируемой цели виджетов
    void addCommonFormData();
    //!Метод заполнения координатами полей при просмотре/правке цели с произвольной геометрией
    void addFilledPoints();
    //!Метод заполнения комбобокса списком батарей
    void getDataSourceBatteries();
    //!Метод заполнения комбобокса списком боевых машин определенной батареи
    void getDataSourceWeaponry();
    //!Метод выбора текущей батареи
    void setCurrentDataSourceBattery();
    //!Метод выбора текущей боевой машины
    void setCurrentDataSourceWeaponry();
    //!Метод заполнения комбобокса списком поражаемых целей (на интерфейсе поле Наименование)
    void getHitTargets();
    //!Метод заполнения комбобокса списком степеней укрытия
    void getCoverDegrees();
    //!Метод заполнения комбобокса списком степеней поражения
    void getDamageDegrees();
    //!Метод заполнения комбобокса списком типов ракет
    void getRocketTypes();

    /*!
    Метод преобразования широты, долготы и высоты, выраженных десятичной дробью,
    в градусы, минуты и секунды
    \param[in] lat широта
    \param[in] lon долгота
    \param[in] alt высота
    \return возвращает строку с координатами в виде градусов, минут и секунд и высотой
    */
    QString getParsedCoordinates(double lat, double lon, double alt);
    /*!
     * Метод создает строку, формирующую линию (ST_MakeLine)
     * из имеющихся двух точек, для записи в БД
     * \return сформированная строка
     */
    QString getMakeLineString();
    /*!
     * Метод создает строку, формирующую полигон (ST_MakePolygon)
     * из имеющихся трех и более точек, для записи в БД
     * \return сформированная строка
     */
    QString getMakePolygonString();
    /*!
    Метод получения значения поля № Цели
    \return возвращает строку со значением поля № Цели
    */
    QString getTargetNumberString();
    /*!
    Метод получения текущего значения поля Наименование
    \return возвращает строку с текущим текстом поля Наименование
    */
    QString getTargetNameString();

private:
    QSqlDatabase db;
    QString combatHierarchy;
    MapModule* map;
    QFont font = QFont("MS Shell Dlg 2", 9, QFont::Bold);

    //костыли
    QString dataSourceBatteryTemp = "1";
    QString dataSourceWeaponryTemp = "1.11";
    int idManager = 10;

signals:
    void insertMapObjectCompleted();

private slots:
    /*!
     * Слот обработки изменения индекса комбобокса Источника данных (батареи)
     * \param n индекс комбобокса
     */
    void slotChangeDataSourceBattery(int n);

    //!Слот обработки открытия карты
    void slotMapOpen();
    //!Слот обработки закрытия карты
    void slotMapClose();
    /*!
    \brief Слот обработки нажатия на кнопку Съем координат

    Посылает запрос на переход в режим съема координат,
    учитывая выбор геометрии цели (ломаная, прямоугольник или круг)
    */
    void slotPickCoordinates();
    //!Слот обработки нажатия на кнопку Добавить точку
    void slotAddPoint();
    //!Слот обработки нажатия на кнопку Убрать точку
    void slotRemovePoint();

    //!Слот обработки нажатия на радиокнопку Произвольная
    void slotToggleRandomRB();
    //!Слот обработки нажатия на радиокнопку Прямоугольник
    void slotToggleSquareRB();
    //!Слот обработки нажатия на радиокнопку Круговая
    void slotToggleRoundRB();
    //!Слот обработки нажатия на чекбокс Пуск
    void slotToggleLaunchCB();
    //!Слот обработки нажатия на чекбокс Взрыв
    void slotToggleExplosionCB();

    /*!
    Слот обработки полученных координат, снятых с карты
    \param[in] data массив данных, содержащий информацию о координатах
    */
    void receiveMetricsNetwork(QByteArray& data);
};
