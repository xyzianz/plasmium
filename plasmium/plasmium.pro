QT -= gui
QT += dbus network

HEADERS += common.h plasmium.h plasmiumchrometab.h browserconnection.h
SOURCES += plasmium.cpp plasmiumchrometab.cpp browserconnection.cpp

target.path = /usr/bin
INSTALLS += target
