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

#include "mpris2playerinterfacetester.h"
#include "dbus/2.0/player_proxy.h"

Mpris2PlayerInterfaceTester::Mpris2PlayerInterfaceTester(
        Mpris2PlayerProxy * interface,
        QObject *parent)
    : DBusInterfaceTester( interface, parent )
    , m_interface( interface )
    , m_controlMode( UnknownControlMode )
    , m_length( -1 )
{
    m_playbackStatusValues << "Playing" << "Paused" << "Stopped";
    m_loopStatusValues << "None" << "Track" << "Playlist";
    connect( interface, SIGNAL(propertyChanged(QString,QVariant)),
             this, SLOT(dbusPropertyChanged(QString,QVariant)) );
    connect( interface, SIGNAL(propertyInvalidated(QString)),
             this, SLOT(dbusPropertyInvalidated(QString)) );
}

bool Mpris2PlayerInterfaceTester::checkProperty( const QString& propertyName,
                                                 const QVariant& propertyValue,
                                                 bool fromUpdatedSignal )
{
    if ( propertyName == QLatin1String( "CanControl" ) )
    {
        if ( fromUpdatedSignal )
        {
            emit unexpectedPropertyChangedSignal( propertyName );
            return false;
        }
        if ( propertyValue.isValid() )
        {
            m_controlMode = propertyValue.toBool() ? CanControl : CannotControl;
            return true;
        }
        return false;
    }
    else if ( propertyName == QLatin1String( "CanGoNext" ) ||
              propertyName == QLatin1String( "CanGoPrevious" ) ||
              propertyName == QLatin1String( "CanPlay" ) ||
              propertyName == QLatin1String( "CanPause" ) ||
              propertyName == QLatin1String( "CanSeek" ) )
    {
        if ( propertyValue.type() == QVariant::Bool )
        {
            if ( m_controlMode == CanControl && propertyValue.toBool() == true )
            {
                emit propertyError( PropertyError(
                        PropertyError::BadPropertyValue,
                        propertyName,
                        propertyValue,
                        propertyName + " must be false when CanControl is false" ) );
                return false;
            }
            return true;
        }
        else if ( fromUpdatedSignal )
        {
            emitUpdatedPropertyTypeError( propertyName, propertyValue, "bool", "b" );
            return false;
        }
        return false;
    }
    else if ( propertyName == QLatin1String( "PlaybackStatus" ) )
    {
        if ( propertyValue.type() == QVariant::String )
        {
            return checkStringEnumProperty( propertyName,
                                            m_playbackStatusValues,
                                            propertyValue.toString() );
        }
        else if ( fromUpdatedSignal )
        {
            emitUpdatedPropertyTypeError( propertyName, propertyValue, "string", "s" );
            return false;
        }
        else
            return false;
    }
    else if ( propertyName == QLatin1String( "LoopStatus" ) )
    {
        if ( propertyValue.type() == QVariant::String )
        {
            return checkStringEnumProperty( propertyName,
                                            m_loopStatusValues,
                                            propertyValue.toString() );
        }
        else if ( fromUpdatedSignal )
        {
            emitUpdatedPropertyTypeError( propertyName, propertyValue, "string", "s" );
            return false;
        }
        // optional
        return true;
    }
    else if ( propertyName == QLatin1String( "Shuffle" ) )
    {
        if ( fromUpdatedSignal && propertyValue.type() != QVariant::Bool )
        {
            emitUpdatedPropertyTypeError( propertyName, propertyValue, "bool", "b" );
            return false;
        }
        // optional
        return true;
    }
    else if ( propertyName == QLatin1String( "MinimumRate" ) )
    {
        if ( propertyValue.type() == QVariant::Double )
        {
            if ( propertyValue.toDouble() > 1.0 )
            {
                emit propertyError( PropertyError(
                        PropertyError::BadPropertyValue,
                        propertyName,
                        propertyValue,
                        propertyName + " must not be greater than 1.0" ) );
                return false;
            }
            return true;
        }
        else if ( fromUpdatedSignal )
        {
            emitUpdatedPropertyTypeError( propertyName, propertyValue, "double", "d" );
            return false;
        }
        return false;
    }
    else if ( propertyName == QLatin1String( "MaximumRate" ) )
    {
        if ( propertyValue.type() == QVariant::Double )
        {
            if ( propertyValue.toDouble() < 1.0 )
            {
                emit propertyError( PropertyError(
                        PropertyError::BadPropertyValue,
                        propertyName,
                        propertyValue,
                        propertyName + " must not be less than 1.0" ) );
                return false;
            }
            return true;
        }
        else if ( fromUpdatedSignal )
        {
            emitUpdatedPropertyTypeError( propertyName, propertyValue, "double", "d" );
            return false;
        }
        return false;
    }
    else if ( propertyName == QLatin1String( "Rate" ) )
    {
        if ( propertyValue.type() == QVariant::Double )
        {
            // FIXME: warn if Rate is not between MinimumRate and MaximumRate?
            return true;
        }
        else if ( fromUpdatedSignal )
        {
            emitUpdatedPropertyTypeError( propertyName, propertyValue, "double", "d" );
            return false;
        }
        return false;
    }
    else if ( propertyName == QLatin1String( "Volume" ) )
    {
        if ( propertyValue.type() == QVariant::Double )
        {
            if ( propertyValue.toDouble() < 0.0 )
            {
                emit propertyError( PropertyError(
                        PropertyError::BadPropertyValue,
                        propertyName,
                        propertyValue,
                        propertyName + " must not be negative" ) );
                return false;
            }
            // FIXME: warn if Volume is above 1.0?
            return true;
        }
        else if ( fromUpdatedSignal )
        {
            emitUpdatedPropertyTypeError( propertyName, propertyValue, "double", "d" );
            return false;
        }
        return false;
    }
    else if ( propertyName == QLatin1String( "Metadata" ) )
    {
        if ( propertyValue.type() == QVariant::Map )
        {
            QVariantMap metadata = propertyValue.value<QVariantMap>();
            if ( metadata.contains( "mpris:length" ) )
            {
                bool converted = false;
                m_length = metadata.value( "mpris:length" ).toULongLong( &converted );
                if ( !converted )
                    m_length = -1;
            }
            return checkMetadataProperty( metadata );
        }
        else if ( fromUpdatedSignal )
        {
            emitUpdatedPropertyTypeError( propertyName, propertyValue, "QVariantMap", "a{sv}" );
            return false;
        }
        return false;
    }
    else if ( propertyName == QLatin1String( "Position" ) )
    {
        if ( propertyValue.type() == QVariant::LongLong )
        {
            if ( propertyValue.toLongLong() < 0 )
            {
                emit propertyError( PropertyError(
                        PropertyError::BadPropertyValue,
                        propertyName,
                        propertyValue,
                        propertyName + " must not be negative" ) );
                return false;
            }
            else if ( m_length >= 0 && (qulonglong)value.toLongLong() > m_length )
            {
                emit propertyError( PropertyError(
                        PropertyError::BadPropertyValue,
                        propertyName,
                        propertyValue,
                        propertyName + " must not be greater than the track length" ) );
                return false;
            }
            return true;
        }
        else if ( fromUpdatedSignal )
        {
            emitUpdatedPropertyTypeError( propertyName, propertyValue, "longlong", "x" );
            return false;
        }
        return false;
    }
    else
    {
        // Unknown property
        if ( fromUpdatedSignal )
            emit unexpectedPropertyChangedSignal( propertyName );
        return false;
    }
    return true;
}

QVariantMap Mpris2PlayerInterfaceTester::testProperties( bool* allOk )
{
    QVariantMap values;
    bool ok = true;

    QVariant value = testPropertyType( "CanControl" );
    values.insert( "CanControl", value );
    ok &= checkProperty( "CanControl", value );

    value = testPropertyType( "CanGoNext" );
    values.insert( "CanGoNext", value );
    ok &= checkProperty( "CanGoNext", value );

    value = testPropertyType( "CanGoPrevious" );
    values.insert( "CanGoPrevious", value );
    ok &= checkProperty( "CanGoPrevious", value );

    value = testPropertyType( "CanPlay" );
    values.insert( "CanPlay", value );
    ok &= checkProperty( "CanPlay", value );

    value = testPropertyType( "CanPause" );
    values.insert( "CanPause", value );
    ok &= checkProperty( "CanPause", value );

    value = testPropertyType( "CanSeek" );
    values.insert( "CanSeek", value );
    ok &= checkProperty( "CanSeek", value );

    value = testPropertyType( "PlaybackStatus" );
    values.insert( "PlaybackStatus", value );
    ok &= checkProperty( "PlaybackStatus", value );

    value = testOptionalPropertyType( "LoopStatus" );
    values.insert( "PlaybackStatus", value );
    ok &= checkProperty( "PlaybackStatus", value );

    value = testPropertyType( "MinimumRate" );
    values.insert( "MinimumRate", value );
    ok &= checkProperty( "MinimumRate", value );

    value = testPropertyType( "MaximumRate" );
    values.insert( "MaximumRate", value );
    ok &= checkProperty( "MaximumRate", value );

    value = testPropertyType( "Rate" );
    values.insert( "Rate", value );
    ok &= checkProperty( "Rate", value );

    value = testOptionalPropertyType( "Shuffle" );
    values.insert( "Shuffle", value );
    ok &= checkProperty( "Shuffle", value );

    value = testOptionalPropertyType( "Volume" );
    values.insert( "Volume", value );
    ok &= checkProperty( "Volume", value );

    value = testPropertyType( "Metadata" );
    values.insert( "Metadata", value );
    ok &= checkProperty( "Metadata", value );

    value = testPropertyType( "Position" );
    values.insert( "Position", value );
    ok &= checkProperty( "Position", value );

    if ( allOk )
    {
        *allOk = ok;
    }
}

void Mpris2PlayerInterfaceTester::Next()
{
    QDBusReply<> reply = m_interface->Next();
    if ( !reply.isValid() )
    {
        QDBusError error = reply.error();
        if ( m_interface->canGoNext() )
        {
            if ( error.type() == QDBusError::UnknownMethod )
                emit methodError( MethodError( MethodError::MissingMethod, "Next", error ) );
            else
                emit methodError( MethodError( error ) );
        }
    }
}

void Mpris2PlayerInterfaceTester::Previous()
{
    QDBusReply<> reply = m_interface->Previous();
    if ( !reply.isValid() )
    {
        QDBusError error = reply.error();
        if ( m_interface->canGoPrevious() )
        {
            if ( error.type() == QDBusError::UnknownMethod )
                emit methodError( MethodError( MethodError::MissingMethod, "Previous", error ) );
            else
                emit methodError( MethodError( error ) );
        }
    }
}

void Mpris2PlayerInterfaceTester::Pause()
{
    QDBusReply<> reply = m_interface->Pause();
    if ( !reply.isValid() )
    {
        QDBusError error = reply.error();
        if ( m_interface->canPause() )
        {
            if ( error.type() == QDBusError::UnknownMethod )
                emit methodError( MethodError( MethodError::MissingMethod, "Pause", error ) );
            else
                emit methodError( MethodError( error ) );
        }
    }
}

void Mpris2PlayerInterfaceTester::PlayPause()
{
    QDBusReply<> reply = m_interface->PlayPause();
    if ( !reply.isValid() )
    {
        QDBusError error = reply.error();
        if ( m_interface->canPause() )
        {
            if ( error.type() == QDBusError::UnknownMethod )
                emit methodError( MethodError( MethodError::MissingMethod, "PlayPause", error ) );
            else
                emit methodError( MethodError( error ) );
        }
    }
}

void Mpris2PlayerInterfaceTester::Stop()
{
    QDBusReply<> reply = m_interface->Stop();
    if ( !reply.isValid() )
    {
        QDBusError error = reply.error();
        if ( m_interface->canControl() )
        {
            if ( error.type() == QDBusError::UnknownMethod )
                emit methodError( MethodError( MethodError::MissingMethod, "Stop", error ) );
            else
                emit methodError( MethodError( error ) );
        }
    }
}

void Mpris2PlayerInterfaceTester::Play()
{
    QDBusReply<> reply = m_interface->Play();
    if ( !reply.isValid() )
    {
        QDBusError error = reply.error();
        if ( m_interface->canPlay() )
        {
            if ( error.type() == QDBusError::UnknownMethod )
                emit methodError( MethodError( MethodError::MissingMethod, "Play", error ) );
            else
                emit methodError( MethodError( error ) );
        }
    }
}

void Mpris2PlayerInterfaceTester::Seek( qint64 offset )
{
    QDBusReply<> reply = m_interface->Seek(offset);
    if ( !reply.isValid() )
    {
        QDBusError error = reply.error();
        if ( m_interface->canSeek() )
        {
            if ( error.type() == QDBusError::UnknownMethod )
                emit methodError( MethodError( MethodError::MissingMethod, "Seek", error ) );
            else
                emit methodError( MethodError( error ) );
        }
    }
}

void Mpris2PlayerInterfaceTester::SetPosition( const QString& trackId, qint64 position )
{
    QDBusReply<> reply = m_interface->SetPosition(trackId, position);
    if ( !reply.isValid() )
    {
        QDBusError error = reply.error();
        if ( m_interface->canSeek() )
        {
            if ( error.type() == QDBusError::UnknownMethod )
                emit methodError( MethodError( MethodError::MissingMethod, "SetPosition", error ) );
            else
                emit methodError( MethodError( error ) );
        }
    }
}

void Mpris2PlayerInterfaceTester::OpenUri( const QString& uri )
{
    QDBusReply<> reply = m_interface->OpenUri(uri);
    if ( !reply.isValid() )
    {
        QDBusError error = reply.error();
        // FIXME: we need to compare with SupportedUris from the root interface
        if ( true )
        {
            if ( error.type() == QDBusError::UnknownMethod )
                emit methodError( MethodError( MethodError::MissingMethod, "OpenUri", error ) );
            else
                emit methodError( MethodError( error ) );
        }
    }
}


void Mpris2PlayerInterfaceTester::dbusPropertyChanged( const QString &propertyName,
                                                       const QVariant &value )
{
    checkProperty( propertyName, value, true );
}

void Mpris2PlayerInterfaceTester::dbusPropertyInvalidated( const QString &propertyName )
{
    checkProperty( propertyName, testPropertyType( propertyName ) );
}

bool Mpris2PlayerInterfaceTester::checkMetadataProperty( const QVariantMap& metadata )
{
    bool ok = true;

    if ( metadata.contains( "mpris:trackid" ) )
    {
        QVariant trackIdV = metadata.value( "mpris:trackid" );
        if ( trackIdV.type() != QVariant::String )
        {
            emit propertyError( PropertyError(
                    PropertyError::BadPropertyValue,
                    "Metadata",
                    value,
                    QString("mpris:trackid must be a string, but instead was a ") +
                        QMetaType::typeName( trackIdV.userType() ) ) );
        }
        else if ( trackIdV.toString().isEmpty() )
        {
            emit propertyError( PropertyError(
                    PropertyError::BadPropertyValue,
                    "Metadata",
                    value,
                    "mpris:trackid must not be empty" ) );
        }
    }
    else
    {
        emit propertyError( PropertyError(
                PropertyError::BadPropertyValue,
                "Metadata",
                value,
                "mpris:trackid must be present" ) );
        ok = false;
    }

    // FIXME: check more stuff

    return ok;
}

