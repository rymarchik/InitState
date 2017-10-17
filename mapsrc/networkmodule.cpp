#include <QtGui>
#include "mapsrc/networkmodule.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QByteArray>
#include <QDataStream>
#include <QTime>
#include <QSettings>
#include <QStandardItemModel>
#include <QStandardItem>

extern unsigned char sampleMas[];

NetworkModule* NetworkModule::_instance = NULL;
//сигнатура начала блока данных
//#$NETWORK DATA BLOCK
unsigned char sampleMas[]={
	0x23,0x24,0x4e,0x45,0x54,0x57,0x4f,0x52,0x4b,0x0,0x44,0x41,0x54,0x41,0x0,0x42,0x4c,0x4f,0x43,0x4b
};

//!конструктор 
NetworkModule::NetworkModule(QObject *parent)
	: QObject(parent)
{
	m_ptcpServer=NULL;
	m_ptcpSocket=NULL;
	_typeModule=Offline;
}

//!деструктор
NetworkModule::~NetworkModule()
{

}

/** \fn   Instance 
Метод организации доступа к объекту синглетона
\return указатель на актуальный объект класса NetworkModule
*/
NetworkModule& NetworkModule::Instance()
{
	if(_instance==NULL)
	{
		_instance=new NetworkModule(NULL);
	}
	return(*_instance);
}

/** \fn   typeModule 
Метод возвращает текущее состояние подключения к ЛВС для программного комплекса
\return возвращает код текущего состояния подключения к ЛВС
*/
TypeNetworkModule NetworkModule::typeModule()
{
	return(_typeModule);
}

/** \fn   serverFunc 
Метод создания TCP-сервера
\return если создание прошло успешно, то возвращает true, иначе false
*/
bool NetworkModule::serverFunc()
{
	if(m_ptcpServer==NULL)
	{
		closeFunc();
	}
	m_ptcpServer = new QTcpServer(this); 
	if (!m_ptcpServer->listen(QHostAddress::Any, 31000))
	{
//		QMessageBox::critical(0, 
//							  "Server Error", 
//							  "Unable to start the server:" 
//							  + m_ptcpServer->errorString()
//							 );
		m_ptcpServer->close();
		emit serverError(m_ptcpServer->errorString());
		return(false);
	}
	_typeModule=Server;
	connect(m_ptcpServer,SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
	serverStatus=tr("Сервер активен.\nПодключено клиентов: %1").arg(listSocket.size());
	return(true);
}

/** \fn   clientFunc 
Метод подключения клиента к серверу
\param[in] QString сетевой адрес сервера 
\return если подключение прошло успешно, то возвращает true, иначе false
*/
bool NetworkModule::clientFunc(QString host)
{
	if(m_ptcpSocket)
	{
		closeFunc();
	}
	m_ptcpSocket = new QTcpSocket(this);
	m_ptcpSocket->connectToHost(host, 31000);
	m_ptcpSocket->setReadBufferSize(1000000);

    connect(m_ptcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_ptcpSocket, SIGNAL(readyRead()), SLOT(slotReadClient()));

	return(true);
}

/** \fn   closeFunc 
Метод закрытия сетевого подключения
\return если закрытие и очистка данных прошло успешно, то возвращает true, иначе false
*/
bool NetworkModule::closeFunc()
{
	if(m_ptcpSocket==NULL)
	{
		if(m_ptcpServer==NULL)
		{
		}
		else
		{
			m_ptcpServer->close();
			delete m_ptcpServer;
			m_ptcpServer=NULL;
			listSocket.clear();
			mapNames.clear();
			mapData.clear();
			_typeModule=Offline;
		}
	}
	else
	{
		m_ptcpSocket->close();
		delete m_ptcpSocket;
		_typeModule=Offline;
	}
	return(true);
}


/** \fn slotNewConnection 
Слот для ситуации нового подключения к серверу
*/
void NetworkModule::slotNewConnection()
{
	QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
	pClientSocket->setReadBufferSize(1000000);

    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(disconnected()), this, SLOT(clientSlotDisconnected()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadServer()));

	listSocket.push_back(pClientSocket);
	QByteArray temp;
	mapData[pClientSocket]=temp;
	serverStatus=tr("Сервер активен.\nПодключено клиентов: %1").arg(listSocket.size());
	emit serverStatusSignal(serverStatus);
	emit addClient();
}


/** \fn clientSlotDisconnected 
Слот для ситуации когда было разорвано соединение
*/
void NetworkModule::clientSlotDisconnected()
{
	for(int i=0;i<listSocket.size();i++)
	{
		if(listSocket[i]->state()==QAbstractSocket::UnconnectedState)
		{
			mapNames.remove(listSocket[i]);
			mapData.remove(listSocket[i]);

			listSocket.removeAt(i);
			i--;
			continue;
		}
	}
	serverStatus=tr("Сервер активен.\nПодключено клиентов: %1").arg(listSocket.size());
	emit serverStatusSignal(serverStatus);
	emit closeClient();
}


/** \fn slotReadServer 
Слот для ситуации когда на сервер приходит пакет данных от клиента
*/
void NetworkModule::slotReadServer()
{
	QByteArray mas;
	QTcpSocket* pClientSocket = (QTcpSocket*)sender();
//получение блока данных
	mas=pClientSocket->readAll();
	mapData[pClientSocket].append(mas);
	do
	{
		QByteArray dataBlock;
		int pos=findDataBlock(0,mapData[pClientSocket],&dataBlock);
		if(pos<0)
			break;
		mapData[pClientSocket].remove(0,pos+dataBlock.size());
		char * data=dataBlock.data();
		pos=sizeof(quint32);
		quint32     command=0;
		memcpy(&command,data+pos,sizeof(quint32));
		pos+=sizeof(quint32);
		bool fl=true;

        qDebug() << "COMMAND: " << command;
        switch(command)
		{
		case 0:		//квитанция готовности 
			break;
	//------------------------------------------------------
	//работа с картой
		case NETWORK_SEND_MAP:	//запрос на высылку пользовательской карты
			emit sendNetworkUserMap(pClientSocket);
			fl=false;
			break;
        case NETWORK_INSERT_OBJECT: //пришел созданный на карте объект
			emit receiveInsertObjectNetwork(dataBlock);
            break;
        case NETWORK_INSERT_OBJECT_FROM_DB: //пришел созданный на сервере объект с прикрепленным mas_object
            emit receiveInsertObjectNetworkFromDB(dataBlock);
            break;
        case NETWORK_OBJECT_SET_ID: //пока не юзается
            emit receiveInsertObjectNetworkSetID(dataBlock);
            break;
        case NETWORK_DELETE_OBJECT: //на карте удален объект
            emit receiveDeleteObjectNetwork(dataBlock);
            break;
        case NETWORK_SEND_METRIC: //получение координат с режима Съем координат
            emit receiveMetricsNetwork(dataBlock);
            break;
		default:
			break;
		}
		if(fl)
		{
			//циркулярная рассылка полученной информации на остальные клиенты
			QByteArray sendBlock=maskData(dataBlock);
			for(int i=0;i<listSocket.size();i++)
			{
				if(listSocket[i]==pClientSocket)
					continue;
				listSocket[i]->write(sendBlock);
			}
		}
	}while(1);
}


/** \fn slotConnected 
Слот для ситуации когда клиент смог подключиться к серверу
*/
void NetworkModule::slotConnected()
{
	_typeModule=Client;
	emit clientConnected();
	return;
}


/** \fn slotReadClient 
Слот для ситуации когда клиенту приходит пакет данных от сервера
*/
void NetworkModule::slotReadClient() //походу не юзается
{
	QByteArray mas;
	QByteArray dataBlock;
	QTcpSocket* pClientSocket = (QTcpSocket*)sender();
//получение блока данных
	mas=pClientSocket->readAll();
	readMas.append(mas);
	do
	{
		int pos=findDataBlock(0,readMas,&dataBlock);
		if(pos<0)
			break;
		readMas.remove(0,pos+dataBlock.size());
		char * data=dataBlock.data();
		pos=sizeof(quint32);
		quint32     command=0;
        quint32     type=0;
		memcpy(&command,data+pos,sizeof(quint32));
		pos+=sizeof(quint32);
		switch(command)
		{
        case 0:		//квитанция готовности (все эти сигналы по идее юзлесс и не нужны, но пока пусть будут)
			break;
        case NETWORK_SEND_MAP:
            emit receiveNetworkUserMap(dataBlock); //сигнал который посылается когда карта получила все объекты
            break;
        case NETWORK_INSERT_OBJECT:
            emit receiveInsertObjectNetwork(dataBlock);
            break;
        case NETWORK_INSERT_OBJECT_FROM_DB:
            emit receiveInsertObjectNetworkFromDB(dataBlock);
            break;
        case NETWORK_OBJECT_SET_ID:
            emit receiveInsertObjectNetworkSetID(dataBlock);
            break;
        case NETWORK_DELETE_OBJECT:
            emit receiveDeleteObjectNetwork(dataBlock);
            break;
        case NETWORK_GET_METRIC:
            memcpy(&type,data+pos,sizeof(quint32));
            emit receiveMetricsNetworkReq(type);
            break;
		default:
			break;
		}
	}while(1);
	return;
}





/** \fn   maskData 
Метод осуществляет обертку пакета данных сервисно-контрольным блоком, для дальнейшей передачи по ЛВС 
\param[in] QByteArray data массив данных, который надо обработать
\return массив данных, подготовленный к отправке по ЛВС
*/
QByteArray NetworkModule::maskData(QByteArray data)
{
	QByteArray mas=QByteArray((char *)sampleMas,20);
	mas.append(data);
	return(mas);
}


/** \fn   maskData 
Метод осуществляет обертку пакета данных сервисно-контрольным блоком, для дальнейшей передачи по ЛВС 
\param[in] quint32 command команда, передаваемая с массивом данных
\param[in] QByteArray data массив данных, который надо обработать
\return массив данных, подготовленный к отправке по ЛВС
*/
QByteArray NetworkModule::maskData(quint32 command,QByteArray data)
{
	QByteArray mas=QByteArray((char *)sampleMas,20);
	quint32 p1=2*sizeof(quint32)+data.size();
	mas.append((const char *)(&p1),sizeof(quint32));
	p1=command;
	mas.append((const char *)(&p1),sizeof(quint32));
	mas.append(data);
	return(mas);
}

/** \fn   maskDataString 
Метод осуществляет обертку пакета данных сервисно-контрольным блоком, для дальнейшей передачи по ЛВС. 
В пакете данных упакована строка текста
\param[in] quint32 command команда, передаваемая с массивом данных
\param[in] QByteArray data массив данных, который надо обработать
\return массив данных, подготовленный к отправке по ЛВС
*/
QByteArray NetworkModule::maskDataString(quint32 command,QByteArray data)
{
	QByteArray mas=QByteArray((char *)sampleMas,20);
	quint32 p1=3*sizeof(quint32)+data.size();
	mas.append((const char *)(&p1),sizeof(quint32));
	p1=command;
	mas.append((const char *)(&p1),sizeof(quint32));
	mas.append(data);
	p1=0;
	mas.append((const char *)(&p1),sizeof(quint32));
	return(mas);
}

/** \fn   findDataBlock 
Метод производит поиск сформированного пакета данных в потоке данных приходящих по ЛВС
\param[in] quint32 pos позиция в потоке данных, неачиная с которой надо искать сформированный пакет данных
\param[in] QByteArray  data массив данных, в который заносятся данные поступающие по ЛВС
\param[out] QByteArray  res указатель на массив данных, в который будет скопирован найденный, сформированный пакет данных
\return если в потоке данных не найдено сформированного пакета данных, то возвращает -1, иначе возвращает позицию, с которой надо искать новый пакет данных
*/
int NetworkModule::findDataBlock(quint32 pos,QByteArray  data, QByteArray * res)
{
	int rpos=findSampleBlock(data,pos);
	if(rpos<0)
		return(-1);
	char * lp=data.data();
	lp+=rpos+20;
	int sizeBlock;
	memcpy((char *)(&sizeBlock),lp,sizeof(quint32));
	if(sizeBlock>data.size()-rpos-20)
		return(-1);
	res->clear();
	res->append(lp,sizeBlock);
	return(rpos+20);
}

/** \fn   findSampleBlock 
Метод производит поиск сигнатуры сформированного пакета данных в потоке данных приходящих по ЛВС
\param[in] QByteArray  data массив данных, в который заносятся данные поступающие по ЛВС
\param[in] quint32 pos позиция в потоке данных, неачиная с которой надо искать сформированный пакет данных
\return если в потоке данных не найдено сиинатуры сформированного пакета данных, то возвращает -1,
иначе возвращает позицию, с которой начинается сигнатуры сформированного пакета данных
*/
int NetworkModule::findSampleBlock(QByteArray data,quint32 pos)
{
	unsigned char * lp=(unsigned char *)data.data();
	if(pos<0)
		return(-1);
	if(pos>=data.size()-20)
		return(-1);
	for(int i=pos;i<data.size()-20;i++)
	{
		if(lp[i]==sampleMas[0])
		{
			if(lp[i+1]==sampleMas[1])
			{
				bool fl=true;
				for(int j=0;j<20;j++)
				{
					if(lp[i+j]!=sampleMas[j])
					{
						fl=false;
					}
				}
				if(fl)
					return(i);
			}
		}
	}
	return(-1);
}


/** \fn   sendDataFromMap 
Метод обеспечивает посылку пакета данных по ЛВС с учетом статуса подключения
\param[in] QByteArray masList массив данных, которые надо отослать по ЛВС
\return возвращаемых значений нет
*/
void NetworkModule::sendDataFromMap(QByteArray mas)
{
	if(m_ptcpServer)
	{
		for(int i=0;i<listSocket.size();i++)
		{
            listSocket[i]->write(mas);
        }
	}
    else if(m_ptcpSocket)
    {
        m_ptcpSocket->write(mas);
    }
}

/** \fn   sendNetworkMapReq 
Метод посылает запрос на сервер на текущую пользовательскую карту открытую сейчас на сервере
\return возвращаемых значений нет
*/
void NetworkModule::sendNetworkMapReq() // походу не юзается
{
	emit createNetworkUserMap();
	quint32 sl=0;
	sendDataFromMap(maskData(NETWORK_SEND_MAP,QByteArray(((char *)(&sl)),sizeof(quint32))));
}

void NetworkModule::sendMetricsReq(quint32 type)
{
    quint32 sl=type;
    sendDataFromMap(maskData(NETWORK_GET_METRIC,QByteArray(((char *)(&sl)),sizeof(quint32))));
}

/** \fn   SendToClient 
Метод отсылает пакет данных от сетвера к клиенту
\param[in] QTcpSocket* pSocket указатель на сокет на кторый надо отослать пакет данных
\param[in] QByteArray &mas массив данных, посылаемый по ЛВС
\return возвращаемых значений нет
*/
void NetworkModule::SendToClient(QTcpSocket* pSocket, QByteArray &mas)
{
    pSocket->write(mas);
}
