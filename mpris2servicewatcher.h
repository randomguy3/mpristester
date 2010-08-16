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

#ifndef MPRIS2SERVICEWATCHER_H
#define MPRIS2SERVICEWATCHER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>

class QDBusConnection;
class Mpris2Service;

/**
 * Watches for MPRIS services
 *
 * Note that all service names are without the "org.mpris.MediaPlayer2" prefix.
 */
class Mpris2ServiceWatcher : public QObject
{
Q_OBJECT
public:
    /**
     * The D-Bus service name prefix for MPRIS2
     *
     * @return "org.mpris.MediaPlayer2."
     */
    static QString dbusServicePrefix();

    /**
     * Construct a new watcher on the session bus
     */
    explicit Mpris2ServiceWatcher( QObject* parent = 0 );
    /**
     * Construct a new watcher on a specific bus
     */
    explicit Mpris2ServiceWatcher( const QDBusConnection& connection,
                                   QObject* parent = 0 );

    /**
     * List all the currently-known service names
     */
    QStringList registeredMpris2ServiceNames();
    /**
     * List all the currently-known services
     */
    QList<Mpris2Service*> registeredMpris2Services();
    /**
     * List all the MPRIS services that D-Bus knows how to start
     *
     * Note that there may be some overlap with the registered
     * names.
     *
     * @see startService()
     */
    QStringList activatableMpris2ServiceNames();

    /**
     * Get the service for a given service name.
     *
     * @param serviceName  The service name, eg: "amarok"
     */
    Mpris2Service * service( const QString& serviceName );
    /**
     * Ask D-Bus to start the application that provides the given service name
     *
     * You should listen to mpris2ServiceRegistered() to find out if and
     * when it finishes starting.
     *
     * @see activatableMpris2ServiceNames()
     */
    void startService( const QString& serviceName );

signals:
    /**
     * Notifies when a new MPRIS service was registered on the bus
     */
    void mpris2ServiceRegistered( Mpris2Service * service );
    /**
     * Notifies when an MPRIS service was unregistered from the bus
     *
     * This usually happens when an application quits.
     */
    void mpris2ServiceUnregistered( Mpris2Service * service );

private slots:
    void dbusServiceOwnerChanged( const QString& name,
                                  const QString& oldOwner,
                                  const QString& newOwner );

private:
    QStringList filterServiceNames( const QStringList& serviceNames ) const;
    void init();

    QDBusConnection m_connection;
    QHash<QString,Mpris2Service *> m_services;
};

#endif // MPRIS2SERVICEWATCHER_H
