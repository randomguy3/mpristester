#!/bin/sh

# Generates the QtDBus helper classes


if [ "x$1" == "x--adaptors" ]; then
	qdbusxml2cpp -N -i mpristypes.h \
		     -c Mpris1RootAdaptor \
		     -a root_adaptor org.freedesktop.MediaPlayer.root.xml
	qdbusxml2cpp -N -i mpristypes.h \
		     -c Mpris1PlayerAdaptor \
		     -a player_adaptor org.freedesktop.MediaPlayer.player.xml
	qdbusxml2cpp -N -i mpristypes.h \
		     -c Mpris1TrackListAdaptor \
		     -a tracklist_adaptor org.freedesktop.MediaPlayer.tracklist.xml
else
	qdbusxml2cpp -N -i mpristypes.h \
		     -c Mpris1RootProxy \
		     -p root_proxy org.freedesktop.MediaPlayer.root.xml
	qdbusxml2cpp -N -i mpristypes.h \
		     -c Mpris1PlayerProxy \
		     -p player_proxy org.freedesktop.MediaPlayer.player.xml
	qdbusxml2cpp -N -i mpristypes.h \
		     -c Mpris1TrackListProxy \
		     -p tracklist_proxy org.freedesktop.MediaPlayer.tracklist.xml
fi
