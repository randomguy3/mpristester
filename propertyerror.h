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

#ifndef PROPERTYERROR_H
#define PROPERTYERROR_H

#include <QString>
#include <QVariant>
#include <QtDBus/QDBusError>

class PropertyError
{
public:
    enum PropertyErrorType
    {
        MissingProperty,
        BadPropertyType,
        BadPropertyValue,
        OtherPropertyError
    };

    PropertyError( const QString& propertyName, const QDBusError& error )
        : m_property( propertyName )
        , m_error( error )
        , m_type( OtherPropertyError )
    { }
    PropertyError( PropertyErrorType type,
                   const QString& propertyName,
                   const QDBusError& error )
        : m_property( propertyName )
        , m_error( error )
        , m_type( type )
    { }
    PropertyError( PropertyErrorType type,
                   const QString& propertyName,
                   const QString& message )
        : m_property( propertyName )
        , m_error( QDBusError( QDBusError::NoError, message ) )
        , m_type( type )
    { }
    PropertyError( PropertyErrorType type,
                   const QString& propertyName,
                   const QVariant& propertyValue,
                   const QString& message )
        : m_property( propertyName )
        , m_value( propertyValue )
        , m_error( QDBusError( QDBusError::NoError, message ) )
        , m_type( type )
    { }
    PropertyError( const PropertyError& other )
        : m_property( other.m_property )
        , m_value( other.m_value )
        , m_error( other.m_error )
        , m_type( other.m_type )
    { }
    PropertyError& operator=( const PropertyError& other )
    {
        m_property = other.m_property;
        m_value = other.m_value;
        m_error = other.m_error;
        m_type = other.m_type;
    }

    QString name() { return m_property; }
    QVariant value() { return m_value; }
    PropertyErrorType type() { return m_type; }
    QString message() { return m_error.message(); }
    QDBusError dbusError() { return m_error; }

private:
    QString m_property;
    QVariant m_value;
    QDBusError m_error;
    PropertyErrorType m_type;
};

#endif // PROPERTYERROR_H
