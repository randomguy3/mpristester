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

#include "mpris2servicewatcher.h"
#include "mpris2service.h"
#include <QtDBus/QtDBus>
#include <QHash>

Q_GLOBAL_STATIC_WITH_ARGS(QString, servicePrefix, (QLatin1String("org.mpris.MediaPlayer2.")))

static QString Mpris2ServiceWatcher::dbusServicePrefix()
{
    return *servicePrefix();
}

Mpris2ServiceWatcher::Mpris2ServiceWatcher(QObject *parent)
    : QObject( parent )
    , m_connection( QDBusConnection::sessionBus() )
{
    init();
}
Mpris2ServiceWatcher::Mpris2ServiceWatcher( const QDBusConnection& connection,
                                            QObject* parent )
    : QObject( parent )
    , m_connection( connection )
{
    init();
}

void Mpris2ServiceWatcher::init()
{
    connect(m_connection.interface(), SIGNAL(serviceOwnerChanged(QString,QString,QString)),
            this, SLOT(dbusServiceOwnerChanged(QString,QString,QString)));
    foreach ( QString name, m_connection->interface()->registeredServiceNames() )
    {
        if ( name.startsWith( *servicePrefix() ) )
        {
            QString shortName = name.mid( servicePrefix()->size() );
            service = new Mpris2Service( shortName, this );
            m_services.insert( shortName, service );
        }
    }
}

QList<Mpris2Service*> Mpris2ServiceWatcher::registeredMpris2Services()
{
    return m_services.values();
}

QStringList Mpris2ServiceWatcher::registeredMpris2ServiceNames()
{
    return m_services.keys();
}

QStringList Mpris2ServiceWatcher::activatableMpris2ServiceNames()
{
    QStringList names = m_connection->interface()->call(
            QDBus::Block,
            QLatin1String( "ListActivatableNames" ));

    return filterServiceNames( names );
}

Mpris2Service * Mpris2ServiceWatcher::service( const QString& sName )
{
    return m_services.value( sName );
}

void Mpris2ServiceWatcher::startService( const QString& sName )
{
    m_connection.interface()->startService( *servicePrefix() + sName );
}

void Mpris2ServiceWatcher::dbusServiceOwnerChanged( const QString& name,
                         const QString& oldOwner,
                         const QString& newOwner )
{
    if ( name.startsWith( *servicePrefix() ) )
    {
        QString shortName = name.mid( servicePrefix()->size() );
        Mpris2Service * service = m_services.value( sName );
        if ( service )
        {
            emit mpris2ServiceUnregistered( service );
            service->deleteLater();
            m_services.remove( shortName );
        }
        if ( !newOwner.isEmpty() )
        {
            service = new Mpris2Service( shortName, this );
            m_services.insert( shortName, service );
            emit mpris2ServiceRegistered( service );
        }
    }
}

QStringList Mpris2ServiceWatcher::filterServiceNames(const QStringList& serviceNames)
{
    QStringList filtered;
    foreach ( QString name, serviceNames )
    {
        if ( name.startsWith( *servicePrefix() ) )
        {
            filtered << name;
        }
    }
    return filtered;
}

