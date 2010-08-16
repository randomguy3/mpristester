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
 *
 * This file contains code from Qt:
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 */

#include "dbusinterfacetester.h"
#include <QtCore/QStringList>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusAbstractInterface>
#include <QtDBus/QDBusMetaType>

DBusInterfaceTester::DBusInterfaceTester( QDBusAbstractInterface * interface,
                                          QObject *parent ) :
    QObject( parent ),
    m_interface( interface )
{
}

QVariant DBusInterfaceTester::testPropertyType( const QString &property )
{
    QVariant value = interface->property( property );
    if ( value.type() == QVariant::Invalid )
    {
        QDBusError error = m_interface->lastError();

        if ( error.type() == QDBusError::InvalidSignature )
            emit propertyError( PropertyError( PropertyError::BadPropertyType,
                                               property, error ) );
        else if ( error.type() == QDBusError::InvalidArgs )
            emit propertyError( PropertyError( PropertyError::MissingProperty,
                                               property, error ) );
        else
            emit propertyError( PropertyError( error ) );
    }
    return value;
}

QVariant DBusInterfaceTester::testOptionalPropertyType( const QString &property )
{
    QVariant value = interface->property( property );
    if ( value.type() == QVariant::Invalid )
    {
        QDBusError error = m_interface->lastError();

        if ( error.type() == QDBusError::InvalidSignature )
            emit propertyError( PropertyError( PropertyError::BadPropertyType,
                                               property, error ) );
        else if ( error.type() != QDBusError::InvalidArgs )
            emit propertyError( PropertyError( error ) );
    }
    return value;
}

bool DBusInterfaceTester::checkStringEnumProperty( const QString& propertyName,
                                                   const QStringList& acceptableValues,
                                                   const QString& value )
{
    if ( !acceptableValues.contains( value ) )
    {
        emit propertyError( PropertyError(
                PropertyError::BadPropertyValue,
                propertyName,
                QVariant( value ),
                '"' + value + "\" is not a valid value for " + propertyName ) );
        return false;
    }
    return true;
}

void DBusInterfaceTester::emitUpdatedPropertyTypeError( const QString& property,
                                                        const QVariant& value,
                                                        const QLatin1String& expTypeName,
                                                        const QLatin1String& expTypeSig )
{
    QString errorMessage = QLatin1String(
            "Unexpected `%1' (%2) in update for property `%3' "
            "(expected type `%5' (%6))" );

    const char * foundType;
    const char * foundSignature;
    if ( value.userType() == qMetaTypeId<QDBusArgument>() )
    {
        QDBusArgument arg = qvariant_cast<QDBusArgument>(value);

        foundType = "user type";
        foundSignature = arg.currentSignature().toLatin1();
    }
    else
    {
        foundType = propertyValue.typeName();
        foundSignature = QDBusMetaType::typeToSignature(value.userType());
    }

    emit propertyError( PropertyError(
            PropertyError::BadPropertyType,
            property,
            QDBusError( QDBusError::InvalidSignature,
                        errorMessage.arg(foundType).arg(foundSignature)
                                    .arg(propertyName)
                                    .arg(expTypeName).arg(expTypeSig) )
            ) );
}
