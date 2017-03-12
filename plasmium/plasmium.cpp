#include <iostream>
#include <cstdio>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>
#include <QFile>
#include <QDataStream>
#include <QDir>

#include "plasmium.h"
#include "common.h"

Plasmium::Plasmium()
{
    m_logfile = new QFile(QDir::homePath().append("/.plasmium.log"));
    m_logfile->open(QIODevice::ReadWrite|QIODevice::Text);
    m_logfile->seek(m_logfile->size());
    this->log("Starting Plasmium.");

    m_server = new QLocalServer(this);

    QString socketfile = QStandardPaths::locate(QStandardPaths::RuntimeLocation, "plasmium.sock", QStandardPaths::LocateFile);
    m_server->listen(socketfile);
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    m_in = new QFile();
    m_in->open(stdin, QIODevice::ReadOnly);
    m_dataStreamIn = new QDataStream(m_in);
    m_textStreamIn = new QTextStream(m_in);

    m_out = new QFile();
    m_out->open(stdout, QIODevice::ReadWrite);
    m_dataStreamOut = new QDataStream(m_out);
    m_textStreamOut = new QTextStream(m_out);
}//Plasmium::Plasmium()

Plasmium::~Plasmium()
{
    m_logfile->close();
    delete m_logfile;

    delete m_server;
    delete m_dataStreamIn;
    delete m_textStreamIn;
    m_in->close();
    delete m_in;

    delete m_dataStreamOut;
    delete m_textStreamOut;
    m_out->close();
    delete m_out;

    if (m_notify) {
        delete m_notify;
    }
}//Plasmium::~Plasmium()

void Plasmium::init()
{
    this->log("Initializing Plasmium Core");

    m_notify = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
    connect(m_notify, SIGNAL(activated(int)), this, SLOT(readNativeMessage()));

}//Plasmium::init()

void Plasmium:log(const QString &string)
{
    QDateTime now(QDateTime(QDateTime::currentDateTimeUtc()));
    QString output = "[" + now.toString(Qt::ISODateWithMs) + "] " + string + "\n";
    m_logfile->seek(m_logfile->size());
    m_logfile->write(output.toUtf8());
    m_logfile->flush();
}//Plasmium::log(const QString&)

void Plasmium::newConnection()
{
    while (m_server->hasPendingConnections()) {
        QLocalSocket *connection = m_server->nextPendingConnection();
        BrowserConnection *browserConnection = new BrowserConnection(connection, m_server, this);
        m_connections << browserConnection;
        connect(browserConnection, SIGNAL(quit()), this, SLOT(closeDisconnectedConnections()));
        connect(browserConnection, SIGNAL(message(const QJsonDocument&)), this, SLOT(incomingMessage(const QJsonDocument&)));
    }
}//Plasmium::newConnection()

void Plasmium::closeDisconnectedConnections()
{
}//Plasmium::closeDisconnectedConnections()

void Plasmium::refreshTabs()
{
    QJsonObject message
    {
        {"command", "list all tabs"}
    };
    this->sendNativeMessage(QJsonDocument(message));
}//Plasmium::refreshTabs()

void Plasmium::listTopSites()
{
    QJsonObject message
    {
        {"command", "list top sites"}
    };
    this->sendNativeMessage(QJsonDocument(message));
}//Plasmium::listTopSites()

QStringMap Plasmium::getTopSites()
{
    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    connect(this, SIGNAL(listOfTopSites(QStringMap)), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start(1000);
    this->listTopSites();
    loop.exec();
    return m_topsites;
}//Plasmium::getTopSites();

QStringMapMap Plasmium::getTabs()
{
    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    connect(this, SIGNAL(listOfTabs(QStringMapMap)), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start(1000);
    this->refreshTabs();
    loop.exec();
    return m_tabs;
}//Plasmium::getTabs();

void Plasmium::listAllAudibleWindows()
{
    QJsonObject message
    {
        {"command", "list all audible windows"}
    };
    this->sendNativeMessage(QJsonDocument(message));
}//Plasmium::listAllAudibleWindows()

void Plasmium::muteAllTabs()
{
    QJsonObject message
    {
        {"command", "mute all tabs"}
    };
    this->sendNativeMessage(QJsonDocument(message));
}//Plasmium::muteAllTabs()

void Plasmium::muteAllBackgroundTabs()
{
    QJsonObject message
    {
        {"command", "mute all background tabs"}
    };
    this->sendNativeMessage(QJsonDocument(message));
}//Plasmium::muteAllBackgroundTabs()

void Plasmium::unmuteActiveTab()
{
    QJsonObject message
    {
        {"command", "unmute active tab"}
    };
    this->sendNativeMessage(QJsonDocument(message));
}//Plasmium::unmuteActiveTab()

void Plasmium::unmuteAllTabs()
{
    QJsonObject message
    {
        {"command", "unmute all tabs"}
    };
    this->sendNativeMessage(QJsonDocument(message));
}//Plasmium::unmuteAllTabs()


void Plasmium::highlightTab(int windowId, int tabIndex)
{
    QJsonObject message
    {
        {"command", "highlight tab"},
        {"windowId", windowId},
        {"tabIndex", tabIndex}
    };
    this->sendNativeMessage(QJsonDocument(message));
}//Plasmium::highlightTab()

void Plasmium::highlightTab(const PlasmiumChromeTab &tab)
{
    this->highlightTab(tab.getWindowId(), tab.getTabIndex());
}//Plasmium::highlightTab()

void Plasmium::newTab(const QString &uri)
{
    QJsonObject message
    {
        {"command", "new active tab"},
        {"uri", uri}
    };
    this->sendNativeMessage(QJsonDocument(message));
}//Plasmium::newTab()

void Plasmium::sendMessageAsync(const QJsonDocument &message)
{
    QString command = message.object().value("command").toString("No command found!");
    this.log("Sending message: " + command);

    // for all connections registered:
    //      send;
    
}//Plasmium::sendMessageAsync(const QJsonDocument &)

void Plasmium::sendMessageSync(const QJsonDocument &message, QJsonDocument &response)
{
    QString command = message.object().value("command").toString("No command found!");
    this.log("Sending message: " + command);

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    // create responseMessageList
    // for each registered connection
    //     connect syncResponse and syncAbort
    //     send on connection
    timer.start(1000);
    // unconnect all signals
    // destroy responseMessageList
    loop.exec();
    // parse all responses caught and save to response object
    
}//Plasmium::sendMessageSync(const QJsonDocument &, QJsonDocument &)

constexpr unsigned int getCommandCase(const QString &command)
{
    QString[] list = {
        "list of top sites",
        "list of tabs",
        "ping",
        "pong"
    };
    
    int i = 0;
    for (QString &item: list) {
        if (item == command) {
            return i;
        }
    }
    return -1;
}//getCommandCase(const QString &)

void Plasmium::parseMessage(const QJsonDocument &message)
{
    QString command = message.object().value("command").toString("No command found!");
    this.log("Parsing incoming message: " + command);
    switch (getCommandCase(command)) {
        case getCommandCase("list of top sites"):
            QJsonArray topsites = message.object().value("sites").toArray();
            this->log("Got a list of " + topsites.size() + " top sites.");
            m_topsites = QStringMap();
            for (QJsonArray::const_iterator iter = topsites .constBegin(), end = topsites.constEnd(); iter < end; ++iter) {
                m_topsites.insert((*iter).toObject().value("uri").toString(), (*iter).toObject().value("title").toString());
            }
            emit listOfTopSites(m_topsites);
            break;//list of top sites

        case getCommandCase("list of tabs"):
            QJsonObject windows = message.object().value("list").toObject();
            m_tabs= QStringMapMap();
            this->log("Got a list of " + topsites.size() + " tabs.");
            for (QJsonObject::const_iterator iter = windows.constBegin(), end = windows.constEnd(); iter != end; ++iter) {
                QJsonArray tabs = (*iter).toObject().value("windowInfo").toObject().value("tabs").toArray();
                for (QJsonArray::const_iterator tabIter = tabs.constBegin(), tabEnd = tabs.constEnd(); tabIter < tabEnd; ++tabIter) {
                    QStringMap values;
                    values.insert("title", (*tabIter).toObject().value("title").toString());
                    values.insert("uri", (*tabIter).toObject().value("url").toString());
                    values.insert("favicon", (*tabIter).toObject().value("favIconUrl").toString());
                    values.insert("windowId", QString("%1").arg((*tabIter).toObject().value("windowId").toInt()));
                    values.insert("tabIndex", QString("%1").arg((*tabIter).toObject().value("index").toInt()));
                    m_tabs.insert((*tabIter).toObject().value("url").toString(), values);
                }
            }
            emit listOfTabs(m_tabs);
            break;//list of tabs
    }//switch
}//Plasmium::parseMessage(const QJsonDocument &)

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    qDBusRegisterMetaType<QStringMap>();
    qDBusRegisterMetaType<QStringMapMap>();

    if (!QDBusConnection::sessionBus().isConnected()) {
        // log
        return 1;
    }

    if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME)) {
        //log
        exit(1);
    }

    Plasmium plasmium;
    plasmium.init();
    QDBusConnection::sessionBus().registerObject("/", &plasmium, QDBusConnection::ExportScriptableSlots|QDBusConnection::ExportScriptableSignals);

    app.exec();
    return 0;
}//main
