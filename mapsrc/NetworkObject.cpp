#include "mapsrc/NetworkObject.h"

#define copyToSerialize(p) \
	lp=(unsigned char *)(&p);\
	for(int i=0;i<sizeof(p);i++)\
	{\
		mas[pos]=lp[i];\
		pos++;\
		if(pos==size)\
			return(pos);\
	}

#define copyToDeserialize(p) \
	lp=(unsigned char *)(&p);\
	for(int i=0;i<sizeof(p);i++)\
	{\
		lp[i]=mas[pos];\
		pos++;\
		if(pos==size)\
			return(pos);\
	}

NetworkObject::NetworkObject(void)
{
    idObj=-1;
}

NetworkObject::~NetworkObject(void)
{
}


//!возвращает размер объекта в байтах при сериализации
int NetworkObject::getSize()
{
//	return(0);
	return(
		sizeof(data.m_OBJECT_ID)+
		sizeof(data.m_ENEMY)+
		sizeof(data.m_ACCESS_LVL)+
		sizeof(data.m_VISIBLE)+
		sizeof(data.m_CODE)+
		sizeof(data.m_TYPE_ID)+
		sizeof(int)+
		data.m_NAME.toUtf8().size()+
		sizeof(int)+
		sizeof(double)*metrics.size()*3+
		sizeof(int)+
		masObject.size()
		);
}



//!производит сериализацию объекта в линейный массив размера size
//!метод возвращает количество зан€тых байт
int NetworkObject::serialize(unsigned char * mas, int size)
{
	int pos=0;
	int sz=0;
	QByteArray buf;
	unsigned char * lp;
	copyToSerialize(data.m_OBJECT_ID)
	copyToSerialize(data.m_ENEMY)
	copyToSerialize(data.m_ACCESS_LVL)
	copyToSerialize(data.m_VISIBLE)
	copyToSerialize(data.m_CODE)
	copyToSerialize(data.m_TYPE_ID)
//----------------------------
	buf=data.m_NAME.toUtf8();
	sz=buf.size();
	copyToSerialize(sz)
	memcpy(mas+pos,buf.data(),sz);
	pos+=sz;
//----------------------------
	sz=metrics.size();
	copyToSerialize(sz)
	for(int ii=0;ii<metrics.size();ii++)
	{
		copyToSerialize(metrics[ii].m_LATITUDE)
		copyToSerialize(metrics[ii].m_LONGITUDE)
		copyToSerialize(metrics[ii].m_HEIGHT)
	}
//-----------------------------
	sz=masObject.size();
	copyToSerialize(sz)
	memcpy(mas+pos,masObject.data(),sz);
	pos+=sz;
	return(pos);
}

QByteArray NetworkObject::serialize()
{
    int size=getSize();
    unsigned char * objMas=new unsigned char[size];
    serialize(objMas,size);
    QByteArray mas=QByteArray((const char *)objMas,size);
    delete objMas;
    return(mas);
}

//!производит распаковку объекта из линейного массива
//!метод возвращает количество байт прочитанных из массива
int NetworkObject::deserialize(unsigned char * mas, int size)
{
	int pos=0;
	unsigned char * lp;
	int sz=0;
	QByteArray buf;

	copyToDeserialize(data.m_OBJECT_ID)
	copyToDeserialize(data.m_ENEMY)
	copyToDeserialize(data.m_ACCESS_LVL)
	copyToDeserialize(data.m_VISIBLE)
	copyToDeserialize(data.m_CODE)
	copyToDeserialize(data.m_TYPE_ID)
//----------------------------
	copyToDeserialize(sz)
	data.m_NAME=QString::fromUtf8(((const char *)(mas+pos)),sz);
	pos+=sz;
//----------------------------
	metrics.clear();
	copyToDeserialize(sz)

	for(int ii=0;ii<sz;ii++)
	{
		tagDataPoint point;
		copyToDeserialize(point.m_LATITUDE)
		copyToDeserialize(point.m_LONGITUDE)
		copyToDeserialize(point.m_HEIGHT)
		metrics.push_back(point);
	}
//-----------------------------
	copyToDeserialize(sz)
	buf=QByteArray(((const char *)(mas+pos)),sz);
	masObject=QByteArray(buf,sz);
	pos+=sz;
	return(pos);
}
