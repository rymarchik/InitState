#ifndef _PropertyObj
#define _PropertyObj
#include <QString>
#include <QVector>
#include <QByteArray>
/// ��������� ���� ��������� ������� ������� ����������� ��� �������� 
//� ���� ������ � �������� ���������� ����� ��� ������������

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
	int	m_OBJECT_ID;	// �� ������� � ���� ������
	int m_ENEMY;		// ������� ����-�����
	int m_ACCESS_LVL;	// �������/������ 
	bool m_VISIBLE;		// ������� ��������� ��� ���. ��������
	QString m_NAME;		// ��� ������� 
    quint64 m_CODE;     // ��� �������� (8-12 �����)
	int m_TYPE_ID;		// ��� (�������, ������, ����,�����, �����)
};

/// ��������� ���� ��� ������� ��������� "object_coords"
// � ������������� ������� ��������� �� ����������WGS-84 
// � ���������� �������� (�������� 54.67584,27.0986)
struct tagDataPoint
{
	tagDataPoint() : 
	m_LATITUDE (0.0),m_LONGITUDE(0.0),m_HEIGHT(0.0){};
	virtual ~tagDataPoint(){};
	double	m_LATITUDE;  // ������
	double	m_LONGITUDE; // �������
	double	m_HEIGHT;     //������
};

#endif //_PropertyObj


