#include "server.h"

Server::Server(int port) : QObject()
{
    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, port)) {
        QMessageBox::critical(0, "Server name", "Unable to start the server" + server->errorString());
        server->close();
        return;
    }
    connect(server, SIGNAL(newConnection()), SLOT(slotNewConnection()));
}

void Server::slotNewConnection() {
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), SLOT(slotReadClient()));
    sendToClient("Server: you connected!");
}

void Server::slotReadClient() {

}

void Server::sendToClient(const QString& str) {
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << str;
    socket->write(arrBlock);
}
