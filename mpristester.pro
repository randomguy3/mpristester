CONFIG += qt \
    gui \
    debug
QT += dbus
HEADERS += window.h \
    metadatamodel.h \
    dbus/1.0/mpristypes.h \
    dbus/1.0/root_proxy.h \
    dbus/1.0/player_proxy.h \
    dbus/1.0/tracklist_proxy.h \
    dbus/2.0/root_proxy.h \
    dbus/2.0/player_proxy.h \
    dbus/2.0/tracklist_proxy.h \
    mpris2servicewatcher.h \
    mpris2service.h \
    dbus/2.0/dbusabstractinterface.h \
    mpris2tester.h \
    mpris2rootinterfacetester.h \
    dbusinterfacetester.h \
    propertyerror.h \
    methoderror.h \
    mpris2playerinterfacetester.h
SOURCES += main.cpp \
    window.cpp \
    metadatamodel.cpp \
    dbus/1.0/mpristypes.cpp \
    dbus/1.0/root_proxy.cpp \
    dbus/1.0/player_proxy.cpp \
    dbus/1.0/tracklist_proxy.cpp \
    dbus/2.0/root_proxy.cpp \
    dbus/2.0/player_proxy.cpp \
    dbus/2.0/tracklist_proxy.cpp \
    mpris2servicewatcher.cpp \
    mpris2service.cpp \
    dbus/2.0/dbusabstractinterface.cpp \
    mpris2tester.cpp \
    mpris2rootinterfacetester.cpp \
    dbusinterfacetester.cpp \
    mpris2playerinterfacetester.cpp
FORMS += ui/window.ui
