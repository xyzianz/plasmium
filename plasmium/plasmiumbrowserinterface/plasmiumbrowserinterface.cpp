#include <iostream>
#include <cstdio>
#include <signal.h>
#include <unistd.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QDateTime>
#include <QtNetwork>

#include "../common.h"
#include "plasmiumbrowserinterface.h"

PlasmiumBrowserInterface::PlasmiumBrowserInterface()
{
    m_logfile = new QFile(QDir::homePath().append("/.plasmium.log"));
    m_logfile->open(QIODevice::ReadWrite|QIODevice::Text);
    m_logfile->seek(m_logfile->size());
    this->log("Starting Plasmium Browser Interface.");
    
    m_stdinfile.open(stdin, QIODevice::ReadOnly);
    m_dataStreamIn = new QDataStream(&m_stdinfile);
    m_dataStreamIn->setByteOrder(QDataStream::LittleEndian);
    m_stdoutfile.open(stdout, QIODevice::ReadWrite);

    m_socket = new QLocalSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readSocketMessage()));
    QString socketfile = QStandardPaths::locate(QStandardPaths::RuntimeLocation, "plasmium.sock", QStandardPaths::LocateFile);
    m_socket->connectToServer(socketfile, QIODevice::ReadWrite);

    connect(this, SIGNAL(socketMessage(const QJsonDocument &)), this, SLOT(sendNativeMessage(const QJsonDocument &)));
    connect(this, SIGNAL(nativeMessage(const QJsonDocument &)), this, SLOT(sendSocketMessage(const QJsonDocument &)));

    m_stdinNotifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
    connect(m_stdinNotifier, SIGNAL(activated(int)), this, SLOT(readNativeMessage()));
}//PlasmiumBrowserInterface::PlasmiumBrowserInterface()

PlasmiumBrowserInterface::~PlasmiumBrowserInterface()
{
    this->log("Stopping Plasmium Browser Interface.");
    m_stdinfile.close();
    m_stdoutfile.close();
    m_logfile->close();
    delete m_logfile;
    delete m_stdinNotifier;
    delete m_dataStreamIn;
}//PlasmiumBrowserInterface::~PlasmiumBrowserInterface()

void PlasmiumBrowserInterface::log(const QString &string)
{
    QDateTime now(QDateTime(QDateTime::currentDateTimeUtc()));
    QString output = "[" + now.toString(Qt::ISODateWithMs) + "] " + string + "\n";
    m_logfile->seek(m_logfile->size());
    m_logfile->write(output.toUtf8());
    m_logfile->flush();
}//PlasmiumBrowserInterface::log(const QString&)

void PlasmiumBrowserInterface::sendNativeMessage(const QJsonDocument &message)
{
    QByteArray bytesToSend = message.toJson(QJsonDocument::Compact);

    // Calculate 32bit "chrome native byte order"
    // (which seems to imply LittleEndian) integer 
    // identifying the byte length of a message to send.
    quint32 length = bytesToSend.length();
    QByteArray size;
    QDataStream sizestream(&size, QIODevice::WriteOnly);
    sizestream.setByteOrder(QDataStream::LittleEndian);
    sizestream << length;

    // Log to debug file
    this->log(QString("Sending %1 %2 bytes").arg(QString(size.toHex())).arg(length));
    this->log(QString("Sending message: %1").arg(QString(bytesToSend)));

    // Write actual bytes to stdout
    m_stdoutfile.write(size.append(bytesToSend));
    m_stdoutfile.flush();
    
}//PlasmiumBrowserInterface::sendNativeMessage()

void PlasmiumBrowserInterface::readNativeMessage()
{
    // Read 4 bytes of data (in LittleEndian format) from DataStream on stdin into a unsigned 32-bit integer
    // representing the length of data to be read
    quint32 length;
    QByteArray array;
    *m_dataStreamIn >> length;
    
    // If there is data specified to be read, read it
    if (length > 0) {
        
        // Get ASCII HEX byte representation of data length variable for debugging
        QByteArray sizearray;
        QDataStream sizestream(&sizearray, QIODevice::WriteOnly);
        sizestream << length;

        // Log to file
        this->log(QString("Reading: %1 %2 bytes").arg(QString(sizearray.toHex())).arg(length));

        // Actually read data from stdin into a QJsonDocument, via a QByteArray
        char* s = new char[length];
        m_stdinfile.read(s, length);//>> input;//char* s;
        QByteArray b(s);
        if (static_cast<quint32>(b.length()) > length) {
            b.chop(b.length() - length);
        }
        delete s;
        QJsonParseError p;
        QJsonDocument message = QJsonDocument::fromJson(b, &p);

        // Some more debug logging
        this->log("Incoming message: " + message.toJson());

        emit(nativeMessage(message));
    }
}//PlasmiumBrowserInterface::readNativeMessage()

void PlasmiumBrowserInterface::readSocketMessage()
{
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_4_0);
    if (m_blockSize == 0) {
        if (m_socket->bytesAvailable() < (int)sizeof(quint32)) {
            return;
        }
        in >> m_blockSize;
    }

    if (m_socket->bytesAvailable() < m_blockSize) {
        return;
    }

    QByteArray data;
    in >> data;
    QJsonDocument message = QJsonDocument::fromBinaryData(data, QJsonDocument::BypassValidation);
    emit(socketMessage(message));
}//PlasmiumBrowserInterface::readSocketMessage()

void PlasmiumBrowserInterface::sendSocketMessage(const QJsonDocument &message)
{
    this->log("Sending message on socket: " + QString(message.toJson(QJsonDocument::Compact)));

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
    m_socket->write(data);
    m_socket->flush();
}//PlasmiumBrowserInterface::sendSocketMessage(const QJsonDocument &)

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    auto handler = [](int unixSignal)->void{
        Q_UNUSED(unixSignal)
        QCoreApplication::quit();
    };
    for (int unixSignal: {SIGQUIT, SIGINT, SIGTERM, SIGHUP}) {
        signal(unixSignal, handler);
    }

    PlasmiumBrowserInterface interface;
    interface.log("Testing Plasmium Browser Interface.");
    app.exec();
    interface.log("Initiating shutdown.");
    return 0;
}//main
