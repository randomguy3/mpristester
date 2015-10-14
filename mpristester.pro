CONFIG += qt gui debug widgets
QT += dbus

include(mpris2/mpris2.pri)

HEADERS += window.h \
           metadatamodel.h

SOURCES += main.cpp \
           window.cpp \
           metadatamodel.cpp

FORMS += ui/roottest.ui \
         ui/playertest.ui \
         ui/window.ui

OTHER_FILES += README
