#ifndef PLASMIUMBROWSERINTERFACE_H
#define PLASMIUMBROWSERINTERFACE_H

#include <QtCore/QObject>
#include <QSocketNotifier>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QJsonDocument>

#include "../common.h"

class QLocalSocket;

class PlasmiumBrowserInterface : public QObject
{
    Q_OBJECT

private:
    QFile m_stdinfile, m_stdoutfile, *m_logfile;
    QSocketNotifier *m_stdinNotifier;
    QLocalSocket *m_socket;
    quint32 m_blockSize;
    QDataStream *m_dataStreamIn;
    QTextStream *m_logstream;

public:
    PlasmiumBrowserInterface();
    ~PlasmiumBrowserInterface();
    void log(const QString &string);
    static void quitHandler(int sig);

Q_SIGNALS:
    Q_SCRIPTABLE void nativeMessage(const QJsonDocument &message);
    Q_SCRIPTABLE void socketMessage(const QJsonDocument &message);

private Q_SLOTS:
    void readNativeMessage();
    void sendNativeMessage(const QJsonDocument &message);
    void readSocketMessage();
    void sendSocketMessage(const QJsonDocument &message);
};

#endif // PLASMIUMBROWSERINTERFACE_H
