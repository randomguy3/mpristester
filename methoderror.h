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

#ifndef METHODERROR_H
#define METHODERROR_H

#include <QString>
#include <QVariant>
#include <QtDBus/QDBusError>

class MethodError
{
public:
    enum MethodErrorType
    {
        MissingMethod,
        BadReturnType,
        BadReturnValue,
        OtherMethodError
    };

    MethodError( const QString& methodName, const QDBusError& error )
        : m_property( methodName )
        , m_error( error )
        , m_type( OtherMethodError )
    { }
    MethodError( MethodErrorType type,
                   const QString& methodName,
                   const QDBusError& error )
        : m_property( methodName )
        , m_error( error )
        , m_type( type )
    { }
    MethodError( MethodErrorType type,
                   const QString& methodName,
                   const QString& message )
        : m_property( methodName )
        , m_error( QDBusError( QDBusError::NoError, message ) )
        , m_type( type )
    { }
    MethodError( const MethodError& other )
        : m_property( other.m_property )
        , m_error( other.m_error )
        , m_type( other.m_type )
    { }
    MethodError& operator=( const MethodError& other )
    {
        m_property = other.m_property;
        m_error = other.m_error;
        m_type = other.m_type;
    }

    QString name() { return m_method; }
    MethodErrorType type() { return m_type; }
    QString message() { return m_error.message(); }
    QDBusError dbusError() { return m_error; }

private:
    QString m_method;
    QDBusError m_error;
    MethodErrorType m_type;
};

#endif // METHODERROR_H
