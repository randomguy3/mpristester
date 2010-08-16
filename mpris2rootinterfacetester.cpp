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

#include "mpris2rootinterfacetester.h"
#include "dbus/2.0/root_proxy.h"
#include "QtCore/QRegExp"

Mpris2RootInterfaceTester::Mpris2RootInterfaceTester(
        Mpris2RootProxy * interface,
        QObject *parent)
            : DBusInterfaceTester( parent )
            , m_interface( interface )
{
    connect( interface, SIGNAL(propertyChanged(QString,QVariant)),
             this, SLOT(dbusPropertyChanged(QString,QVariant)) );
    connect( interface, SIGNAL(propertyInvalidated(QString)),
             this, SLOT(dbusPropertyInvalidated(QString)) );
}

QVariantMap Mpris2RootInterfaceTester::testProperties( bool* allOk )
{
    QVariantMap values;
    bool ok = true;
    values.insert( "CanQuit", testPropertyType( "CanQuit" ) );
    ok &= values.value( "CanQuit" ).isValid();
    values.insert( "CanRaise", testPropertyType( "CanRaise" ) );
    ok &= values.value( "CanRaise" ).isValid();
    values.insert( "HasTrackList", testPropertyType( "HasTrackList" ) );
    ok &= values.value( "HasTrackList" ).isValid();
    values.insert( "Identity", testPropertyType( "Identity" ) );
    ok &= values.value( "Identity" ).isValid();

    QVariant value = testOptionalPropertyType( "DesktopEntry" );
    values.insert( "DesktopEntry", value );
    if ( value.isValid() )
    {
        QString desktopEntry = value.toString();
        if ( desktopEntry.endsWith( ".desktop" ) )
        {
            emit propertyError( PropertyError(
                    PropertyError::BadPropertyValue,
                    "DesktopEntry",
                    value,
                    "DesktopEntry should not end with .desktop" ) );
            ok = false;
        }
    }
    else
    {
        ok = false;
    }

    value = testPropertyType( "SupportedUriSchemes" );
    values.insert( "SupportedUriSchemes", value );
    if ( value.isValid() )
    {
        ok &= checkUriSchemesProperty( value.toStringList() );
    }
    else
    {
        ok = false;
    }

    value = testPropertyType( "SupportedMimeTypes" );
    values.insert( "SupportedMimeTypes", value );
    if ( value.isValid() )
    {
        ok &= checkMimetypesProperty( value.toStringList() );
    }
    else
    {
        ok = false;
    }

    if ( allOk )
    {
        *allOk = ok;
    }
}

void Mpris2RootInterfaceTester::Quit()
{
    QDBusReply<> reply = m_interface->Quit();
    if ( !reply.isValid() )
    {
        QDBusError error = reply.error();
        if ( m_interface->canQuit() )
        {
            if ( error.type() == QDBusError::UnknownMethod )
                emit methodError( MethodError( MethodError::MissingMethod, "Quit", error ) );
            else
                emit methodError( MethodError( error ) );
        }
    }
}

void Mpris2RootInterfaceTester::Raise()
{
    QDBusReply<> reply = m_interface->Raise();
    if ( !reply.isValid() )
    {
        QDBusError error = reply.error();
        if ( m_interface->canRaise() )
        {
            if ( error.type() == QDBusError::UnknownMethod )
                emit methodError( MethodError( MethodError::MissingMethod, "Raise", error ) );
            else
                emit methodError( MethodError( error ) );
        }
    }
}

void Mpris2RootInterfaceTester::dbusPropertyChanged( const QString &propertyName,
                                                     const QVariant &value )
{
    Q_UNUSED( value )
    emit unexpectedPropertyChangedSignal( propertyName );
}

void Mpris2RootInterfaceTester::dbusPropertyInvalidated( const QString &propertyName )
{
    emit unexpectedPropertyChangedSignal( propertyName );
}

bool Mpris2RootInterfaceTester::checkUriSchemesProperty( const QStringList& schemes )
{
    QRegExp schemePattern = QRegExp( "[a-z][-a-z0-9+.]*" );
    foreach ( const QString& scheme, schemes )
    {
        if ( scheme.endsWith( "://" ) )
        {
            emit propertyError( PropertyError(
                    PropertyError::BadPropertyValue,
                    "SupportedUriSchemes",
                    value,
                    "URI schemes must not end with ://" ) );
            ok = false;
            break;
        }
        else if ( scheme.endsWith( ':' ) )
        {
            emit propertyError( PropertyError(
                    PropertyError::BadPropertyValue,
                    "SupportedUriSchemes",
                    value,
                    "URI schemes must not end with :" ) );
            ok = false;
            break;
        }
        else if ( !schemePattern.exactMatch( scheme ) )
        {
            emit propertyError( PropertyError(
                    PropertyError::BadPropertyValue,
                    "SupportedUriSchemes",
                    value,
                    '"' + scheme + "\" is not a valid URI scheme" ) );
            ok = false;
            break;
        }
    }
}

bool Mpris2RootInterfaceTester::checkMimetypesProperty( const QStringList& mimetypes )
{
    QStringList standardContentTypes;
    standardContentTypes << "application" << "audio" << "example"
                         << "image" << "message" << "model"
                         << "multipart" << "text" << "video";
    foreach ( const QString& mimetype, mimetypes )
    {
        QStringList parts = mimetype.split( '/', QString::KeepEmptyParts );
        if ( parts.count() == 0 || parts.count() > 2 )
        {
            emit propertyError( PropertyError(
                    PropertyError::BadPropertyValue,
                    "SupportedMimeTypes",
                    value,
                    '"' + mimetype + "\" is not a valid mimetype" ) );
            return false;
        }
        else
        {
            QString contentType = parts.at( 0 );
            if ( !contentType.startsWith( "x-", Qt::CaseInsensitive ) )
            {
                if ( !standardContentTypes.contains( contentType, Qt::CaseInsensitive ) )
                {
                    emit propertyError( PropertyError(
                            PropertyError::BadPropertyValue,
                            "SupportedMimeTypes",
                            value,
                            '"' + mimetype + "\" is not a valid mimetype"
                            " (" + contentType + " is not a known content type)" ) );
                    return false;
                }
            }
        }
    }
    return true;
}
