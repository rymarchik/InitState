/**
\file hitTargets.h
\brief Содержит описание класса HitTargets для поражаемых целей
\date 19.07.2017
\author	Рымарчик А.
*/

#ifndef HITTARGETS_H
#define HITTARGETS_H

#include <QtWidgets>
#include <QtSql>
#include "basetoolclass.h"
#include "hittargetstabform.h"
#include "utility.h"

class MapModule;

class HitTargets : public BaseToolClass
{
    Q_OBJECT

public:
    HitTargets(QSqlDatabase db, QTableWidget *navigatorTable,
               QTableWidget *navigatorReceiversTable, QTableWidget *changesTable,
               QTableWidget *changesReceiversTable, MapModule* map, QWidget* parent = 0);

    //! Метод заполнения верхней таблицы вкладки Навигатор
    void fillNavigator();

    //! Метод заполнения верхней таблицы вкладки Изменения
    void fillChanges();

    /*!
    Метод создания и инициализации формы HitTargetsForm для новой поражаемой цели
    \return возвращает созданную форму HitTargetsForm
    */
    QWidget *onAdd();

    /*!
    Метод создания и инициализации формы HitTargetsForm для редактируемой поражаемой цели
    \return возвращает созданную форму HitTargetsForm
    */
    QWidget *onEdit();

    /*!
    Метод удаления поражаемой цели
    \return возвращает true, если удаление прошло успешно, иначе false
    */
    bool onDelete();

    /*!
    Метод сохранения созданной или редактируемой формы HitTargetsForm
    \param[in] n индекс сохраняемой формы HitTargetsForm
    \return возвращает true, если сохранение прошло успешно, иначе false
    */
    bool onSave(int n);
    bool onSend();

    /*!
    Метод удаления формы HitTargetsForm по индексу из списка форм formList
    \param[in] n индекс формы HitTargetsForm
    */
    void removeForm(int n);

    /*!
    \brief Метод удаления формы HitTargetsForm из списка форм formList

    Используется при удалении поражаемой цели прямо из вкладки Навигатор,
    когда неизвестен индекс формы выбранной цели
    \return возвращает индекс удаленной формы HitTargetsForm
    */
    int removeFormFromNavigator();

    /*!
    Метод формирования названия вкладки
    \return возвращает строку с названием вкладки
    */
    QString getTargetNumber();

private:
    //! Верхняя таблица вкладки Навигатор
    QTableWidget* navigatorTable;
    //! Верхняя таблица вкладки Изменения
    QTableWidget* changesTable;
    //! Список, хранящий формы открытых вкладок с поражаемыми целями
    QList<HitTargetsTabForm*> formList;
    //! Объект карты
    MapModule* map;

public slots:

};

#endif // HITTARGETS_H
