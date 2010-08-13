/*
 * Copyright 2008, 2009  Alex Merry <alex.merry@kdemail.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef MPRISTYPES_H
#define MPRISTYPES_H

#include <QDBusArgument>
#include <QFlags>

namespace Mpris
{
    /**
     * The bit values for the capabilities flags
     */
    enum Cap {
        NO_CAPS               = 0,
        CAN_GO_NEXT           = 1 << 0,
        CAN_GO_PREV           = 1 << 1,
        CAN_PAUSE             = 1 << 2,
        CAN_PLAY              = 1 << 3,
        CAN_SEEK              = 1 << 4,
        CAN_PROVIDE_METADATA  = 1 << 5,
        CAN_HAS_TRACKLIST     = 1 << 6,
        ALL_KNOWN_CAPS        = (1 << 7) - 1
    };
    Q_DECLARE_FLAGS(Caps, Cap)


    struct Version
    {
        quint16 major;
        quint16 minor;
    };


    struct Status
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

        Status(PlayMode _play = Stopped,
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

    /**
     * Registers the D-Bus types with the Qt metadata system.
     *
     * This MUST be called before using either Status or Version in any
     * D-Bus calls (including making calls that return those types).
     */
    void registerTypes();
} // namespace Mpris

Q_DECLARE_OPERATORS_FOR_FLAGS(Mpris::Caps)

Q_DECLARE_METATYPE(Mpris::Version)

Q_DECLARE_METATYPE(Mpris::Status)

// Marshall the Mpris::Version data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const Mpris::Version &version);
// Retrieve the Mpris::Version data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, Mpris::Version &version);

// Marshall the Mpris::Status data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const Mpris::Status &status);
// Retrieve the Mpris::Status data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, Mpris::Status &status);

#endif // MPRISTYPES_H
