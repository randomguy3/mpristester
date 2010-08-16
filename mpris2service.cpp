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

#include "mpris2service.h"
#include "mpris2servicewatcher.h"
#include "dbus/2.0/root_proxy.h"
#include "dbus/2.0/player_proxy.h"
#include "dbus/2.0/tracklist_proxy.h"
#include <QtDBus/QtDBus>

Q_GLOBAL_STATIC_WITH_ARGS(QString, objectPath, (QLatin1String("/org/mpris/MediaPlayer2")))


Mpris2Service::Mpris2Service( const QString& serviceName,
                              const QDBusConnection& connection,
                              QObject *parent )
    : QObject( parent )
    , m_serviceName( serviceName )
    , m_connection( connection )
{
    setObjectName( serviceName );
}

Mpris2Service::~Mpris2Service()
{
}

Mpris2RootProxy* Mpris2Service::rootInterface()
{
    if ( !m_rootInterface )
    {
        m_rootInterface = new Mpris2RootProxy( dbusServiceName(), objectPath, m_connection, this );
    }
    return m_rootInterface;
}

Mpris2PlayerProxy* Mpris2Service::playerInterface()
{
    if ( !m_playerInterface )
    {
        m_playerInterface = new Mpris2PlayerProxy( dbusServiceName(), objectPath, m_connection, this );
    }
    return m_playerInterface;
}

Mpris2TrackListProxy* Mpris2Service::trackListInterface()
{
    if ( !m_trackListInterface )
    {
        m_trackListInterface = new Mpris2TrackListProxy( dbusServiceName(), objectPath, m_connection, this );
    }
    return m_trackListInterface;
}

QString Mpris2Service::serviceName() const
{
    return m_serviceName;
}

QString Mpris2Service::dbusServiceName() const
{
    return Mpris2ServiceWatcher::dbusServicePrefix() + m_serviceName;
}
