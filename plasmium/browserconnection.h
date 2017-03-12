#ifndef BROWSERCONNECTION_H
#define BROWSERCONNECTION_H

#include <QtCore/QObject>
#include <QtNetwork>
#include <QJsonDocument>

#include "plasmium.h"

class Plasmium;

class BrowserConnection : public QObject
{
    Q_OBJECT
    quint32 m_blockSize;
    int m_waitingForReply;
    QLocalSocket* m_connection;
    QLocalServer* m_server;
    Plasmium *m_plasmium;

public:
    BrowserConnection(QLocalSocket* socket, QLocalServer* server, Plasmium *plasmium);
    ~BrowserConnection();

public slots:
    void sendAsync(const QJsonDocument &message);
    void sendSync(const QJsonDocument &message);
    bool isAlive();
    bool isConnected();
    void closeDisconnected();

signals:
    Q_SCRIPTABLE void message(const QJsonDocument &message);
    Q_SCRIPTABLE void reply(const QJsonDocument &reply);
    Q_SCRIPTABLE void quit(const QJsonDocument &message);

private Q_SLOTS:
    void ping();
    void read();
    void send(const QJsonDocument &message);
};

#endif // BROWSERCONNECTION_H
