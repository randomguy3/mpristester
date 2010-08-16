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

#include "dbusabstractinterface.h"
#include <QtDBus/QDBusArgument>

Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, \
                          propertiesChangedSignature, \
                          (QMetaObject::normalizedSignature(SIGNAL(propertiesChanged(QMap<QString,QVariant>,QStringList)))))
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, \
                          propertyChangedSignature, \
                          (QMetaObject::normalizedSignature(SIGNAL(propertyChanged(QString,QVariant)))))

DBusAbstractInterface::DBusAbstractInterface(
        const QString &service,
        const QString &path,
        const char *interface,
        const QDBusConnection &connection,
        QObject *parent)
        : QDBusAbstractInterface( service, path, interface, connection, parent )
        , m_connected( false )
{

}

void DBusAbstractInterface::connectNotify(const char *signal)
{
    if ( QLatin1String(signal) == *propertiesChangedSignature() ||
         QLatin1String(signal) == *propertyChangedSignature() )
    {
        if ( !m_connected )
        {
            connection().connect( service(),
                                  path(),
                                  "org.freedesktop.DBus.Properties",
                                  "PropertiesChanged",
                                  this,
                                  SLOT(_m_propertiesChanged(QString,QMap<QString,QVariant>,QStringList)) );
            m_connected = true;
            return;
        }
    }
    else
    {
        QDBusAbstractInterface::connectNotify( signal );
    }
}

void DBusAbstractInterface::disconnectNotify(const char *signal)
{
    if ( QLatin1String(signal) == *propertiesChangedSignature() ||
         QLatin1String(signal) == *propertyChangedSignature() )
    {
        if ( m_connected &&
             receivers(propertiesChangedSignature()->constData()) == 0 &&
             receivers(propertyChangedSignature()->constData()) == 0 )
        {
            connection().disconnect( service(),
                                     path(),
                                     "org.freedesktop.DBus.Properties",
                                     "PropertiesChanged",
                                     this,
                                     SLOT(_m_propertiesChanged(QString,QMap<QString,QVariant>,QStringList)) );
            m_connected = false;
            return;
        }
    }
    else
    {
        QDBusAbstractInterface::disconnectNotify( signal );
    }
}


void DBusAbstractInterface::_m_propertiesChanged(
        const QString& interfaceName,
        const QVariantMap& changedProperties,
        const QStringList& invalidatedProperties )
{
    if ( interfaceName == interface() )
    {
        emit propertiesChanged( changedProperties, invalidatedProperties );
        QMapIterator<QString, QVariant> i( changedProperties );
        while ( i.hasNext() ) {
            i.next();
            if ( i.value().userType() == qMetaTypeId<QDBusArgument>() )
                qFatal( "We do need to demarshall" );
            emit propertyChanged( i.key(), i.value() );
        }
        QListIterator<QString> j( invalidatedProperties );
        while ( j.hasNext() ) {
            emit propertyInvalidated( j.next() );
        }
    }
}
