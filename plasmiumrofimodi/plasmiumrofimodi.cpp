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
    qDBusRegisterMetaType<QStringMapMap>();

    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return 1;
    }

    QDBusInterface interface(SERVICE_NAME, "/", "", bus);
    if (interface.isValid()) {
        QString search;
        if (argc > 1) {
            search = QString(argv[1]);
        }
        QStringList items;

        // Get list of all open tabs
        QDBusReply<QStringMapMap> tabsReply = interface.call("getTabs");
        if (tabsReply.isValid()) {
            QStringMapMap windowMap = tabsReply.value();
            for (QStringMapMap::const_iterator it = windowMap.constBegin(), end = windowMap.constEnd(); it != end; ++it) {
                QStringMap tabMap = *it;
                QString current("tab: %1 (%2)");
                current = current.arg(tabMap["title"], tabMap["uri"]);
                // if execute statement matches, switch to this tab
                if (argc > 1 && current == search) {
                    bool ok;
                    interface.call("highlightTab", tabMap["windowId"].toInt(&ok, 10), tabMap["tabIndex"].toInt(&ok, 10));
                    return 0;
                }
                items << current;
            }
        } else {
            std::cerr << "Got an invalid reply from plasmium." << tabsReply.error().message().toStdString() << std::endl;
            return 1;
        }

        // Get list of all top sites
        QDBusReply<QStringMap> topsitesReply = interface.call("getTopSites");
        if (topsitesReply.isValid()) {
            QStringMap map = topsitesReply.value();
            for (QStringMap::const_iterator it = map.constBegin(), end = map.constEnd(); it != end; ++it) {
                QString current("%1 (%2)");
                current = current.arg(it.value(), it.key());
                // if execute statement matches, launch this top site
                if (argc > 1 && current == search) {
                    interface.call("newTab", it.key());
                    return 0;
                }
                items << current;
            }
        } else {
            std::cerr << "Got an invalid reply from plasmium." << std::endl;
            return 1;
        }

        // Run new tab if no hit was found
        if (argc > 1) {
            interface.call("newTab", search);
            return 0;
        }

        // If no execute statement was supplied, print available entries.
        for (QStringList::const_iterator it = items.constBegin(), end = items.constEnd(); it != end; ++it) {
            std::cout << (*it).toStdString() << std::endl;
        }
        return 0;
    }

    std::cerr << "Couldn't connect to chrome. Please make sure plasmium is installed and activated in Chrome." << std::endl;
    return 1;
}//main
