QT -= gui
QT += dbus

HEADERS += common.h plasmium.h plasmiumchrometab.h
SOURCES += plasmium.cpp plasmiumchrometab.cpp

target.path = /usr/bin
INSTALLS += target
