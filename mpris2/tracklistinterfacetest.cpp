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


#include "tracklistinterfacetest.h"

#include <QtDBus>

#define MPRIS2_TRACKLIST_IFACE "org.mpris.MediaTrackList2.TrackList"

using namespace Mpris2;

TrackListInterfaceTest::TrackListInterfaceTest(const QString& service, QObject* parent)
    : InterfaceTest(MPRIS2_TRACKLIST_IFACE, service, parent)
{
}

TrackListInterfaceTest::~TrackListInterfaceTest()
{
}

void TrackListInterfaceTest::connectSignals()
{
    QDBusConnection::sessionBus().connect(
            iface->service(),
            iface->path(),
            iface->interface(),
            "TrackListReplaced", /* signature, */
            this,
            SLOT(_m_trackListReplaced(QList<QDBusObjectPath>,QDBusObjectPath)));
    QDBusConnection::sessionBus().connect(
            iface->service(),
            iface->path(),
            iface->interface(),
            "TrackAdded", /* signature, */
            this,
            SLOT(_m_trackAdded(QVariantMap,QDBusObjectPath)));
    QDBusConnection::sessionBus().connect(
            iface->service(),
            iface->path(),
            iface->interface(),
            "TrackRemoved", /* signature, */
            this,
            SLOT(_m_trackRemoved(QDBusObjectPath)));
    QDBusConnection::sessionBus().connect(
            iface->service(),
            iface->path(),
            iface->interface(),
            "TrackMetadataChanged", /* signature, */
            this,
            SLOT(_m_trackMetadataChanged(QDBusObjectPath,QVariantMap)));
}

void TrackListInterfaceTest::checkUpdatedProperty(const QString& propName)
{
    if (propName == "CanEditTracks") {
        checkPropValid("CanEditTracks", QVariant::Bool);
    } else if (propName == "Tracks") {
        checkTracks();
    }
}

void TrackListInterfaceTest::checkProps(const QVariantMap& oldProps)
{
    checkPropValid("CanEditTracks", QVariant::Bool);
    checkTracks();
}

void TrackListInterfaceTest::checkTracks(const QVariantMap& oldProps)
{
}

static bool compare(const QVariantMap& one, const QVariantMap& other)
{
    if (one.size() != other.size())
        return false;

    QVariantMap::const_iterator it1 = one.begin();
    QVariantMap::const_iterator it2 = other.begin();

    while (it1 != one.end()) {
        if (it1.value().userType() != it2.value().userType())
            return false;
        if (!(it1.value() == it2.value())) {
            if (it1.value().userType() == qMetaTypeId<QDBusObjectPath>()) {
                if (!(it1.value().value<QDBusObjectPath>() == it2.value().value<QDBusObjectPath>()))
                    return false;
            }
        }
        if (qMapLessThanKey(it1.key(), it2.key()) || qMapLessThanKey(it2.key(), it1.key()))
            return false;
        ++it2;
        ++it1;
    }
    return true;
}

void TrackListInterfaceTest::checkMetadata(const QVariantMap& oldProps)
{
    if (!props.contains("Metadata")) {
        emit interfaceError(Property, "Metadata", "Property Metadata is missing");
        return;
    }
    if (props["Metadata"].type() != QVariant::Map &&
        !props.value("Metadata").canConvert<QDBusArgument>())
    {
        const char * gotTypeCh = QDBusMetaType::typeToSignature(props["Metadata"].userType());
        QString gotType = gotTypeCh ? QString::fromAscii(gotTypeCh) : "<unknown>";
        emit interfaceError(Property, "Metadata", "Property Metadata has type " + gotType + ", but should be type a{sv}");
        return;
    }
    QVariantMap metadata;
    if (props["Metadata"].type() == QVariant::Map) {
        metadata = props["Metadata"].toMap();
    } else {
        QDBusArgument arg = props["Metadata"].value<QDBusArgument>();
        arg >> metadata;
        // replace the entry in the properties array
        props["Metadata"] = metadata;
    }

    if (oldProps.contains("Metadata") &&
        oldProps.value("Metadata").canConvert(QVariant::Map))
    {
        QVariantMap oldMetadata = oldProps.value("Metadata").toMap();

        // custom compare fn as we're expecting a QDBusObjectPath entry
        if (!compare(metadata, oldMetadata)) {
            outOfDateProperties["Metadata"] = props["Metadata"];
            props["Metadata"] = oldProps["Metadata"];
            return;
        } else {
            // same as before; don't re-run checks
            return;
        }
    }
    if (metadata.isEmpty()) {
        emit interfaceInfo(Property, "Metadata",
                           "No metadata provided");
        return;
    }

    if (!metadata.contains("mpris:trackid")) {
        emit interfaceError(Property, "Metadata",
                            "No mpris:trackid entry for the current track");
    } else if (metadata.value("mpris:trackid").userType() != qMetaTypeId<QDBusObjectPath>()) {
        emit interfaceError(Property, "Metadata",
                            "mpris:trackid entry was not sent as a D-Bus object path (D-Bus type 'o')");
    } else if (metadata.value("mpris:trackid").value<QDBusObjectPath>().path().isEmpty()) {
        emit interfaceError(Property, "Metadata",
                            "mpris:trackid entry is an empty path");
    }

    checkMetadataEntry(metadata, "mpris:length", QVariant::LongLong);

    if (checkMetadataEntry(metadata, "mpris:artUrl", QVariant::Url)) {
        QString artUrl = metadata.value("mpris:artUrl").toString();
        QUrl asUrl(artUrl, QUrl::StrictMode);
        if (asUrl.scheme() != "file" && asUrl.scheme() != "http" && asUrl.scheme() != "https") {
            emit interfaceInfo(Property, "Metadata",
                                "mpris:artUrl has a scheme (" + asUrl.scheme() + ") which not all clients may recognise");
        } else {
            if (asUrl.scheme() == "file") {
                if (!QFile::exists(asUrl.toLocalFile())) {
                    emit interfaceInfo(Property, "Metadata",
                                        "mpris:artUrl references a file that does not exist");
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
                emit interfaceWarning(Property, "Metadata",
                                      "Unrecognised entry " + key);
            }
        }
    }

    checkMetadataEntry(metadata, "xesam:album", QVariant::String);
    checkMetadataEntry(metadata, "xesam:albumArtist", QVariant::StringList);
    checkMetadataEntry(metadata, "xesam:artist", QVariant::StringList);
    checkMetadataEntry(metadata, "xesam:asText", QVariant::String);
    checkMetadataEntry(metadata, "xesam:audioBpm", QVariant::Int);
    checkMetadataEntry(metadata, "xesam:autoRating", QVariant::Double);
    checkMetadataEntry(metadata, "xesam:comment", QVariant::StringList);
    checkMetadataEntry(metadata, "xesam:composer", QVariant::StringList);
    checkMetadataEntry(metadata, "xesam:contentCreator", QVariant::DateTime);
    checkMetadataEntry(metadata, "xesam:discNumber", QVariant::Int);
    checkMetadataEntry(metadata, "xesam:firstUsed", QVariant::DateTime);
    checkMetadataEntry(metadata, "xesam:genre", QVariant::StringList);
    checkMetadataEntry(metadata, "xesam:lastUsed", QVariant::DateTime);
    checkMetadataEntry(metadata, "xesam:lyricist", QVariant::StringList);
    checkMetadataEntry(metadata, "xesam:title", QVariant::String);
    checkMetadataEntry(metadata, "xesam:trackNumber", QVariant::Int);
    checkMetadataEntry(metadata, "xesam:url", QVariant::Url);
    checkMetadataEntry(metadata, "xesam:useCount", QVariant::Int);
    checkMetadataEntry(metadata, "xesam:userRating", QVariant::Double);
}

bool TrackListInterfaceTest::checkMetadataEntry(const QVariantMap& metadata, const QString& entry, QVariant::Type expType)
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
                emit interfaceError(Property, "Metadata",
                                    entry + " entry is of type '" + gotType + "' but should have been of type '" + expType + "'");
                return false;
            } else {
                emit interfaceWarning(Property, "Metadata",
                                      entry + " entry is of type '" + gotType + "' but should have been of type '" + expType + "'");
                return true;
            }
        }

        // extra checks for special types
        if (expType == QVariant::DateTime) {
            QDateTime dtValue = QDateTime::fromString(value.toString(), Qt::ISODate);
            if (!dtValue.isValid()) {
                emit interfaceError(Property, "Metadata",
                                    entry + " entry does not contain a valid date/time string (value was " + value.toString() + ")");
                return false;
            }
        } else if (expType == QVariant::Url) {
            if (value.toString().isEmpty()) {
                return false;
            } else {
                QUrl asUrl(value.toString(), QUrl::StrictMode);
                if (!asUrl.isValid()) {
                    emit interfaceError(Property, "Metadata",
                                        entry + " entry is not a valid URL");
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

void TrackListInterfaceTest::_m_trackListReplaced(const QList<QDBusObjectPath>& trackIds, const QDBusObjectPath& currentPath)
{
}

void TrackListInterfaceTest::_m_trackAdded(const QVariantMap& metadata, const QDBusObjectPath& afterTrack)
{
}

void TrackListInterfaceTest::_m_trackRemoved(const QDBusObjectPath& afterTrack)
{
}

void TrackListInterfaceTest::_m_trackMetadataChanged(const QDBusObjectPath& track, const QVariantMap& metadata)
{
}


/*
void TrackListInterfaceTest::testNext()
{
    QDBusReply<void> reply = iface->call("Next");
    if (!reply.isValid()) {
        emit interfaceError(Method, "Next", "Call to Next failed with error " + reply.error().message());
    } else {
        if (!props["CanGoNext"].toBool()) {
            emit interfaceInfo(Method, "Next", "Next called, but CanGoNext is false, so this should have no effect");
            // TODO: check to see that the track does not change in the next second or so
        } else {
            emit interfaceInfo(Method, "Next", "Next called; the media player should now move to the next track");
            // TODO: check to see if the track changes
            // TODO: check to make sure the PlaybackStatus does not change
        }
    }
}
*/

QList<QVariantMap> TrackListInterfaceTest::testGetTracksMetadata(const QList<QDBusObjectPath>& trackIds)
{
}

void TrackListInterfaceTest::testAddTrack(const QString& uri, const QDBusObjectPath& afterTrack, bool setAsCurrent)
{
}

void TrackListInterfaceTest::testRemoveTrack(const QDBusObjectPath& track)
{
}

void TrackListInterfaceTest::testGoTo(const QDBusObjectPath& track)
{
}

