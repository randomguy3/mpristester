CONFIG += qt gui debug
QT += dbus

HEADERS += window.h \
           metadatamodel.h \
           dbus/1.0/mpristypes.h \
           dbus/1.0/root_proxy.h \
           dbus/1.0/player_proxy.h \
           dbus/1.0/tracklist_proxy.h

SOURCES += main.cpp \
           window.cpp \
           metadatamodel.cpp \
           dbus/1.0/mpristypes.cpp \
           dbus/1.0/root_proxy.cpp \
           dbus/1.0/player_proxy.cpp \
           dbus/1.0/tracklist_proxy.cpp

FORMS += ui/window.ui

