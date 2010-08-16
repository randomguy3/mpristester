/*
 * Copyright 2010  Alex Merry <alex.merry@kdemail.net>
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

#ifndef MPRIS2SERVICE_H
#define MPRIS2SERVICE_H

#include <QObject>
#include <QString>

class QDBusConnection;
class Mpris2RootProxy;
class Mpris2PlayerProxy;
class Mpris2TrackListProxy;

/**
 * Represents an MPRIS provider on the D-Bus
 */
class Mpris2Service : public QObject
{
Q_OBJECT
public:
    explicit Mpris2Service( const QString& serviceName,
                            const QDBusConnection& connection,
                            QObject* parent = 0 );
    virtual ~Mpris2Service();

    /**
     * Get a proxy for the root interface
     */
    Mpris2RootProxy *rootInterface();
    /**
     * Get a proxy for the player interface
     */
    Mpris2PlayerProxy *playerInterface();
    /**
     * Get a proxy for the track list interface
     *
     * Note that this proxy may not work if the MPRIS provider
     * does not implement the org.mpris.MediaPlayer2.TrackList
     * interface.  See Mpris2RootProxy::hasTrackList().
     */
    Mpris2TrackListProxy *trackListInterface();

    /**
     * The service name.
     *
     * eg: "amarok" or "juk.1434"
     */
    QString serviceName() const;
    /**
     * The D-Bus service name.
     *
     * eg: "org.mpris.MediaPlayer2.amarok" or
     * "org.mpris.MediaPlayer2.juk.1434"
     */
    QString dbusServiceName() const;

private:
    QString m_serviceName;
    QDBusConnection m_connection;
    Mpris2RootProxy* m_rootInterface;
    Mpris2PlayerProxy* m_playerInterface;
    Mpris2TrackListProxy* m_trackListInterface;
};

#endif // MPRIS2SERVICE_H
