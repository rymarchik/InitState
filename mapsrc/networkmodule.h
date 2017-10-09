/**
\file networkmodule.h
\brief Содержит описаниние класса NetworkModule модуля обеспечения обмена данными по ЛВС
\date 15:01:15
\author	Гаврилюк
\ingroup NetworkModule
\addtogroup NetworkModule
*/

#ifndef NETWORKMODULE_H
#define NETWORKMODULE_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QByteArray>
#include <QString>
#include <QStringList>
class QTcpServer;
class QTcpSocket;


enum TypeNetworkModule
{Offline=-1,Server,Client};

#define NETWORK_SEND_MAP 1
#define NETWORK_INSERT_OBJECT 6
#define NETWORK_INSERT_OBJECT_FROM_DB 7
#define NETWORK_OBJECT_SET_ID 8
#define NETWORK_UPDATE_OBJECT 9
#define NETWORK_DELETE_OBJECT 10
#define NETWORK_GROUP_OBJECT 12
#define NETWORK_UNGROUP_OBJECT 13
#define NETWORK_LAYER_MOVE_OBJECT 14
#define NETWORK_CUT_PASTE_OBJECT 15
#define NETWORK_GET_METRIC 20
#define NETWORK_SEND_METRIC 21

/**\class NetworkModule
\brief Класс модуля для поддержки обмена данными по ЛВС
Содержит методы-обработчики обеспечивающие реализацию обмена данными по ЛВС
*/
class NetworkModule : public QObject
{
    Q_OBJECT

private:
    //!конструктор
    NetworkModule(QObject *parent);
    //!деструктор
    ~NetworkModule();
    //!указатель на статический объект для организации синглетона
    static NetworkModule * _instance;
public:
    /** \fn   Instance
    Метод организации доступа к объекту синглетона
    \return указатель на актуальный объект класса NetworkModule
    */
    static NetworkModule& Instance();

    /** \fn   serverFunc
    Метод создания TCP-сервера
    \return если создание прошло успешно, то возвращает true, иначе false
    */
    bool serverFunc();
    /** \fn   clientFunc
    Метод подключения клиента к серверу
    \param[in] QString сетевой адрес сервера
    \return если подключение прошло успешно, то возвращает true, иначе false
    */
    bool clientFunc(QString);
    /** \fn   closeFunc
    Метод закрытия сетевого подключения
    \return если закрытие и очистка данных прошло успешно, то возвращает true, иначе false
    */
    bool closeFunc();

    /** \fn   sendDataFromMap
    Метод обеспечивает посылку пакета данных по ЛВС с учетом статуса подключения
    \param[in] QByteArray masList массив данных, которые надо отослать по ЛВС
    \return возвращаемых значений нет
    */
    void sendDataFromMap(QByteArray mas);

    /** \fn   sendNetworkMapReq
    Метод посылает запрос на сервер на текущую пользовательскую карту открытую сейчас на сервере
    \return возвращаемых значений нет
    */
    void sendNetworkMapReq();

    /** \fn   sendMetricsReq
    Метод посылает запрос на карту за метрикой
    \return возвращаемых значений нет
    */
    void sendMetricsReq(quint32 type=1);

    /** \fn   typeModule
    Метод возвращает текущее состояние подключения к ЛВС для программного комплекса
    \return возвращает код текущего состояния подключения к ЛВС
    */
    TypeNetworkModule typeModule();	//возвращает тип подключения
    /** \fn   maskData
    Метод осуществляет обертку пакета данных сервисно-контрольным блоком, для дальнейшей передачи по ЛВС
    \param[in] QByteArray data массив данных, который надо обработать
    \return массив данных, подготовленный к отправке по ЛВС
    */
    QByteArray maskData(QByteArray data);
    /** \fn   maskData
    Метод осуществляет обертку пакета данных сервисно-контрольным блоком, для дальнейшей передачи по ЛВС
    \param[in] quint32 command команда, передаваемая с массивом данных
    \param[in] QByteArray data массив данных, который надо обработать
    \return массив данных, подготовленный к отправке по ЛВС
    */
    QByteArray maskData(quint32 command,QByteArray data);
    /** \fn   maskDataString
    Метод осуществляет обертку пакета данных сервисно-контрольным блоком, для дальнейшей передачи по ЛВС.
    В пакете данных упакована строка текста
    \param[in] quint32 command команда, передаваемая с массивом данных
    \param[in] QByteArray data массив данных, который надо обработать
    \return массив данных, подготовленный к отправке по ЛВС
    */
    QByteArray maskDataString(quint32 command,QByteArray data);

    /** \fn   findDataBlock
    Метод производит поиск сформированного пакета данных в потоке данных приходящих по ЛВС
    \param[in] quint32 pos позиция в потоке данных, неачиная с которой надо искать сформированный пакет данных
    \param[in] QByteArray  data массив данных, в который заносятся данные поступающие по ЛВС
    \param[out] QByteArray  res указатель на массив данных, в который будет скопирован найденный, сформированный пакет данных
    \return если в потоке данных не найдено сформированного пакета данных, то возвращает -1, иначе возвращает позицию, с которой надо искать новый пакет данных
    */
    int findDataBlock(quint32 pos,QByteArray  data, QByteArray * res);
    /** \fn   findSampleBlock
    Метод производит поиск сигнатуры сформированного пакета данных в потоке данных приходящих по ЛВС
    \param[in] QByteArray  data массив данных, в который заносятся данные поступающие по ЛВС
    \param[in] quint32 pos позиция в потоке данных, неачиная с которой надо искать сформированный пакет данных
    \return если в потоке данных не найдено сиинатуры сформированного пакета данных, то возвращает -1,
    иначе возвращает позицию, с которой начинается сигнатуры сформированного пакета данных
    */
    int findSampleBlock(QByteArray  data,quint32 pos);

    /** \fn   SendToClient
    Метод отсылает пакет данных от сетвера к клиенту
    \param[in] QTcpSocket* pSocket указатель на сокет на кторый надо отослать пакет данных
    \param[in] QByteArray &mas массив данных, посылаемый по ЛВС
    \return возвращаемых значений нет
    */
    void SendToClient(QTcpSocket* pSocket, QByteArray &mas);

    /** \fn   sendClientName
    Метод отсылает текущее имя клиента серверу для формирования списка подключенных клиентов
    \param[in] QString name текущий ник клиента
    \return возвращаемых значений нет
    */

private:
    //!тип сетевого подключения для текущего экземпляра объета
    TypeNetworkModule _typeModule;
//----------------------------------------------------------
// Данные сервера
    //! сокет сервера
    QTcpServer* m_ptcpServer;
    //! список сокетов поключившихся клиентов
    QList<QTcpSocket *> listSocket;
    //! список имен клиентов, для сервера используется ключ NULL
    QMap<QTcpSocket *,QString> mapNames;
    //! очередь на пересылку данных от сервера
    QMap<QTcpSocket *,QByteArray> mapData;

//----------------------------------------------------------
//данные клиента
    //! сокет для подключения к серверу
    QTcpSocket* m_ptcpSocket;
    //! буфер для приема потока данных приходящих по ЛВС
    QByteArray readMas;
//----------------------------------------------------------
//общие данные
    int port;
public:

    //! строка описывающая текущее состояние сервера
    QString serverStatus;

public slots:

    /** \fn slotNewConnection
    Слот для ситуации нового подключения к серверу
    */
    void slotNewConnection();
    /** \fn slotConnected
    Слот для ситуации когда клиент смог подключиться к серверу
    */
    void slotConnected();
    /** \fn clientSlotDisconnected
    Слот для ситуации когда было разорвано соединение
    */
    void clientSlotDisconnected();
    /** \fn slotReadServer
    Слот для ситуации когда на сервер приходит пакет данных от клиента
    */
    void slotReadServer();
    /** \fn slotReadClient
    Слот для ситуации когда клиенту приходит пакет данных от сервера
    */
    void slotReadClient();

signals:
    void serverError(QString);
    void serverSuccess();
    void serverStatusSignal(QString);
    void addClient();
    void closeClient();
    void sendNetworkUserMap(QTcpSocket* pSocket);
    void receiveNetworkUserMap(QByteArray&);
    void createNetworkUserMap();
    void clientConnected();
    void receiveInsertObjectNetwork(QByteArray&);
    void receiveInsertObjectNetworkFromDB(QByteArray&);
    void receiveInsertObjectNetworkSetID(QByteArray&);
    void receiveMetricsNetworkReq(quint32);
    void receiveMetricsNetwork(QByteArray&);
    void receiveDeleteObjectNetwork(QByteArray&);
    void receiveUpdateObjectNetwork(QByteArray&);

};

#endif // QCALC_H
