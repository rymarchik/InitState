#pragma once
#include "mapsrc/PropertyObj.h"

class NetworkObject
{
public:
	NetworkObject(void);
	~NetworkObject(void);

    tagDataObj data;				//описательная часть объекта для передачи в БД
    QVector <tagDataPoint> metrics;	//метрика объекта
// Блок данных объекта для отображения на карте
// хранить цельным куском без расшифовки
// размер плавающий
	QByteArray masObject;
//----------------------------------------
//поля, которые не идут на сериализацию
    int idObj;	//идентификатор объекта на карте

public:
    //!возвращает размер объекта в байтах при сериализации
    virtual int getSize();
    //!производит сериализацию объекта в линейный массив размера size
    //!метод возвращает количество занятых байт
    virtual int serialize(unsigned char * mas, int size);
    //!производит сериализацию объекта в QByteArray
    //!метод возвращает QByteArray
    QByteArray serialize();
    //!производит распаковку объекта из линейного массива
    virtual int deserialize(unsigned char * mas, int size);

};
