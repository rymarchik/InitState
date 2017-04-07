#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QtWidgets>

class Server : public QObject
{
    Q_OBJECT

public:
    Server(int port);
    void sendToClient(const QString&);

private:
    QTcpServer* server;
    QTcpSocket* socket;

public slots:
    void slotNewConnection();
    void slotReadClient();
};

#endif // SERVER_H
