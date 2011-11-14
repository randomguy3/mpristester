/*
 * Copyright 2011  Alex Merry <dev@randomguy3.me.uk>
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
#include <QDir>
#include <QSettings>
#include <QString>
#include <QTextStream>

#define MPRIS2_ROOT_IFACE "org.mpris.MediaPlayer2"

using namespace Mpris2;

RootInterfaceTest::RootInterfaceTest(const QString& service, QObject* parent)
    : InterfaceTest(MPRIS2_ROOT_IFACE, service, parent)
{
    // http://standards.freedesktop.org/basedir-spec/0.6/
    QStringList dataPaths;
    QByteArray xdgDataHomeVar = qgetenv("XDG_DATA_HOME");
    if (!xdgDataHomeVar.isEmpty()) {
        dataPaths << QFile::decodeName(xdgDataHomeVar);
    } else {
        dataPaths << QDir::homePath() + "/.local/share";
    }
    QByteArray xdgDataDirsVar = qgetenv("XDG_DATA_DIRS");
    if (xdgDataDirsVar.isEmpty()) {
        xdgDataDirsVar = "/usr/local/share:/usr/share";
    }
    Q_FOREACH(const QByteArray& dir, xdgDataDirsVar.split(':')) {
        dataPaths << QFile::decodeName(dir);
    }
    QDir::setSearchPaths("xdgdata", dataPaths);

    // http://www.iana.org/assignments/media-types/index.html
    m_rootMimetypes << "application" << "audio" << "example"
                    << "image" << "message" << "model"
                    << "multipart" << "text" << "video";
    QFile mimeTypesFile("/etc/mime.types");
    if (!mimeTypesFile.open(QIODevice::ReadOnly)) {
        qWarning("Cannot open /etc/mime.types; full mimetype checking disabled");
    } else {
        QTextStream mimeStream(&mimeTypesFile);
        // assume /etc/mime.types doesn't have ridiculous lines
        QString line = mimeStream.readLine();
        QRegExp wsRegExp("\\s");
        while (!line.isNull()) {
            line = line.trimmed();
            if (!line.isEmpty() && !line.startsWith('#')) {
                int wsPos = line.indexOf(wsRegExp);
                if (wsPos > 0) {
                    line = line.left(wsPos);
                }
                m_mimeTypes.insert(line);
            }
            line = mimeStream.readLine();
        }
    }
}

RootInterfaceTest::~RootInterfaceTest()
{
}

void RootInterfaceTest::checkPropertyIdentity(const QVariantMap& oldProps)
{
    if (checkNonEmptyStringPropValid("Identity", oldProps)) {
        QString identity = props.value("Identity").toString();
        if (("org.mpris.MediaPlayer2." + identity) == iface->service()) {
            emit interfaceWarning(Property, "Identity", "Identity is the same as the service name (one is user-readable, the other is the 'internal' name)");
        }
    }
}

void RootInterfaceTest::checkPropertyDesktopEntry(const QVariantMap& oldProps)
{
    if (checkNonEmptyStringPropValid("DesktopEntry", oldProps)) {
        QString desktopEntry = props.value("DesktopEntry").toString();
        QFile file("xdgdata:applications/" + desktopEntry + ".desktop");
        if (!file.exists()) {
            file.setFileName("xdgdata:applications/" + desktopEntry.replace('-', '/') + ".desktop");
            if (!file.exists()) {
                emit interfaceWarning(Property, "DesktopEntry", "Could not find the desktop file");
                return;
            }
        }

        QSettings desktopFile(file.fileName(), QSettings::IniFormat);
        desktopFile.beginGroup("Desktop Entry");
        QString generalName = desktopFile.value("Name").toString();
        if (generalName.isEmpty()) {
            emit interfaceWarning(Property, "DesktopEntry", "Failed to read the Name entry of " + desktopFile.fileName());
        } else if (generalName != props.value("Identity").toString()) {
            // TODO: translated names?  http://standards.freedesktop.org/desktop-entry-spec/1.1/ar01s04.html
            emit interfaceWarning(Property, "DesktopEntry", "The Name entry of " + desktopFile.fileName() + " (" + generalName + ") is different from Identity");
        }
    }
}

void RootInterfaceTest::checkPropertySupportedUriSchemes(const QVariantMap& oldProps)
{
    if (checkPropValid("SupportedUriSchemes", QVariant::StringList, oldProps)) {
        QStringList uriSchemes = props.value("SupportedUriSchemes").toStringList();
        if (!uriSchemes.contains("file")) {
            emit interfaceWarning(Property, "SupportedUriSchemes", "\"file\" is not listed as a supported URI scheme (this is unusual)");
        }
        // TODO: check valid protocols
        // check duplicates?
    }
}

void RootInterfaceTest::checkPropertySupportedMimeTypes(const QVariantMap& oldProps)
{
    if (checkPropValid("SupportedMimeTypes", QVariant::StringList, oldProps)) {
        QStringList mimeTypes = props.value("SupportedMimeTypes").toStringList();
        if (mimeTypes.isEmpty()) {
            emit interfaceWarning(Property, "SupportedMimeTypes", "The media player claims not to support any mime types");
        }

        QMap<QString,int> seenCount;
        Q_FOREACH (const QString& mimeType, mimeTypes) {
            ++seenCount[mimeType];
            if (!m_mimeTypes.contains(mimeType)) {
                int slashIndex = mimeType.indexOf('/');
                if (slashIndex < 1) {
                    emit interfaceError(Property, "SupportedMimeTypes", "\"" + mimeType + "\" is not a valid mimetype");
                } else {
                    QString rootType = mimeType.left(slashIndex);
                    QString subType = mimeType.mid(slashIndex + 1);
                    if (!m_rootMimetypes.contains(rootType)) {
                        emit interfaceError(Property, "SupportedMimeTypes",
                                            "\"" + mimeType + "\" is not a valid mimetype (\""
                                            + rootType + "\" is not a valid content type)");
                    } else if (!m_mimeTypes.isEmpty() && !subType.startsWith("x-", Qt::CaseInsensitive)) {
                        emit interfaceWarning(Property, "SupportedMimeTypes",
                                            "\"" + mimeType + "\" is not a recognized mimetype");
                    }
                }
            }
        }

        QMap<QString,int>::ConstIterator it = seenCount.constBegin();
        for (; it != seenCount.constEnd(); ++it) {
            if (it.value() > 1) {
                emit interfaceWarning(Property, "SupportedMimeTypes",
                                    "\"" + it.key() + "\" appeared " + QString::number(it.value()) + " times");
            }
        }
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

void RootInterfaceTest::checkConsistency(const QVariantMap& oldProps)
{
}

// vim:et:sw=4:sts=4
