#include "browserconnection.h"

/**
 * 
 */
BrowserConnection::BrowserConnection(QLocalSocket* socket, QLocalServer* server, Plasmium* plasmium)
    : m_connection(socket), m_server(server), m_plasmium(plasmium)
{
    plasmium->log(QString("New connection"));
    connect(m_connection, SIGNAL(readyRead()), this, SLOT(read()));
    m_waitingForReply = 0;
}//BrowserConnection::BrowserConnection()

/**
 * 
 */
BrowserConnection::~BrowserConnection()
{
    m_connection->close();
    delete m_connection;
}//BrowserConnection::~BrowserConnection()

/*
 * Public slots
 */

/**
 * 
 */
void BrowserConnection::sendSync(const QJsonDocument &message)
{
    m_waitingForReply = qrand()%std::numeric_limits<int>::max();
    QJsonObject o = message.object();
    o.insert("message", m_waitingForReply);
    this->send(QJsonDocument(o));
}//BrowserConnection::sendSync(const QJsonDocument&)

void BrowserConnection::sendAsync(const QJsonDocument &message)
{
    m_waitingForReply = 0;
    this->send(message);
}//BrowserConnection::sendAsync(const QJsonDocument&)

void BrowserConnection::send(const QJsonDocument &message)
{
    m_plasmium->log(QString("Sending message on socket: ") + QString(message.toJson(QJsonDocument::Compact)));

    QByteArray data;
    QDataStream dataFormatter(&data, QIODevice::WriteOnly);
    dataFormatter.setVersion(QDataStream::Qt_4_0);
    // Fill the first 32 bits of data with zeros (fill this later on with the length of the package)
    dataFormatter << (quint32)0;
    // Fill the data block with the binary json serialization
    dataFormatter << message.toBinaryData();
    // Return to the start of the array and write 32 bits of data with the length of data being sent
    dataFormatter.device()->seek(0);
    dataFormatter << (quint32)(data.size() - sizeof(quint32));
    m_connection->write(data);
    m_connection->flush();
}//BrowserConnection::send(const QJsonDocument&)

/**
 * 
 */
bool BrowserConnection::isAlive()
{
    return true;
}//BrowserConnection::isAlive()

/**
 * 
 */
bool BrowserConnection::isConnected()
{
    return true;
}//BrowserConnection::isConnected()

/**
 * 
 */
void BrowserConnection::closeDisconnected()
{
}//BrowserConnection::closeDisconnected()

/*
 * Private slots
 */

/**
 * 
 */
void BrowserConnection::ping()
{
    QJsonObject message
    {
        {"command", "ping"}
    };
    this->sendSync(QJsonDocument(message));
}//BrowserConnection::ping()

/**
 * 
 */
void BrowserConnection::read()
{
    QDataStream in(m_connection);
    in.setVersion(QDataStream::Qt_4_0);
    if (m_blockSize == 0) {
        if (m_connection->bytesAvailable() < (int)sizeof(quint32)) {
            return;
        }
        in >> m_blockSize;
    }

    if (m_connection->bytesAvailable() < m_blockSize) {
        return;
    }

    QByteArray data;
    in >> data;
    QJsonDocument msg = QJsonDocument::fromBinaryData(data, QJsonDocument::BypassValidation);
    if (msg.object().value("reply").toInt(-1) == m_waitingForReply) {
        emit(reply(msg));
    } else {
        emit(message(msg));
    }
}//BrowserConnection::read()

