CONFIG += qt gui debug
QT += dbus

HEADERS += window.h \
           metadatamodel.h \
           dbus/mpristypes.h \
           dbus/mprisroot.h \
           dbus/mprisplayer.h \
           dbus/mpristracklist.h

SOURCES += main.cpp \
           window.cpp \
           metadatamodel.cpp \
           dbus/mpristypes.cpp \
           dbus/mprisroot.cpp \
           dbus/mprisplayer.cpp \
           dbus/mpristracklist.cpp

FORMS += ui/window.ui

