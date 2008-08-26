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


struct DBusStatus
{
    enum PlayMode {
        Playing = 0,
        Paused = 1,
        Stopped = 2
    };

    enum RandomMode {
        Linear = 0,
        Random = 1
    };

    enum TrackRepeatMode {
        GoToNext = 0,
        RepeatCurrent = 1
    };

    enum PlaylistRepeatMode {
        StopWhenFinished = 0,
        PlayForever = 1
    };

    DBusStatus(PlayMode _play = Stopped,
                    RandomMode _random = Linear,
                    TrackRepeatMode _trackRepeat = GoToNext,
                    PlaylistRepeatMode _playlistRepeat = StopWhenFinished)
        : play(_play),
          random(_random),
          trackRepeat(_trackRepeat),
          playlistRepeat(_playlistRepeat)
    {
    }
    PlayMode           play;
    RandomMode         random;
    TrackRepeatMode    trackRepeat;
    PlaylistRepeatMode playlistRepeat;
};

Q_DECLARE_METATYPE(DBusStatus)

// Marshall the DBusStatus data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const DBusStatus &status);
// Retrieve the DBusStatus data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, DBusStatus &status);

#endif // MPRISDEFS_H
