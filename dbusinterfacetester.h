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

#ifndef DBUSINTERFACETESTER_H
#define DBUSINTERFACETESTER_H

#include <QObject>
#include <QVariant>
#include <QtDBus/QDBusError>
#include "propertyerror.h"
#include "methoderror.h"
class QStringList;
class QDBusAbstractInterface;

class DBusInterfaceTester : public QObject
{
Q_OBJECT
public:
    explicit DBusInterfaceTester( QDBusAbstractInterface * interface,
                                  QObject *parent = 0 );

signals:
    void propertyError( const PropertyError& problem );
    void methodError( const MethodError& problem );
    void dbusError( const QDBusError& error );
    void unexpectedPropertyChangedSignal( const QString& property );

protected:
    QVariant testPropertyType( const QString& property );
    QVariant testOptionalPropertyType( const QString& property );
    bool checkStringEnumProperty( const QString& propertyName,
                                  const QStringList& acceptableValues,
                                  const QString& value );
    void emitUpdatedPropertyTypeError( const QString& property,
                                       const QVariant& value,
                                       const QLatin1String& expTypeName,
                                       const QLatin1String& expTypeSig );

private:
    QDBusAbstractInterface * m_interface;
};

#endif // DBUSINTERFACETESTER_H
