#ifndef PLASMIUM_H
#define PLASMIUM_H

#include <QtCore/QObject>
#include <QSocketNotifier>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QtNetwork>

#include "plasmiumchrometab.h"
#include "browserconnection.h"
#include "common.h"

class BrowserConnection;

class Plasmium: public QObject
{
    Q_OBJECT
    QString m_logfilepath;
    QStringMap m_topsites;
    QStringMapMap m_tabs;
    QLocalServer *m_server;
    QList<BrowserConnection*> m_connections;
    QSocketNotifier *m_notify;
    QFile *m_in, *m_out, *m_logfile;
    QDataStream *m_dataStreamIn, *m_dataStreamOut;
    QTextStream *m_textStreamIn, *m_textStreamOut;

public:
    Plasmium();
    ~Plasmium();
    void init();

public slots:
    Q_SCRIPTABLE void refreshTabs();
    Q_SCRIPTABLE QStringMapMap getTabs();
    Q_SCRIPTABLE void listTopSites();
    Q_SCRIPTABLE QStringMap getTopSites();
    Q_SCRIPTABLE void muteAllTabs();
    Q_SCRIPTABLE void muteAllBackgroundTabs();
    Q_SCRIPTABLE void listAllAudibleWindows();
    Q_SCRIPTABLE void unmuteActiveTab();
    Q_SCRIPTABLE void unmuteAllTabs();
    Q_SCRIPTABLE void highlightTab(int windowId, int tabIndex);
    Q_SCRIPTABLE void highlightTab(const PlasmiumChromeTab &tab);
    Q_SCRIPTABLE void newTab(const QString &uri);
    void log(const QString &string);

signals:
    Q_SCRIPTABLE void listOfTopSites(QStringMap topsites);
    Q_SCRIPTABLE void listOfTabs(QStringMapMap tabs);

private Q_SLOTS:
    void sendMessageAsync(const QJsonDocument &message);
    void sendMessageSync(const QJsonDocument &message, QList<QJsonDocument> &response);
    void parseMessage(const QJsonDocument &message);
    void parseTabs(const QList<QJsonDocument> &tabs);
    void newConnection();
    void closeDisconnectedConnections();
};


#endif // PLASMIUM_H
