#pragma once
#include <QString>
#include <QVector>
#include <QByteArray>
#include "mapsrc/NetworkObject.h"

class NetworkObjectManager
{
public:
	NetworkObjectManager(void);
	~NetworkObjectManager(void);
	QVector<NetworkObject> listObject;
	QString fname;
	bool load(QString fname);
	bool save(QString fname);
	QByteArray serialize();
	void deserialize(QByteArray);
};
