#include "mapsrc/NetworkObjectManager.h"
#include <QFile>

NetworkObjectManager::NetworkObjectManager(void)
{
}

NetworkObjectManager::~NetworkObjectManager(void)
{
}


bool NetworkObjectManager::load(QString fname)
{
	QFile file(fname);
	if(!file.open(QIODevice::ReadOnly))
        return false;
	deserialize(file.readAll());
	file.close();
	return(true);
}

bool NetworkObjectManager::save(QString fname)
{
	QFile file(fname);
	if(!file.open(QIODevice::WriteOnly))
        return false;
	file.write(serialize());
	file.close();
	return(true);
}



QByteArray NetworkObjectManager::serialize()
{
	QByteArray data;
	int volume=sizeof(int);
	int count;
	unsigned char * mas;
	for(int i=0;i<listObject.size();i++)
	{
		volume+=sizeof(int);
		volume+=listObject[i].getSize();
	}
	mas=new unsigned char [volume];
	count=listObject.size();
	int pos=0;
	memcpy(mas+pos,&count,sizeof(int));
	pos+=sizeof(int);
	for(int i=0;i<count;i++)
	{
		int size=listObject[i].getSize();
		memcpy(mas+pos,&size,sizeof(int));
		pos+=sizeof(int);
		pos+=listObject[i].serialize(mas+pos,volume-pos);
	}
	data.append((const char *)mas,pos);
	delete mas;
	return(data);
}

void NetworkObjectManager::deserialize(QByteArray data)
{
	listObject.clear();
	unsigned char * mas=(unsigned char *)data.data();
	int pos=0;
	int count;
	memcpy(&count,mas+pos,sizeof(int));
	pos+=sizeof(int);
	for(int i=0;i<count;i++)
	{
		NetworkObject obj;
		int size;
		memcpy(&size,mas+pos,sizeof(int));
		pos+=sizeof(int);
		pos+=obj.deserialize(mas+pos,size);
		listObject.push_back(obj);
	}
}
