#!/bin/sh

# Generates the QtDBus helper classes


if [ "x$1" == "x--adaptors" ]; then
	qdbusxml2cpp -N -c Mpris2RootAdaptor \
		     -a root_adaptor Root_Node.xml
	qdbusxml2cpp -N -c Mpris2PlayerAdaptor \
		     -a player_adaptor Player_Node.xml
	qdbusxml2cpp -N -c Mpris2TrackListAdaptor \
		     -a tracklist_adaptor TrackList_Node.xml
else
	qdbusxml2cpp -N -i "dbusabstractinterface.h" \
		     -c Mpris2RootProxy \
		     -p root_proxy Root_Node.xml
	qdbusxml2cpp -N -i "dbusabstractinterface.h" \
		     -c Mpris2PlayerProxy \
		     -p player_proxy Player_Node.xml
	qdbusxml2cpp -N -i "dbusabstractinterface.h" \
		     -c Mpris2TrackListProxy \
		     -p tracklist_proxy TrackList_Node.xml
	sed -i 's/: public QDBusAbstractInterface/: public DBusAbstractInterface/' \
		root_proxy.h player_proxy.h tracklist_proxy.h
	sed -i 's/: QDBusAbstractInterface/: DBusAbstractInterface/' \
		root_proxy.cpp player_proxy.cpp tracklist_proxy.cpp
fi
