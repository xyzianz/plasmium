#include <iostream>
#include <cstdio>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>
#include <QFile>
#include <QDataStream>

#include "plasmium.h"
#include "common.h"


Plasmium::Plasmium()
{
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
    m_notify = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
    connect(m_notify, SIGNAL(activated(int)), this, SLOT(readNativeMessage()));

}//Plasmium::init()

QString Plasmium::matchTab(const QString &text)
{
    if (text == "test") {
        return "testing";
    } else {
        return "failing";
    }        
}//Plasmium::matchTab()

void Plasmium::switchTab(const QString &text)
{
//    *m_dataStreamOut << (quint32)text.length();
//    *m_textStreamOut << text;

    QByteArray sizearray;
    QDataStream sizestream(&sizearray, QIODevice::WriteOnly);
    sizestream.setByteOrder(QDataStream::LittleEndian);
    sizestream << (quint32)text.toUtf8().length();
    
    
    QFile debugFile("/home/aron/.plasmium.log");
    debugFile.open(QIODevice::ReadWrite|QIODevice::Text);
    debugFile.seek(debugFile.size());
    QTextStream debugStream(&debugFile);
    QDataStream debugDataStram(&debugFile);
    debugStream << endl;
    debugStream << "Sending:" << endl;
    debugStream << sizearray.toHex() << "    " << text.toUtf8().length() << endl;
    debugStream << text.toUtf8().toHex() << "    " << text << endl << endl;
//    debugFile.flush();
//    debugStream << endl << text << endl;
    debugFile.flush();
    debugFile.close();

    QFile o;
    o.open(stdout, QIODevice::WriteOnly);
    o.write(sizearray.append(text.toUtf8()));
//    o.write(.toHex());
//    QDataStream ds(&o);
//    ds << sizearray << text.toUtf8();
    o.flush();
    o.close();
    
}//Plasmium::switchTab()

void Plasmium::readNativeMessage()
{
    QFile debugFile("/home/aron/.plasmium.log");
    debugFile.open(QIODevice::ReadWrite|QIODevice::Text);
    debugFile.seek(debugFile.size());
    QTextStream debugStream(&debugFile);
    debugStream << endl << "Reading incoming..." << endl;
    debugFile.flush();

    QFile qtin;
    qtin.open(stdin, QIODevice::ReadOnly);
    QDataStream in(&qtin);
    QTextStream tin(&qtin);
    in.setByteOrder(QDataStream::LittleEndian);
    quint32 length;
    QByteArray array;
    in >> length;
    QByteArray sizearray;
    QDataStream sizestream(&sizearray, QIODevice::WriteOnly);
    sizestream << length;
    debugStream << endl << "Read length: " << sizearray.toHex() << " (" << length << ")" << endl;
    
    if (length > 0) {
        char* s = new char[length-1];
        qtin.read(s, length);//>> input;//char* s;
        QByteArray b(s);
        b.chop(1);
        delete s;
        debugStream << QString().fromUtf8(b);
        //in.readRawData(s, 4);

    debugStream << endl << "Incoming:" << endl;
//    debugStream << "s: " << s << endl;
    debugStream << "bytes: " << QString(b).toUtf8().toHex() << endl;
    debugStream << "text: " << QString(b) << endl;
    debugFile.flush();
    debugFile.close();
    }
}//Plasmium::readNativeMessage()

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    if (!QDBusConnection::sessionBus().isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return 1;
    }

    if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME)) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::sessionBus().lastError().message()));
        exit(1);
    }

    Plasmium plasmium;
    plasmium.init();
    QDBusConnection::sessionBus().registerObject("/", &plasmium, QDBusConnection::ExportAllSlots);

    app.exec();
    return 0;
}//main
