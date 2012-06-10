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

void InterfaceTest::connectSignals()
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

bool InterfaceTest::getProp(const QString& propName, InterfaceTest::PropErrorAllowance allowError)
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
            if (allowError & PropAllowMissing) {
                emit interfaceInfo(Property, propName,
                                   DBUS_PROPS_IFACE ".Get failed; reported \"invalid args\" (ie: unknown property)");
            } else {
                emit interfaceError(Property, propName,
                                    DBUS_PROPS_IFACE ".Get failed; reported \"invalid args\" (ie: unknown property)");
            }
        } else if (propsReply.error().name() == "org.freedesktop.DBus.UnknownProperty") {
            if (allowError & PropAllowMissing) {
                emit interfaceInfo(Property, propName,
                                   DBUS_PROPS_IFACE ".Get failed; reported \"unknown property\"");
            } else {
                emit interfaceError(Property, propName,
                                    DBUS_PROPS_IFACE ".Get failed; reported \"unknown property\"");
            }
        } else {
            if (allowError) {
                emit interfaceWarning(Other, "",
                                    "Calling " DBUS_PROPS_IFACE
                                    ".Get resulted in the unexpected error " + propsReply.error().name());
            } else {
                emit interfaceError(Other, "",
                                    "Calling " DBUS_PROPS_IFACE
                                    ".Get resulted in the error " + propsReply.error().name());
            }
        }
        return false;
    }
    props[propName] = propsReply.value();

    return true;
}

bool InterfaceTest::setProp(const QString& propName, const QDBusVariant& propValue, InterfaceTest::PropErrorAllowance allowError)
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

    QDBusReply<void> propsReply = propsIface->call("Set", iface->interface(), propName, QVariant::fromValue<QDBusVariant>(propValue));
    if (propsReply.isValid()) {
        return true;
    } else {
        if (propsReply.error().type() == QDBusError::UnknownInterface) {
            emit interfaceError(Other, "",
                                "The " DBUS_PROPS_IFACE
                                " interface was not implemented correctly at "
                                MPRIS2_PATH "(replied \"unknown interface\")");
        } else if (propsReply.error().type() == QDBusError::UnknownMethod) {
            emit interfaceError(Other, "",
                                "The " DBUS_PROPS_IFACE
                                " interface was not implemented correctly at "
                                MPRIS2_PATH "(replied \"unknown method\" for method \"Set\")");
        } else if (propsReply.error().type() == QDBusError::InvalidArgs) {
            if (allowError & PropAllowMissing || allowError & PropAllowReadOnly) {
                emit interfaceInfo(Property, propName,
                                   DBUS_PROPS_IFACE ".Set failed; reported \"invalid args\"");
            } else {
                emit interfaceError(Property, propName,
                                    DBUS_PROPS_IFACE ".Set failed; reported \"invalid args\"");
            }
        } else if (propsReply.error().name() == "org.freedesktop.DBus.UnknownProperty") {
            if (allowError & PropAllowMissing) {
                emit interfaceInfo(Property, propName,
                                   DBUS_PROPS_IFACE ".Set failed; reported \"unknown property\"");
            } else {
                emit interfaceError(Property, propName,
                                    DBUS_PROPS_IFACE ".Set failed; reported \"unknown property\"");
            }
        } else if (propsReply.error().name() == "com.trolltech.QtDBus.Error.InternalError") {
            // Qt returns InternalError for read-only properties
            if (allowError & PropAllowReadOnly) {
                emit interfaceInfo(Property, propName,
                                   DBUS_PROPS_IFACE ".Set failed; reported \"internal error\" (Qt's way of saying \"read only\")");
            } else {
                emit interfaceError(Property, propName,
                                    DBUS_PROPS_IFACE ".Set failed; reported \"internal error\" (Qt's way of saying \"read only\")");
            }
        } else {
            if (allowError) {
                emit interfaceWarning(Other, "",
                                    "Calling " DBUS_PROPS_IFACE
                                    ".Set resulted in the unexpected error " + propsReply.error().name());
            } else {
                emit interfaceError(Other, "",
                                    "Calling " DBUS_PROPS_IFACE
                                    ".Set resulted in the error " + propsReply.error().name());
            }
        }
        return false;
    }
}

bool InterfaceTest::checkPropValid(const QString& propName, QVariant::Type expType, const QVariantMap& oldProps) {
    if (!props.contains(propName)) {
        emit interfaceError(Property, propName, "Property " + propName + " is missing");
        return false;
    } else {
        return checkOptionalPropValid(propName, expType, oldProps);
    }
}

bool InterfaceTest::checkOptionalPropValid(const QString& propName, QVariant::Type expType, const QVariantMap& oldProps) {
    if (!props.contains(propName)) {
        return false;
    } else if (props.value(propName).type() != expType) {
        // FIXME: generate D-Bus type description
        const char * gotTypeCh = QDBusMetaType::typeToSignature(props.value(propName).userType());
        QString gotType = gotTypeCh ? QString::fromAscii(gotTypeCh) : "<unknown>";
        const char * expTypeCh = QDBusMetaType::typeToSignature(expType);
        QString expType = expTypeCh ? QString::fromAscii(expTypeCh) : "<unknown>";

        emit interfaceError(Property, propName, "Property " + propName + " has type '" + gotType + "', but should be type '" + expType + "'");
        return false;
    } else if (oldProps.contains(propName)) {
        // FIXME: QVariant equality only works for builtin types
        if (props.value(propName).type() < QVariant::UserType) {
            if (props.value(propName) != oldProps.value(propName)) {
                outOfDateProperties.insert(propName, props.value(propName));
                props[propName] = oldProps.value(propName);
            }
        } else {
            qDebug() << "Could not check equality for" << propName;
        }
        // we have either already checked this, or we will check it
        // soon
        return false;
    }
    return true;
}

bool InterfaceTest::checkOptionalNonEmptyStringPropValid(const QString& propName, const QVariantMap& oldProps) {
    if (checkOptionalPropValid(propName, QVariant::String, oldProps)) {
        if (props[propName].toString().isEmpty()) {
            emit interfaceError(Property, propName, "Property " + propName + " is present, but empty");
        } else {
            return true;
        }
    }
    return false;
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

    QDBusConnection::sessionBus().connect(
            iface->service(),
            iface->path(),
            DBUS_PROPS_IFACE,
            "PropertiesChanged", /* signature, */
            this,
            SLOT( _m_propertiesChanged(QString,QVariantMap,QStringList,QDBusMessage)));
    connectSignals();

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

    QVariantMap oldProps = props;

    QStringList changedPropsList = invalidatedProperties;
    QVariantMap::const_iterator i = changedProperties.constBegin();
    while (i != changedProperties.constEnd()) {
        if (propsNotUpdated.contains(i.key())) {
            emit interfaceError(Signal, "PropertiesChanged", "PropertiesChanged signal sent for " + i.key() + "; you almost certainly didn't want to do this");
        } else {
            props[i.key()] = i.value();
            checkUpdatedProperty(i.key());
            outOfDateProperties.remove(i.key());
            changedPropsList << i.key();
        }
        ++i;
    }
    QStringList::const_iterator j = invalidatedProperties.constBegin();
    while (j != invalidatedProperties.constEnd()) {
        if (propsNotUpdated.contains(*j)) {
            emit interfaceError(Signal, "PropertiesChanged", "PropertiesChanged signal sent for " + *j + "; you almost certainly didn't want to do this");
        } else {
            if (getProp(*j))
                checkUpdatedProperty(*j);
            outOfDateProperties.remove(*j);
        }
        ++j;
    }
    checkConsistency(oldProps);

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

void InterfaceTest::checkMetadata(const QVariantMap& metadata,
                                  QStringList* errors,
                                  QStringList* warnings,
                                  QStringList* infoMessages)
{
    if (!metadata.contains("mpris:trackid")) {
        (*errors) << "No mpris:trackid entry";
    } else if (metadata.value("mpris:trackid").userType() != qMetaTypeId<QDBusObjectPath>()) {
        (*errors) << "mpris:trackid entry was not sent as a D-Bus object path (D-Bus type 'o')";
    } else {
        QDBusObjectPath trackid = metadata.value("mpris:trackid").value<QDBusObjectPath>();
        if (trackid.path().isEmpty()) {
            (*errors) << "mpris:trackid entry is an empty path (seriously, how did you convince D-Bus to let you do that?)";
        } else if (trackid.path() == "/org/mpris/MediaPlayer2/TrackList/NoTrack") {
            (*warnings) << "mpris:trackid entry indicates \"no track\"; for compatibility with older clients, this should be represented by providing no metadata";
        } else if (trackid.path().startsWith("/org/mpris/")) {
            (*warnings) << "The /org/mpris/ namespace is reserved, and should not be used for track ids";
        }
    }

    checkMetadataEntry(metadata, "mpris:length", QVariant::LongLong, errors, warnings, infoMessages);

    if (checkMetadataEntry(metadata, "mpris:artUrl", QVariant::Url, errors, warnings, infoMessages)) {
        QString artUrl = metadata.value("mpris:artUrl").toString();
        QUrl asUrl(artUrl, QUrl::StrictMode);
        if (asUrl.scheme() != "file" && asUrl.scheme() != "http" && asUrl.scheme() != "https") {
            (*infoMessages) << "mpris:artUrl has a scheme (" + asUrl.scheme() + ") which not all clients may recognise";
        } else {
            if (asUrl.scheme() == "file") {
                if (!QFile::exists(asUrl.toLocalFile())) {
                    (*infoMessages) << "mpris:artUrl references a file that does not exist:" << asUrl.toLocalFile();
                }
            }
            // TODO: check network files
        }
    }

    Q_FOREACH( QString key, metadata.keys() ) {
        if (!key.startsWith("xesam:")) {
            if (key != "mpris:trackid" &&
                key != "mpris:length" &&
                key != "mpris:artUrl")
            {
                (*warnings) << "Unrecognised entry " + key;
            }
        }
    }

    checkMetadataEntry(metadata, "xesam:album", QVariant::String, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:albumArtist", QVariant::StringList, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:artist", QVariant::StringList, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:asText", QVariant::String, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:audioBpm", QVariant::Int, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:autoRating", QVariant::Double, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:comment", QVariant::StringList, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:composer", QVariant::StringList, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:contentCreator", QVariant::DateTime, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:discNumber", QVariant::Int, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:firstUsed", QVariant::DateTime, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:genre", QVariant::StringList, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:lastUsed", QVariant::DateTime, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:lyricist", QVariant::StringList, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:title", QVariant::String, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:trackNumber", QVariant::Int, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:url", QVariant::Url, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:useCount", QVariant::Int, errors, warnings, infoMessages);
    checkMetadataEntry(metadata, "xesam:userRating", QVariant::Double, errors, warnings, infoMessages);
}

bool InterfaceTest::checkMetadataEntry(const QVariantMap& metadata,
                                       const QString& entry,
                                       QVariant::Type expType,
                                       QStringList* errors,
                                       QStringList* warnings,
                                       QStringList* infoMessages)
{
    if (metadata.contains(entry)) {
        QVariant value = metadata.value(entry);

        bool propertyTypeError = false;
        bool propertyTypeWarning = false;
        QVariant::Type realExpectedType = expType;
        if (expType == QVariant::DateTime || expType == QVariant::Url) {
            realExpectedType = QVariant::String;
        }

        // be lax about integers
        if (realExpectedType == QVariant::Int) {
            if (value.type() == QVariant::UInt ||
                value.type() == QVariant::LongLong ||
                value.type() == QVariant::ULongLong)
            {
                propertyTypeWarning = true;
            } else if (value.type() != QVariant::Int) {
                propertyTypeError = true;
            }
        } else if (realExpectedType == QVariant::UInt || realExpectedType == QVariant::LongLong) {
            if (value.type() == QVariant::ULongLong) {
                propertyTypeWarning = true;
            } else if (value.type() != realExpectedType) {
                propertyTypeError = true;
            }
        } else if (value.type() != realExpectedType) {
            propertyTypeError = true;
        }

        if (propertyTypeError || propertyTypeWarning) {
            const char * gotTypeCh = QDBusMetaType::typeToSignature(value.userType());
            QString gotType = gotTypeCh ? QString::fromAscii(gotTypeCh) : "<unknown>";
            const char * expTypeCh = QDBusMetaType::typeToSignature(realExpectedType);
            QString expType = expTypeCh ? QString::fromAscii(expTypeCh) : "<unknown>";
            if (propertyTypeError) {
                (*errors) << entry + " entry is of type '" + gotType + "' but should have been of type '" + expType + "'";
                return false;
            } else {
                (*warnings) << entry + " entry is of type '" + gotType + "' but should have been of type '" + expType + "'";
                return true;
            }
        }

        // extra checks for special types
        if (expType == QVariant::DateTime) {
            QDateTime dtValue = QDateTime::fromString(value.toString(), Qt::ISODate);
            if (!dtValue.isValid()) {
                (*errors) << entry + " entry does not contain a valid date/time string (value was " + value.toString() + ")";
                return false;
            }
        } else if (expType == QVariant::Url) {
            if (value.toString().isEmpty()) {
                return false;
            } else {
                QUrl asUrl(value.toString(), QUrl::StrictMode);
                if (!asUrl.isValid()) {
                    (*errors) << entry + " entry is not a valid URL";
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

// vim:et:sw=4:sts=4
