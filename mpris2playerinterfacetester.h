/*
 * Copyright 2010  Alex Merry <alex.merry@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MPRIS2PLAYERINTERFACETESTER_H
#define MPRIS2PLAYERINTERFACETESTER_H

#include "dbusinterfacetester.h"
#include <QVariantMap>
#include <QStringList>
class Mpris2PlayerProxy;

class Mpris2PlayerInterfaceTester : public DBusInterfaceTester
{
Q_OBJECT
public:
    enum ControlMode
    {
        CanControl,
        CannotControl,
        UnknownControlMode
    };

    explicit Mpris2PlayerInterfaceTester( Mpris2PlayerProxy * interface,
                                          QObject *parent = 0 );
    QVariantMap testProperties( bool* allOk = 0 );

signals:

public slots:
    void Next();
    void Previous();
    void Pause();
    void PlayPause();
    void Stop();
    void Play();
    void Seek( qint64 offset );
    void SetPosition( const QString& trackId, qint64 position );
    void OpenUri( const QString& uri );

private slots:
    void dbusPropertyChanged( const QString& propertyName, const QVariant& value );
    void dbusPropertyInvalidated( const QString& propertyName );

private:
    bool checkProperty( const QString& propertyName,
                        const QVariant& propertyValue,
                        bool fromUpdatedSignal = false );
    bool checkMetadataProperty( const QVariantMap& metadata );

    Mpris2PlayerProxy * m_interface;
    QStringList m_playbackStatusValues;
    QStringList m_loopStatusValues;
    ControlMode m_controlMode;
    quint64 m_length;
};

#endif // MPRIS2PLAYERINTERFACETESTER_H
