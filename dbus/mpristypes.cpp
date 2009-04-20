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
#include "mpristypes.h"

#include <QtDBus>

QDBusArgument &operator<<(QDBusArgument &argument, const Mpris::Version &version)
{
    argument.beginStructure();
    argument << version.major << version.minor;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Mpris::Version &version)
{
    argument.beginStructure();
    argument >> version.major >> version.minor;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const Mpris::Status &status)
{
    argument.beginStructure();
    argument << (qint32)status.play;
    argument << (qint32)status.random;
    argument << (qint32)status.trackRepeat;
    argument << (qint32)status.playlistRepeat;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Mpris::Status &status)
{
    qint32 play, random, trackRepeat, playlistRepeat;

    argument.beginStructure();
    argument >> play;
    argument >> random;
    argument >> trackRepeat;
    argument >> playlistRepeat;
    argument.endStructure();

    status.play = (Mpris::Status::PlayMode)play;
    status.random = (Mpris::Status::RandomMode)random;
    status.trackRepeat = (Mpris::Status::TrackRepeatMode)trackRepeat;
    status.playlistRepeat = (Mpris::Status::PlaylistRepeatMode)playlistRepeat;

    return argument;
}

void Mpris::registerTypes()
{
    qDBusRegisterMetaType<Mpris::Version>();
    qDBusRegisterMetaType<Mpris::Status>();
}

// vim: sw=4 sts=4 et tw=100
