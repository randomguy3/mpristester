CONFIG += qt gui debug
QT += dbus

HEADERS += window.h \
           metadatamodel.h \
           mpris2/interfacetest.h \
           mpris2/rootinterfacetest.h \
           mpris2/roottestwidget.h \
           mpris2/testconsole.h

SOURCES += main.cpp \
           window.cpp \
           metadatamodel.cpp \
           mpris2/interfacetest.cpp \
           mpris2/rootinterfacetest.cpp \
           mpris2/roottestwidget.cpp \
           mpris2/testconsole.cpp

FORMS += ui/roottest.ui

OTHER_FILES += \
    dbus/2.0/TrackList_Node.xml \
    dbus/2.0/Root_Node.xml \
    dbus/2.0/qtdbus-annotations.patch \
    dbus/2.0/Player_Node.xml \
    dbus/1.0/org.freedesktop.MediaPlayer.tracklist.xml \
    dbus/1.0/org.freedesktop.MediaPlayer.root.xml \
    dbus/1.0/org.freedesktop.MediaPlayer.player.xml \
    README








