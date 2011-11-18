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


#ifndef MPRIS2_TRACKLISTINTERFACETEST_H
#define MPRIS2_TRACKLISTINTERFACETEST_H

#include "interfacetest.h"
#include <QDBusObjectPath>
#include <QTime>
#include <QList>
#include <QVariantMap>

namespace Mpris2 {
    class TrackListInterfaceTest : public InterfaceTest
    {
        Q_OBJECT

    public:
        TrackListInterfaceTest(const QString& service, QObject* parent = 0);
        virtual ~TrackListInterfaceTest();

    public slots:
        QList<QVariantMap> testGetTracksMetadata(const QList<QDBusObjectPath>& trackIds);
        void testAddTrack(const QString& uri, const QDBusObjectPath& afterTrack, bool setAsCurrent);
        void testRemoveTrack(const QDBusObjectPath& track);
        void testGoTo(const QDBusObjectPath& track);

    signals:
        void TrackListReplaced(const QList<QDBusObjectPath>& trackIds, const QDBusObjectPath& currentPath);
        void TrackAdded(const QVariantMap& metadata, const QDBusObjectPath& afterTrack);
        void TrackRemoved(const QDBusObjectPath& afterTrack);
        void TrackMetadataChanged(const QDBusObjectPath& track, const QVariantMap& metadata);

    protected:
        virtual void checkUpdatedProperty(const QString& propName);
        virtual void checkProps(const QVariantMap& oldProps = QVariantMap());
        virtual void checkConsistency(const QVariantMap& oldProps = QVariantMap());
        virtual void connectSignals();

    private slots:
        void _m_trackListReplaced(const QList<QDBusObjectPath>& trackIds, const QDBusObjectPath& currentPath);
        void _m_trackAdded(const QVariantMap& metadata, const QDBusObjectPath& afterTrack);
        void _m_trackRemoved(const QDBusObjectPath& afterTrack);
        void _m_trackMetadataChanged(const QDBusObjectPath& track, const QVariantMap& metadata);

    private:
        void checkTracks(const QVariantMap& oldProps = QVariantMap());
    };
}

#endif // MPRIS2_TRACKLISTINTERFACETEST_H
// vim:et:sw=4:sts=4
