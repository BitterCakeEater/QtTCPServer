#include "TCPServer.h"

TCPServer::TCPServer() {}

TCPServer::~TCPServer(){}

void TCPServer::startServer()
{
    log << "Server STARTED" << '\n';

    if(this->listen(QHostAddress::Any, 123))
    {
        qDebug() << "Listening...";
        log << "Listening..." << '\n';

        serverStatus = 2;
    }

    else
    {
        qDebug() << "Not listening";
        log << "Not listening" << '\n';
    }
}

void TCPServer::incomingConnection(qintptr handle)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(handle);

    clients.insert(client);

    qDebug() << "New client " << handle << " from: " << client->peerAddress().toString();
    log << "New client " << QString::number(handle).toStdString() << " from:" << client->peerAddress().toString().toStdString() << '\n';

    connect(client, SIGNAL(readyRead()), this, SLOT(socketReady()));
    connect(client, SIGNAL(disconnected()), this, SLOT(socketDisconnect()));
}

void TCPServer::socketReady()
{
    QTcpSocket *clientSocket = (QTcpSocket*)sender();

    while(clientSocket->canReadLine())
    {
        if(serverStatus == 2){
            QString line = QString::fromUtf8(clientSocket->readAll()).trimmed();

            qDebug() << "Read line: " << line;
            log << "Input: " << line.toStdString() << '\n';

            //Connect to server
            if(line[0] == '1')
            {
                bool notEqual = true;
                QString userName = line.mid(2, line.length() - 2);

                if(users.keys().contains(userName))
                    notEqual = false;

                if(notEqual)
                {
                    log << "Added user: " << userName.toStdString() << '\n';
                    users[userName] = clientSocket;

                    sendNewUser(userName);
                }
                else
                {
                    log << "Bad login attempt " << '\n';
                    qDebug() << clientSocket->socketDescriptor() <<" Login is taken";

                    clientSocket->write(QString("3 ").toUtf8());
                }

            }

            //Message
            else if(line[0] == '2')
            {
                int len;
                QStringList pieces = line.split(" ");
                QString nameFrom = pieces[1];
                QString nameTo = pieces[2];

                len = 2 + nameFrom.length() + 1 + nameTo.length() + 1;

                if(users.keys().contains(nameFrom) && users.keys().contains(nameTo))
                {
                    if(nameFrom == nameTo)
                        users[nameFrom]->write(QString("2 " + nameFrom + " " + nameTo + " " + line.mid(len-1, line.length() - len + 1)  + '\n').toUtf8());

                    else
                    {
                        users[nameFrom]->write(QString("2 " + nameFrom + " " + nameTo + " " + line.mid(len-1, line.length() - len + 1)  + '\n').toUtf8());
                        users[nameTo]->write(QString("2 " + nameFrom + " " + nameTo + " " + line.mid(len-1, line.length() - len + 1)  + '\n').toUtf8());
                    }

                    log << "Message form " << nameFrom.toStdString() << " to " << nameTo.toStdString() << ":" << '\n';
                    log << line.mid(len-1, line.length() - len + 1).toStdString() << '\n';
                }

                qDebug() << "User:" << nameFrom;
                //log << "User:" << nameFrom.toStdString() << '\n';
                qDebug() << "Message:" << line.mid(len-1, line.length() - len + 1);
                //log << "Message:" << line.mid(len-1, line.length() - len + 1).toStdString() << '\n';
            }

            else
            {
                qWarning() << "Got bad message from client: " << clientSocket->peerAddress().toString() << line;
            }
        }
    }
}

void TCPServer::socketDisconnect()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    QString userName;
    qDebug() << "Client disconnected:" << client->peerAddress().toString();

    clients.remove(client);

    foreach(QString i, users.keys())
    {
        if(users[i] == client){
            log << "Client " << userName.toStdString() << " disconnected" << '\n';
            userName = i;
            users.remove(i);
            break;
        }

    }

    foreach(QString i, users.keys())
    {
        qDebug() << "0 " + userName;
        users[i]->write(QString("0 " + userName + "\n").toUtf8());
    }

    client->deleteLater();
}

void TCPServer::stopServer()
{
    serverStatus = 0;

    foreach(QTcpSocket *i, clients){
        i->close();
        clients.remove(i);
    }

    foreach(QString i, users.keys()){
        //users[i]->close();
        users.remove(i);
    }

    log << "Server STOPPED" << '\n';

    this->close();
}

void TCPServer::pauseServer()
{
    serverStatus = 1;

    foreach(QString i, users.keys()){
        qDebug() << i;
        users[i]->write(QString("5 " + QString::number(serverStatus) + '\n').toUtf8());
    }

    this->close();

    log << "Server PAUSED" << '\n';
}

void TCPServer::unpauseServer()
{
    this->listen(QHostAddress::Any, 123);

    serverStatus = 2;

    foreach(QString i, users.keys()){
        users[i]->write(QString("5 " + QString::number(serverStatus) + '\n').toUtf8());
    }

    log << "Server STARTED" << '\n';
}

void TCPServer::sendError(QTcpSocket *clientSocket)
{
    qDebug() << "Equal names";
    clientSocket->write((QString("3")).toUtf8());
}

void TCPServer::sendNewUser(QString name)
{
    QStringList userList;

    foreach(QString user, users.keys())
    {
        userList << user;
        if(user != name){
            qDebug() << user << "Sent name: " << name;
            users[user]->write(QString("4 " + name  + '\n').toUtf8());
        }
    }

    users[name]->write(QString("1," + userList.join(",")  + '\n').toUtf8());
}
