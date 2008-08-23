#ifndef MPRISDEFS_H
#define MPRISDEFS_H

#include <QDBusArgument>

enum DBusCaps {
    NO_CAPS               = 0,
    CAN_GO_NEXT           = 1 << 0,
    CAN_GO_PREV           = 1 << 1,
    CAN_PAUSE             = 1 << 2,
    CAN_PLAY              = 1 << 3,
    CAN_SEEK              = 1 << 4,
    CAN_PROVIDE_METADATA  = 1 << 5,
    CAN_HAS_TRACKLIST     = 1 << 6,
    UNKNOWN_CAP           = 1 << 7
};


struct DBusVersion
{
    quint16 major;
    quint16 minor;
};

Q_DECLARE_METATYPE(DBusVersion)

// Marshall the DBusVersion data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const DBusVersion &version);
// Retrieve the DBusVersion data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, DBusVersion &version);


enum DBusStatusPlay {
    Playing = 0,
    Paused = 1,
    Stopped = 2
};

enum DBusStatusRandom {
    Linear = 0,
    Random = 1
};

enum DBusStatusTrackRepeat {
    GoToNext = 0,
    RepeatCurrent = 1
};

enum DBusStatusPlaylistRepeat {
    StopWhenFinished = 0,
    PlayForever = 1
};

struct DBusStatus
{
    DBusStatus(DBusStatusPlay _play = Stopped,
               DBusStatusRandom _random = Linear,
               DBusStatusTrackRepeat _trackRepeat = GoToNext,
               DBusStatusPlaylistRepeat _playlistRepeat = StopWhenFinished)
        : play(_play),
          random(_random),
          trackRepeat(_trackRepeat),
          playlistRepeat(_playlistRepeat)
    {
    }
    DBusStatusPlay           play;
    DBusStatusRandom         random;
    DBusStatusTrackRepeat    trackRepeat;
    DBusStatusPlaylistRepeat playlistRepeat;
};

Q_DECLARE_METATYPE(DBusStatus)

// Marshall the DBusStatus data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const DBusStatus &status);
// Retrieve the DBusStatus data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, DBusStatus &status);

#endif // MPRISDEFS_H
