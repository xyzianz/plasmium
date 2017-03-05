#include <iostream>
#include <cstdio>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>
#include <QFile>

#include "../plasmium/common.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    qDBusRegisterMetaType<QStringMap>();

    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return 1;
    }

    QDBusInterface interface(SERVICE_NAME, "/", "", bus);
    if (interface.isValid()) {
        QDBusReply<QStringMap> reply = interface.call("getTopSites");
        QStringList sites;
        if (reply.isValid()) {
            QStringMap map = reply.value();
            for (QStringMap::const_iterator it = map.constBegin(), end = map.constEnd(); it != end; ++it) {
                QString current("%1 (%2)");
                current = current.arg(it.value(), it.key());
                if (argc > 1 && current == QString(argv[1])) {
                    interface.call("newTab", it.key());
                    return 0;
                }
                sites << current;
            }
            if (argc > 1) {
                interface.call("newTab", QString(argv[1]));
                return 0;
            }
            for (QStringList::const_iterator it = sites.constBegin(), end = sites.constEnd(); it != end; ++it) {
                std::cout << (*it).toStdString() << std::endl;
            }
            return 0;
        }
        std::cerr << "Got an invalid reply from plasmium." << std::endl;
        return 1;
    }

    std::cerr << "Couldn't connect to chrome. Please make sure plasmium is installed and activated in Chrome." << std::endl;
    return 1;
}//main
