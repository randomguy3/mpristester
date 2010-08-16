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

#ifndef MPRIS2ROOTINTERFACETESTER_H
#define MPRIS2ROOTINTERFACETESTER_H

#include "dbusinterfacetester.h"
#include <QVariantMap>
class Mpris2RootProxy;

class Mpris2RootInterfaceTester : public DBusInterfaceTester
{
Q_OBJECT
public:
    explicit Mpris2RootInterfaceTester( Mpris2RootProxy * interface,
                                        QObject* parent = 0 );
    QVariantMap testProperties( bool* allOk = 0 );

signals:

public slots:
    void Quit();
    void Raise();

private slots:
    void dbusPropertyChanged( const QString& propertyName, const QVariant& value );
    void dbusPropertyInvalidated( const QString& propertyName );

private:
    bool checkUriSchemesProperty( const QStringList& schemes );
    bool checkMimetypesProperty( const QStringList& mimetypes );

    Mpris2RootProxy * m_interface;
};

#endif // MPRIS2ROOTINTERFACETESTER_H
