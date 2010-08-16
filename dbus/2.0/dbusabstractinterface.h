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

#ifndef DBUSABSTRACTINTERFACE_H
#define DBUSABSTRACTINTERFACE_H

#include <QtDBus/QDBusAbstractInterface>

/**
 * Hack for property notification support
 */
class DBusAbstractInterface : public QDBusAbstractInterface
{
Q_OBJECT
protected:
    explicit DBusAbstractInterface( const QString &service,
                                    const QString &path,
                                    const char *interface,
                                    const QDBusConnection &connection,
                                    QObject *parent);
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

Q_SIGNALS:
    void propertiesChanged( const QMap<QString,QVariant>& changedProperties,
                            const QStringList& invalidatedProperties );
    void propertyChanged( const QString& propertyName, const QVariant& newValue );
    void propertyInvalidated( const QString& propertyName );

private Q_SLOTS:
    void _m_propertiesChanged( const QString& interfaceName,
                               const QVariantMap& changedProperties,
                               const QStringList& invalidatedProperties );

private:
    QVariant demarshall( const QString& property, const QVariant& value );
    bool m_connected;
};

#endif // DBUSABSTRACTINTERFACE_H
