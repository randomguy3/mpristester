CONFIG += qt gui debug
QT += dbus

HEADERS += window.h \
           metadatamodel.h \
           mpris2/interfacetest.h \
           mpris2/rootinterfacetest.h \
           mpris2/roottestwidget.h \
           mpris2/playerinterfacetest.h \
           mpris2/playertestwidget.h \
           mpris2/testconsole.h

SOURCES += main.cpp \
           window.cpp \
           metadatamodel.cpp \
           mpris2/interfacetest.cpp \
           mpris2/rootinterfacetest.cpp \
           mpris2/roottestwidget.cpp \
           mpris2/playerinterfacetest.cpp \
           mpris2/playertestwidget.cpp \
           mpris2/testconsole.cpp

FORMS += ui/roottest.ui \
         ui/playertest.ui \
         ui/window.ui

OTHER_FILES += README

