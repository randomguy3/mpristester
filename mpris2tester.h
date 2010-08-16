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

#ifndef MPRIS2TESTER_H
#define MPRIS2TESTER_H

#include <QtCore/QObject>
#include <QtDBus/QDBusError>
#include "mpris2service.h"

class Mpris2Tester : public QObject
{
Q_OBJECT
public:
    enum Interface {
        Root,
        Player,
        TrackList
    };

    explicit Mpris2Tester( Mpris2Service* service, QObject* parent = 0 );
    void runInitialTests();

signals:
    void missingProperty( Interface interface,
                          const QString& property,
                          const QString& message );
    void badPropertyValue( Interface interface,
                           const QString& property,
                           const QVariant& value,
                           const QString& message );
    void missingMethod( Interface interface,
                        const QString& method,
                        const QString& message );
    void methodRaisedUnexpectedError( Interface interface,
                                      const QString& method,
                                      const QDBusError& error,
                                      const QString& message );
    void methodRaisedExpectedError( Interface interface,
                                    const QString& method,
                                    const QDBusError& error,
                                    const QString& message );
    void badMethodReturnType( Interface interface,
                              const QString& method,
                              const QString& message );
    // all MPRIS methods have 0 or 1 out parameters
    void badMethodReturnValue( Interface interface,
                               const QString& method,
                               const QVariant& returnValue,
                               const QString& message );
    void badSignalArguments( Interface interface,
                             const QString& signal,
                             const QVariantList& arguments,
                             const QString& message );

public slots:

private:
    Mpris2Service* m_service;
};

#endif // MPRIS2TESTER_H
