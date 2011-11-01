/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Alex Merry <dev@randomguy3.me.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#define MPRIS2_PATH "/org/mpris/MediaPlayer2"
#define DBUS_PROPS_IFACE "org.freedesktop.DBus.Properties"

#include "interfacetest.h"

#include <QtDBus>
#include <QDebug>
#include <QTimer>

using namespace Mpris2;

InterfaceTest::InterfaceTest ( const QString& interface, const QString& service, QObject* parent ) : QObject(parent)
{
    iface = new QDBusInterface(service, MPRIS2_PATH, interface, QDBusConnection::sessionBus(), this);
    propsIface = new QDBusInterface(service, MPRIS2_PATH, DBUS_PROPS_IFACE, QDBusConnection::sessionBus(), this);
    delayedCheckTimer = new QTimer(this);
    delayedCheckTimer->setInterval(2000);
    connect(delayedCheckTimer, SIGNAL(timeout()),
            this, SLOT(delayedIncrementalCheck()));
}

InterfaceTest::~InterfaceTest()
{
}

QVariantMap InterfaceTest::properties() const
{
    return props;
}

bool InterfaceTest::getAllProps()
{
    if (!propsIface->isValid() || !iface->isValid()) {
        emit interfaceError(Other, "",
                            "No object with the "
                            + iface->interface() +
                            " and "
                            DBUS_PROPS_IFACE
                            " interfaces was not found at "
                            MPRIS2_PATH);
        return false;
    }

    QDBusReply<QVariantMap> propsReply = propsIface->call("GetAll", iface->interface());
    if (!propsReply.isValid()) {
        emit interfaceError(Other, "",
                            "The " DBUS_PROPS_IFACE
                            " interface was not implemented correctly at "
                            MPRIS2_PATH);
        return false;
    }
    props = propsReply.value();

    return true;
}

bool InterfaceTest::getProp(const QString& propName)
{
    if (!propsIface->isValid() || !iface->isValid()) {
        emit interfaceError(Other, "",
                            "No object with the "
                            + iface->interface() +
                            " and "
                            DBUS_PROPS_IFACE
                            " interfaces was not found at "
                            MPRIS2_PATH);
        return false;
    }

    QDBusReply<QVariant> propsReply = propsIface->call("Get", iface->interface(), propName);
    if (!propsReply.isValid()) {
        if (propsReply.error().type() == QDBusError::UnknownInterface) {
            emit interfaceError(Other, "",
                                "The " DBUS_PROPS_IFACE
                                " interface was not implemented correctly at "
                                MPRIS2_PATH "(replied \"unknown interface\")");
        } else if (propsReply.error().type() == QDBusError::UnknownMethod) {
            emit interfaceError(Other, "",
                                "The " DBUS_PROPS_IFACE
                                " interface was not implemented correctly at "
                                MPRIS2_PATH "(replied \"unknown method\" for method \"Get\")");
        } else if (propsReply.error().type() == QDBusError::InvalidArgs) {
            emit interfaceError(Property, propName,
                                DBUS_PROPS_IFACE ".Get failed; reported \"invalid args\" (ie: unknown property)");
        } else if (propsReply.error().name() == "org.freedesktop.DBus.UnknownProperty") {
            emit interfaceError(Property, propName,
                                DBUS_PROPS_IFACE ".Get failed; reported \"unknown property\"");
        } else {
            emit interfaceError(Other, "",
                                "Calling " DBUS_PROPS_IFACE
                                ".Get resulted in the error " + propsReply.error().name());
        }
        return false;
    }
    props[propName] = propsReply.value();

    return true;
}

bool InterfaceTest::checkPropValid(const QString& propName, QVariant::Type expType, const QVariantMap& oldProps) {
    if (!props.contains(propName)) {
        emit interfaceError(Property, propName, "Property " + propName + " is missing");
        return false;
    } else if (props[propName].type() != expType) {
        // FIXME: generate D-Bus type description
        const char * gotTypeCh = QDBusMetaType::typeToSignature(props[propName].userType());
        QString gotType = gotTypeCh ? QString::fromAscii(gotTypeCh) : "<unknown>";
        const char * expTypeCh = QDBusMetaType::typeToSignature(expType);
        QString expType = expTypeCh ? QString::fromAscii(expTypeCh) : "<unknown>";

        emit interfaceError(Property, propName, "Property " + propName + " has type '" + gotType + "', but should be type '" + expType + "'");
        return false;
    } else if (oldProps.contains(propName)) {
        // FIXME: QVariant equality only works for builtin types
        if (props[propName] != oldProps[propName]) {
            outOfDateProperties.insert(propName, props[propName]);
            props[propName] = oldProps[propName];
            // don't check right now
            return false;
        }
    }
    return true;
}

bool InterfaceTest::checkNonEmptyStringPropValid(const QString& propName, const QVariantMap& oldProps) {
    if (checkPropValid(propName, QVariant::String, oldProps)) {
        if (props[propName].toString().isEmpty()) {
            emit interfaceError(Property, propName, "Property " + propName + " is present, but empty");
        } else {
            return true;
        }
    }
    return false;
}

void InterfaceTest::initialTest()
{
    if (!getAllProps())
        return;

    checkProps();

    QDBusConnection::sessionBus().connect(iface->service(), iface->path(), iface->interface(),
            "propertiesChanged", /* signature, */
            this, SLOT( _m_propertiesChanged(QString,QVariantMap,QStringList)));

    emit propertiesChanged(properties().keys());
}

void InterfaceTest::incrementalTest()
{
    QVariantMap oldProps = props;

    if (!getAllProps())
        return;

    checkProps(oldProps);

    if (!delayedCheckTimer->isActive())
        delayedCheckTimer->start();
}

void InterfaceTest::_m_propertiesChanged(const QString& interface,
                                         const QVariantMap& changedProperties,
                                         const QStringList& invalidatedProperties,
                                         const QDBusMessage& signalMessage)
{
    Q_UNUSED(interface)
    Q_UNUSED(signalMessage)

    QStringList changedPropsList = invalidatedProperties;
    QVariantMap::const_iterator i = changedProperties.constBegin();
    while (i != changedProperties.constEnd()) {
        props[i.key()] = i.value();
        checkUpdatedProperty(i.key());
        outOfDateProperties.remove(i.key());
        changedPropsList << i.key();
        ++i;
    }
    QStringList::const_iterator j = invalidatedProperties.constBegin();
    while (j != invalidatedProperties.constEnd()) {
        if (getProp(*j))
            checkUpdatedProperty(*j);
        outOfDateProperties.remove(*j);
        ++j;
    }
    checkConsistency();

    emit propertiesChanged(changedPropsList);
}

void InterfaceTest::delayedIncrementalCheck()
{
    QVariantMap::const_iterator i = outOfDateProperties.constBegin();
    while (i != outOfDateProperties.constEnd()) {
        int warningsSoFar = propertyUpdateWarningCount.value(i.key());
        if (warningsSoFar < 4) {
            emit interfaceWarning(Property, i.key(), "Property was not updated via PropertiesChanged signal");
        } else if (warningsSoFar == 4) {
            emit interfaceWarning(Property, i.key(), "Property was not updated via PropertiesChanged signal [further warnings for this property suppressed]");
        }
        propertyUpdateWarningCount.insert(i.key(), warningsSoFar + 1);
        props[i.key()] = i.value();
        ++i;
    }
    outOfDateProperties.clear();
}


