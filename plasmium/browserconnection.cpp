#include "browserconnection.h"

/**
 * 
 */
BrowserConnection::BrowserConnection(connection, server, plasmium)
    : m_connection(connection), m_server(server), m_plasmium(plasmium)
{
    Q_OBJECT
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
void BrowserConnection::send(const QJsonDocument &message)
{
}//BrowserConnection::send(const QJsonDocument&)

/**
 * 
 */
bool BrowserConnection::isAlive()
{
}//BrowserConnection::isAlive()

/**
 * 
 */
bool BrowserConnection::isConnected()
{
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
}//BrowserConnection::ping()

/**
 * 
 */
void BrowserConnection::read()
{
}//BrowserConnection::read()

