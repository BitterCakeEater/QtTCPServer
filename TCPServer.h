#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <Windows.h>
#include <QApplication>
#include <QObject>
#include <QNetworkInterface>
#include <sstream>

using namespace std;

class TCPServer : public QTcpServer
{
    Q_OBJECT

public:
    TCPServer();
    ~TCPServer();

    QSet<QTcpSocket*> clients;
    QMap<QString, QTcpSocket*> users;
    QByteArray Data;
    int counter;
    int serverStatus;
    stringstream log;

public slots:
    void startServer();
    void incomingConnection(qintptr handle) override;
    void socketReady();
    void socketDisconnect();
    void stopServer();
    void pauseServer();
    void unpauseServer();
    void sendNewUser(QString name);
    void sendError(QTcpSocket *clientSocket);
};

#endif // MYSERVER_H
