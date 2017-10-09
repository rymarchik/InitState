#ifndef _PropertyObj
#define _PropertyObj
#include <QString>
#include <QVector>
#include <QByteArray>
/// возможные поля Структуры свойств объекта необходимые для хранения 
//в базе данных и передачи приложению КАРТА для оотображения

//#define TYPE_METRIC_RECT	1
//#define TYPE_METRIC_CIRCLE	2
//#define TYPE_METRIC_LINE	3
//#define TYPE_METRIC_POINT	4

#define TYPE_METRIC_POINT	1
#define TYPE_METRIC_POINT2	2
#define TYPE_METRIC_LINE	3
#define TYPE_METRIC_RECT	4
#define TYPE_METRIC_CIRCLE	5

struct tagDataObj
{
    tagDataObj() :
    m_OBJECT_ID(-1),m_ENEMY(1),m_ACCESS_LVL(1),m_VISIBLE(1),m_NAME(),m_CODE(1015010110),m_TYPE_ID(1){};
	virtual ~tagDataObj(){};
	int	m_OBJECT_ID;	// ИД объекта в базе данных
	int m_ENEMY;		// признак свой-чужой
	int m_ACCESS_LVL;	// уровень/доступ 
	bool m_VISIBLE;		// признак видимости или лог. удаления
	QString m_NAME;		// имя объекта 
    quint64 m_CODE;     // код цифровой (8-12 цифрр)
	int m_TYPE_ID;		// тип (прямоуг, произв, круг,точка, линия)
};

/// Структура точк для вектора координат "object_coords"
// в геодезической системе координат на эллипсоидеWGS-84 
// в десятичных градусах (например 54.67584,27.0986)
struct tagDataPoint
{
	tagDataPoint() : 
	m_LATITUDE (0.0),m_LONGITUDE(0.0),m_HEIGHT(0.0){};
	virtual ~tagDataPoint(){};
	double	m_LATITUDE;  // широта
	double	m_LONGITUDE; // долгота
	double	m_HEIGHT;     //высота
};

#endif //_PropertyObj


