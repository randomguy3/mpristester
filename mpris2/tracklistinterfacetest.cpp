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
    QDBusReply<QList<QVariantMap> > reply = iface->call("GetTracksMetadata");
    if (!reply.isValid()) {
        emit interfaceError(Method, "GetTracksMetadata", "Call to GetTracksMetadata failed with error " + reply.error().message());
    } else {
        QList<QVariantMap> metadataList = reply.value();
        QMap<QDBusObjectPath,QVariantMap> metadataMap;
        int i = 0;
        Q_FOREACH (const QVariantMap& metadata, metadataList) {
            if (metadata.isEmpty()) {
                emit interfaceWarning(Method, "GetTracksMetadata",
                        "Got an empty entry at position " + QString::number(i));
            } else if (!metadata.contains("mpris:trackid")) {
                emit interfaceError(Method, "GetTracksMetadata",
                        "No mpris:trackid entry at position " + QString::number(i));
            } else if (metadata.value("mpris:trackid").userType() != qMetaTypeId<QDBusObjectPath>()) {
                emit interfaceError(Method, "GetTracksMetadata",
                        "mpris:trackid entry was not sent as a D-Bus object path (D-Bus type 'o') at position " + QString::number(i));
            } else {
                QDBusObjectPath trackid = metadata.value("mpris:trackid").value<QDBusObjectPath>();
                if (trackid.path().isEmpty()) {
                    emit interfaceError(Method, "GetTracksMetadata",
                            "mpris:trackid entry is an empty path at position " + QString::number(i));
                } else if (!trackIds.contains(trackid)) {
                    emit interfaceWarning(Method, "GetTracksMetadata",
                            "Entry " + trackid.path() + " was not requested at position " + QString::number(i));
                } else {
                    metadataMap.put(trackid, metadata);
                }
            }
            ++i;
        }
    }
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

// vim:et:sw=4:sts=4
