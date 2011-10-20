/*
 * Copyright 2011  Alex Merry <alex.merry@kdemail.net>
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
#include "rootinterfacetest.h"

#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>

#define MPRIS2_ROOT_IFACE "org.mpris.MediaPlayer2"

using namespace Mpris2;

RootInterfaceTest::RootInterfaceTest(const QString& service, QObject* parent)
    : InterfaceTest(service, MPRIS2_ROOT_IFACE, parent)
{
}

RootInterfaceTest::~RootInterfaceTest()
{
}

void RootInterfaceTest::checkPropertyIdentity(const QVariantMap& oldProps)
{
    if (checkNonEmptyStringPropValid("Identity", oldProps)) {
        QString identity = props["Identity"].toString();
        if (("org.mpris.MediaPlayer2." + identity) == iface->service()) {
            emit interfaceWarning(Property, "Identity", "Identity is the same as the service name (one is user-readable, the other is the 'internal' name)");
        }
    }
}

void RootInterfaceTest::checkPropertyDesktopEntry(const QVariantMap& oldProps)
{
    if (checkNonEmptyStringPropValid("DesktopEntry", oldProps)) {
        // TODO: check for desktop file existence
        // check that desktop name matches identity, warn otherwise
    }
}

void RootInterfaceTest::checkPropertySupportedUriSchemes(const QVariantMap& oldProps)
{
    if (checkPropValid("SupportedUriSchemes", QVariant::Map, oldProps)) {
        QVariantMap uriSchemes = props["SupportedUriSchemes"].toMap();
        if (!uriSchemes.contains("file")) {
            emit interfaceWarning(Property, "SupportedUriSchemes", "\"file\" is not listed as a supported URI scheme (this is unusual)");
        }
        // TODO: check valid protocols
    }
}

void RootInterfaceTest::checkPropertySupportedMimeTypes(const QVariantMap& oldProps)
{
    if (checkPropValid("SupportedMimeTypes", QVariant::Map, oldProps)) {
        QVariantMap mimeTypes = props["SupportedMimeTypes"].toMap();
        if (mimeTypes.isEmpty()) {
            emit interfaceWarning(Property, "SupportedMimeTypes", "The media player claims not to support any mime types");
        }
        // TODO: check valid mimetypes
    }
}

void RootInterfaceTest::checkProps(const QVariantMap& oldProps)
{
    checkPropValid("CanQuit", QVariant::Bool, oldProps);
    checkPropValid("CanRaise", QVariant::Bool, oldProps);
    checkPropValid("HasTrackList", QVariant::Bool, oldProps);
    checkPropertyIdentity(oldProps);
    checkPropertyDesktopEntry(oldProps);
    checkPropertySupportedUriSchemes(oldProps);
    checkPropertySupportedMimeTypes(oldProps);
}

void RootInterfaceTest::checkUpdatedProperty(const QString& propName)
{
    if (propName == "CanQuit") {
        checkPropValid("CanQuit", QVariant::Bool);
    } else if (propName == "CanRaise") {
        checkPropValid("CanRaise", QVariant::Bool);
    } else if (propName == "HasTrackList") {
        checkPropValid("HasTrackList", QVariant::Bool);
    } else if (propName == "Identity") {
        checkPropertyIdentity();
    } else if (propName == "DesktopEntry") {
        checkPropertyDesktopEntry();
    } else if (propName == "SupportedUriSchemes") {
        checkPropertySupportedUriSchemes();
    } else if (propName == "SupportedMimeTypes") {
        checkPropertySupportedMimeTypes();
    }
}

void RootInterfaceTest::testQuit()
{
    QDBusReply<void> reply = iface->call("Quit");
    if (!reply.isValid() && props["CanQuit"].toBool()) {
        emit interfaceError(Method, "Quit", "CanQuit is true, but call to Quit failed with error " + reply.error().message());
    } else if (reply.isValid()) {
        if (!props["CanQuit"].toBool()) {
            emit interfaceInfo(Method, "Quit", "Call to Quit successful, even though CanQuit is false");
        } else {
            emit interfaceInfo(Method, "Quit", "Call to Quit successful; the media player should now quit");
            // TODO: check to see if it has gone away in a few seconds
        }
    }
}

void RootInterfaceTest::testRaise()
{
    QDBusReply<void> reply = iface->call("Raise");
    if (!reply.isValid() && props["CanRaise"].toBool()) {
        emit interfaceError(Method, "Raise", "CanRaise is true, but call to Raise failed with error " + reply.error().message());
    } else if (reply.isValid()) {
        if (!props["CanRaise"].toBool()) {
            emit interfaceInfo(Method, "Raise", "Call to Raise successful, even though CanRaise is false");
        } else {
            emit interfaceInfo(Method, "Raise", "Call to Raise successful; the media player should now be raised");
        }
    }
}

// vim:et:sw=4:sts=4
