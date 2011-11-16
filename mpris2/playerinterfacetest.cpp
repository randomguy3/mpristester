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


#include "playerinterfacetest.h"

#include <QtDBus>

#define MPRIS2_PLAYER_IFACE "org.mpris.MediaPlayer2.Player"

using namespace Mpris2;

PlayerInterfaceTest::PlayerInterfaceTest(const QString& service, QObject* parent)
    : InterfaceTest(MPRIS2_PLAYER_IFACE, service, parent)
{
    m_pos = -1;
    m_currentRate = 0.0;
    propsNotUpdated << "Position";
}

PlayerInterfaceTest::~PlayerInterfaceTest()
{
}

void PlayerInterfaceTest::connectSignals()
{
    QDBusConnection::sessionBus().connect(
            iface->service(),
            iface->path(),
            iface->interface(),
            "Seeked", /* signature, */
            this,
            SLOT( _m_seeked(qint64,QDBusMessage)));
}

void PlayerInterfaceTest::checkUpdatedProperty(const QString& propName)
{
    if (propName == "CanControl") {
        checkPropValid("CanControl", QVariant::Bool);
    } else if (propName == "CanGoNext") {
        checkControlProp("CanGoNext");
    } else if (propName == "CanGoPrevious") {
        checkControlProp("CanGoPrevious");
    } else if (propName == "CanPlay") {
        checkControlProp("CanPlay");
    } else if (propName == "CanPause") {
        checkControlProp("CanPause");
    } else if (propName == "CanSeek") {
        checkControlProp("CanSeek");
    } else if (propName == "Shuffle") {
        checkPropValid("Shuffle", QVariant::Bool);
    } else if (propName == "Volume") {
        checkVolume();
    } else if (propName == "PlaybackStatus") {
        checkPlaybackStatus();
    } else if (propName == "LoopStatus") {
        checkLoopStatus();
    } else if (propName == "MinimumRate") {
        checkMinimumRate();
    } else if (propName == "MaximumRate") {
        checkMaximumRate();
    } else if (propName == "Rate") {
        checkPropValid("Rate", QVariant::Double);
    } else if (propName == "Metadata") {
        checkMetadata();
    }
}

void PlayerInterfaceTest::checkProps(const QVariantMap& oldProps)
{
    // position is time-sensitive; check it first
    checkPosition(oldProps);
    checkPredictedPosition();

    if (checkPropValid("CanControl", QVariant::Bool) && oldProps.contains("CanControl")) {
    	bool newCanControl = props.value("CanControl").toBool();
    	bool oldCanControl = oldProps.value("CanControl").toBool();
	if (newCanControl != oldCanControl) {
	    emit interfaceError(Property, "CanControl",
			       "CanControl is an inherent property of the implementation, and should not change");
	}
    }
    checkControlProp("CanGoNext", oldProps);
    checkControlProp("CanGoPrevious", oldProps);
    checkControlProp("CanPlay", oldProps);
    checkControlProp("CanPause", oldProps);
    checkControlProp("CanSeek", oldProps);
    if (properties().contains("Shuffle")) {
        checkPropValid("Shuffle", QVariant::Bool, oldProps);
    } else {
        emit interfaceInfo(Property, "Shuffle",
                           "Optional property not implemented");
    }
    checkVolume(oldProps);
    checkPlaybackStatus(oldProps);
    if (properties().contains("LoopStatus")) {
        checkLoopStatus(oldProps);
    } else {
        emit interfaceInfo(Property, "LoopStatus",
                           "Optional property not implemented");
    }
    checkMinimumRate(oldProps);
    checkMaximumRate(oldProps);
    checkPropValid("Rate", QVariant::Double, oldProps);
    checkMetadata(oldProps);

    checkConsistency();
}

void PlayerInterfaceTest::checkControlProp(const QString& propName, const QVariantMap& oldProps)
{
    if (!checkPropValid(propName, QVariant::Bool, oldProps))
        return;
    bool canControl = properties().value("CanControl").toBool();
    if (!canControl && properties().value(propName).toBool()) {
        emit interfaceError(Property, propName,
                            "Value is true when CanControl is false");
    }
}

void PlayerInterfaceTest::checkVolume(const QVariantMap& oldProps)
{
    if (!checkPropValid("Volume", QVariant::Double, oldProps))
        return;
    if (properties().value("Volume").toDouble() < 0.0) {
        emit interfaceError(Property, "Volume",
                            "Volume cannot be negative");
    }
    if (properties().value("Volume").toDouble() > 1.0) {
        emit interfaceWarning(Property, "Volume",
                            "Volume is greater than 1.0");
    }
}

void PlayerInterfaceTest::checkLoopStatus(const QVariantMap& oldProps)
{
    if (!properties().contains("LoopStatus"))
        return;
    if (!checkPropValid("LoopStatus", QVariant::String, oldProps))
        return;
    QString loopStatus = properties().value("LoopStatus").toString();
    if (loopStatus != "None" &&
        loopStatus != "Track" &&
        loopStatus != "Playlist")
    {
        emit interfaceError(Property, "LoopStatus",
                            "Invalid value: '" + loopStatus + "'");
    }
}

void PlayerInterfaceTest::checkPlaybackStatus(const QVariantMap& oldProps)
{
    if (!checkPropValid("PlaybackStatus", QVariant::String, oldProps))
        return;
    QString playbackStatus = properties().value("PlaybackStatus").toString();
    if (playbackStatus != "Playing" &&
        playbackStatus != "Paused" &&
        playbackStatus != "Stopped")
    {
        emit interfaceError(Property, "PlaybackStatus",
                            "Invalid value: '" + playbackStatus + "'");
    }
}

void PlayerInterfaceTest::checkMaximumRate(const QVariantMap& oldProps)
{
    if (!checkPropValid("MaximumRate", QVariant::Double, oldProps))
        return;
    double maxRate = properties().value("MaximumRate").toDouble();
    if (maxRate < 1.0) {
        emit interfaceWarning(Property, "MaximumRate",
                              "Maximum rate should not be less than 1.0");
    }
}

void PlayerInterfaceTest::checkMinimumRate(const QVariantMap& oldProps)
{
    if (!checkPropValid("MinimumRate", QVariant::Double, oldProps))
        return;
    double minRate = properties().value("MinimumRate").toDouble();
    if (minRate > 1.0) {
        emit interfaceWarning(Property, "MinimumRate",
                              "Minimum rate should not be greater than 1.0");
    }
    if (minRate < 0.0) {
        emit interfaceInfo(Property, "MinimumRate",
                           "Minimum rate is negative");
    }
}

void PlayerInterfaceTest::checkRate(const QVariantMap& oldProps)
{
    if (!checkPropValid("Rate", QVariant::Double, oldProps))
        return;
    double rate = properties().value("Rate").toDouble();
    if (qFuzzyCompare(rate, 0.0)) {
        emit interfaceError(Property, "Rate",
                            "Rate must not be 0.0");
    }
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

void PlayerInterfaceTest::checkMetadata(const QVariantMap& oldProps)
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
                           "No metadata provided for the current track");
        return;
    }

    QStringList errors;
    QStringList warnings;
    QStringList infoMessages;
    InterfaceTest::checkMetadata(metadata, &errors, &warnings, &infoMessages);
    Q_FOREACH (const QString& message, errors) {
        emit interfaceError(Property, "Metadata", message);
    }
    Q_FOREACH (const QString& message, warnings) {
        emit interfaceWarning(Property, "Metadata", message);
    }
    Q_FOREACH (const QString& message, infoMessages) {
        emit interfaceInfo(Property, "Metadata", message);
    }
}

void PlayerInterfaceTest::checkPosition(const QVariantMap& oldProps)
{
    // don't pass in oldProps: Position doesn't get updated
    // automatically
    if (!checkPropValid("Position", QVariant::LongLong))
        return;

    qint64 position = properties().value("Position").toLongLong();
    if (position < 0) {
        emit interfaceError(Property, "Position",
                            "Negative position value");
    }
}

void PlayerInterfaceTest::checkConsistency(const QVariantMap& oldProps)
{
    checkRateConsistency();
    checkPositionConsistency();
}

void PlayerInterfaceTest::checkPositionConsistency(const QVariantMap& oldProps)
{
    if (!properties().contains("Position") ||
        !properties().contains("Metadata") ||
        !properties().value("Metadata").canConvert<QDBusArgument>()
    ) {
        return;
    }

    qint64 position = properties().value("Position").toLongLong();

    QVariantMap metadata;
    properties().value("Metadata").value<QDBusArgument>() >> metadata;
    if (!metadata.contains("mpris:length")) {
        return;
    }
    qint64 length = metadata.value("mpris:length").toLongLong();

    // drop out if the length and position have not changed
    if (oldProps.contains("Position") &&
        oldProps.value("Position") == position &&
        oldProps.contains("Metadata") &&
        oldProps.value("Metadata").canConvert<QDBusArgument>())
    {
        QVariantMap oldMetadata;
        oldProps.value("Metadata").value<QDBusArgument>() >> oldMetadata;
        if (oldMetadata.contains("mpris:length")) {
            if (length == oldMetadata.value("mpris:length").toLongLong()) {
                return;
            }
        }
    }

    if (position > length) {
        emit interfaceError(Property, "Position",
                            "Position is greater than the track length");
    }
}

void PlayerInterfaceTest::checkRateConsistency(const QVariantMap& oldProps)
{
    if (properties().value("MaximumRate") != oldProps.value("MaximumRate") ||
        properties().value("MinimumRate") != oldProps.value("MinimumRate"))
    {
        if (properties().contains("MaximumRate") && properties().contains("MinimumRate")) {
            double minRate = properties().value("MinimumRate").toDouble();
            double maxRate = properties().value("MaximumRate").toDouble();
            if (minRate > maxRate) {
                emit interfaceError(Property, "MinimumRate",
                                    "Minimum rate must not be greater than maximum rate");
            }
        }
    }
    if (properties().value("Rate") != oldProps.value("Rate") ||
        properties().value("MaximumRate") != oldProps.value("MaximumRate") ||
        properties().value("MinimumRate") != oldProps.value("MinimumRate"))
    {
        if (properties().contains("Rate")) {
            double rate = properties().value("Rate").toDouble();
            if (properties().contains("MaximumRate")) {
                double maxRate = properties().value("MaximumRate").toDouble();
                if (rate > maxRate) {
                    emit interfaceError(Property, "Rate",
                                        "Rate must not be greater than maximum rate");
                }
            }
            if (properties().contains("MinimumRate")) {
                double minRate = properties().value("MinimumRate").toDouble();
                if (minRate > rate) {
                    emit interfaceError(Property, "Rate",
                                        "Rate must not be less than minimum rate");
                }
            }
        }
    }
}

void PlayerInterfaceTest::updateCurrentRate()
{
    QString playbackStatus = properties().value("PlaybackStatus").toString();
    if (playbackStatus == "Playing") {
        m_currentRate = properties().value("Rate").toDouble();
    } else {
        m_currentRate = 0.0;
    }
}

void PlayerInterfaceTest::_m_seeked(qint64 position, const QDBusMessage& message)
{
    emit interfaceInfo(Signal, "Seeked", "Got Seeked(" + QString::number(position) + ") signal");
    m_pos = position;
    m_posLastUpdated = QTime::currentTime();
    props["Position"] = position;
    checkPosition();
    emit Seeked(position);
}

qint64 PlayerInterfaceTest::predictedPosition()
{
    qint64 elapsed = (qint64)m_posLastUpdated.elapsed() * 1000L;
    return m_pos + (m_currentRate * elapsed);
}

void PlayerInterfaceTest::checkPredictedPosition()
{
    qint64 position = properties().value("Position").toLongLong();

    // if this is the initial fetch
    if (m_pos == -1) {
        m_pos = position;
        m_posLastUpdated = QTime::currentTime();
        updateCurrentRate();
        return;
    }

    qint64 predictedPos = predictedPosition();
    m_pos = position;
    m_posLastUpdated = QTime::currentTime();
    updateCurrentRate();

    qint64 diffMillis = (position - predictedPos) / 1000;
    // allow 1s of error
    const qint64 allowanceMillis = 1000;
    if (diffMillis > allowanceMillis ||
        diffMillis < -allowanceMillis)
    {
        qreal diffSecs = ((qreal)diffMillis) / 1000.0;
        qreal predictedPosSecs = ((qreal)(predictedPos/1000)) / 1000.0;
        qreal positionSecs = ((qreal)(position/1000)) / 1000.0;
        if (diffMillis > 0) {
            emit interfaceWarning(Property, "Position",
                                  "Position (" +
                                  QString::number(positionSecs, 'f', 2) +
                                  "s) is " +
                                  QString::number(diffSecs, 'f', 2) +
                                  "s ahead of what was predicted from Rate (" +
                                  QString::number(predictedPosSecs, 'f', 2) +
                                  "s)");
        } else {
            emit interfaceWarning(Property, "Position",
                                  "Position (" +
                                  QString::number(positionSecs, 'f', 2) +
                                  "s) is " +
                                  QString::number(-diffSecs, 'f', 2) +
                                  "s ahead of what was predicted from Rate (" +
                                  QString::number(predictedPosSecs, 'f', 2) +
                                  "s)");
        }
    }
}

void PlayerInterfaceTest::testNext()
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

void PlayerInterfaceTest::testPrevious()
{
    QDBusReply<void> reply = iface->call("Previous");
    if (!reply.isValid()) {
        emit interfaceError(Method, "Previous", "Call to Previous failed with error " + reply.error().message());
    } else {
        if (!props["CanGoNext"].toBool()) {
            emit interfaceInfo(Method, "Previous", "Previous called, but CanGoPrevious is false, so this should have no effect");
            // TODO: check to see that the track does not change in the next second or so
        } else {
            emit interfaceInfo(Method, "Previous", "Previous called; the media player should now move to the next track");
            // TODO: check to see if the track changes
            // TODO: check to make sure the PlaybackStatus does not change
        }
    }
}

void PlayerInterfaceTest::testPause()
{
    QDBusReply<void> reply = iface->call("Pause");
    if (!reply.isValid()) {
        emit interfaceError(Method, "Pause", "Call to Pause failed with error " + reply.error().message());
    } else {
        if (!props["CanPause"].toBool()) {
            emit interfaceInfo(Method, "Pause", "Pause called, but CanPause is false, so this should have no effect");
            // TODO: check to see that the PlaybackStatus does not change
        } else if (props["PlaybackStatus"] == "Paused") {
            emit interfaceInfo(Method, "Pause", "Pause called, but already paused, so this should have no effect");
            // TODO: check to see that the PlaybackStatus does not change
        } else {
            emit interfaceInfo(Method, "Pause", "Pause called; the media player should now be paused");
            // TODO: check to see if the PlaybackStatus changes to Paused
        }
    }
}

void PlayerInterfaceTest::testPlayPause()
{
    QDBusReply<void> reply = iface->call("PlayPause");
    if (!reply.isValid()) {
        emit interfaceError(Method, "PlayPause", "Call to PlayPause failed with error " + reply.error().message());
    } else {
        if (!props["CanPause"].toBool()) {
            emit interfaceInfo(Method, "PlayPause", "PlayPause called, but CanPause is false, so this should have no effect");
            // TODO: check to see that the PlaybackStatus does not change
        } else if (props["PlaybackStatus"] == "Playing") {
            emit interfaceInfo(Method, "PlayPause", "PlayPause called; the media player should now be paused");
            // TODO: check to see if the PlaybackStatus changes to Paused
        } else {
            emit interfaceInfo(Method, "PlayPause", "PlayPause called; the media player should now be playing");
            // TODO: check to see if the PlaybackStatus changes to Playing
        }
    }
}

void PlayerInterfaceTest::testPlay()
{
    QDBusReply<void> reply = iface->call("Play");
    if (!reply.isValid()) {
        emit interfaceError(Method, "Play", "Call to Play failed with error " + reply.error().message());
    } else {
        if (!props["CanPlay"].toBool()) {
            emit interfaceInfo(Method, "Play", "Play called, but CanPlay is false, so this should have no effect");
            // TODO: check to see that the PlaybackStatus does not change
        } else if (props["PlaybackStatus"] == "Playing") {
            emit interfaceInfo(Method, "Play", "Play called, but already playing, so this should have no effect");
            // TODO: check to see that the PlaybackStatus does not change
        } else {
            emit interfaceInfo(Method, "Play", "Play called; the media player should start playback");
            // TODO: check to see if the PlaybackStatus changes to Playing
        }
    }
}

void PlayerInterfaceTest::testStop()
{
    QDBusReply<void> reply = iface->call("Stop");
    if (!reply.isValid()) {
        emit interfaceError(Method, "Stop", "Call to Stop failed with error " + reply.error().message());
    } else {
        if (!props["CanControl"].toBool()) {
            emit interfaceInfo(Method, "Stop", "Stop called, but CanControl is false, so this should have no effect");
            // TODO: check to see that the PlaybackStatus does not change
        } else if (props["PlaybackStatus"] == "Stopped") {
            emit interfaceInfo(Method, "Stop", "Stop called, but already stopped, so this should have no effect");
            // TODO: check to see that the PlaybackStatus does not change
        } else {
            emit interfaceInfo(Method, "Stop", "Stop called; the media player should now stop");
            // TODO: check to see if the PlaybackStatus changes to Stopped
        }
    }
}

void PlayerInterfaceTest::testSeek(qint64 offset)
{
    QDBusReply<void> reply = iface->call("Seek", QVariant::fromValue<qint64>(offset));
    if (!reply.isValid()) {
        emit interfaceError(Method, "Seek", "Call to Seek failed with error " + reply.error().message());
    } else {
        if (!props["CanSeek"].toBool()) {
            emit interfaceInfo(Method, "Seek", "Seek called, but CanSeek is false, so this should have no effect");
            // TODO: check to see that Seeked is not emitted; PlaybackStatus does not change
        } else {
            qint64 newPos = predictedPosition() + offset;
            if (newPos < 0.0) {
                emit interfaceInfo(Method, "Seek", "Seek called with a value that moved beyond the start of the track; the media player should be at the start of the track");
                // TODO: check to see that Seeked is emitted with value 0 (fuzzy)
            } else if (props.contains("mpris:length") &&
                       props["mpris:length"].toLongLong() <= newPos) {
                if (!props["CanGoNext"].toBool()) {
                    emit interfaceInfo(Method, "Seek", "Seek called with a value that would move beyond the end of the track, but CanGoNext is false, so playback should stop");
                    // TODO: check to see that PlaybackStatus changes to Stopped
                } else {
                    emit interfaceInfo(Method, "Seek", "Seek called with a value that would move beyond the end of the track; the media player should now move to the next track");
                    // TODO: check to see if the track changes
                    // TODO: check to make sure the PlaybackStatus does not change
                }
            } else {
                emit interfaceInfo(Method, "Seek", "Seek called; the media player should seek to " +
                        QString::number(newPos));
                // TODO: check that Seeked is emitted with the relevant value (fuzzy)
            }
        }
    }
}

void PlayerInterfaceTest::testSetPosition(const QDBusObjectPath& trackId, qint64 position)
{
    QDBusReply<void> reply = iface->call("SetPosition",
                                         QVariant::fromValue<QDBusObjectPath>(trackId),
                                         QVariant::fromValue<qint64>(position));
    if (!reply.isValid()) {
        emit interfaceError(Method, "SetPosition", "Call to SetPosition failed with error " + reply.error().message());
    } else {
        if (!props["CanSeek"].toBool()) {
            emit interfaceInfo(Method, "SetPosition", "SetPosition called, but CanSeek is false, so this should have no effect");
            // TODO: check to see that Seeked is not emitted; PlaybackStatus does not change
        } else {
            if (trackId.path() != props["Metadata"].toMap()["mpris:trackid"].value<QDBusObjectPath>().path()) {
                emit interfaceInfo(Method, "SetPosition", "SetPosition called with the wrong trackid ('" +
                                   trackId.path() +
                                   "'; expecting '" +
                                   props["mpris:trackid"].value<QDBusObjectPath>().path() +
                                   "'); nothing should happen");
                // TODO: check to see that Seeked is not emitted; PlaybackStatus does not change
            } else if (position < 0.0) {
                emit interfaceInfo(Method, "SetPosition", "SetPosition called with a negative value; the media player should be at the start of the track");
                // TODO: check to see that Seeked is emitted with value 0
            } else if (props.contains("mpris:length") &&
                       props["mpris:length"].toLongLong() <= position) {
                if (!props["CanGoNext"].toBool()) {
                    emit interfaceInfo(Method, "SetPosition", "SetPosition called with a value that would move beyond the end of the track, but CanGoNext is false, so playback should stop");
                    // TODO: check to see that PlaybackStatus changes to Stopped
                } else {
                    emit interfaceInfo(Method, "SetPosition", "SetPosition called with a value that would move beyond the end of the track; the media player should now move to the next track");
                    // TODO: check to see if the track changes
                    // TODO: check to make sure the PlaybackStatus does not change
                }
            } else {
                emit interfaceInfo(Method, "SetPosition", "SetPosition called; the media player should seek to the new position");
                // TODO: check that Seeked is emitted with the relevant value
            }
        }
    }
}

void PlayerInterfaceTest::testOpenUri(const QString& uri)
{
    QDBusReply<void> reply = iface->call("OpenUri",
                                         QVariant::fromValue<QString>(uri));
    if (!reply.isValid()) {
        emit interfaceInfo(Method, "OpenUri", "Call to OpenUri failed with error " + reply.error().message());
        // TODO: check SupportedUriSchemes - if empty, this method may be unimplemented
        //       if uri scheme is not in the list of schemes or not a valid uri, argument
        //       error may be returned
    } else {
        emit interfaceInfo(Method, "OpenUri", "Call to OpenUri did not return an error");
        // ?
    }
}

void PlayerInterfaceTest::testSetLoopStatus(const QString& loopStatus)
{
    // FIXME: look at whether value is valid
    if (setProp("LoopStatus", QDBusVariant(QVariant(loopStatus)))) {
        emit interfaceInfo(Property, "LoopStatus", "Setting LoopStatus did not return an error");
    }
}

void PlayerInterfaceTest::testSetShuffle(bool shuffle)
{
    if (setProp("Shuffle", QDBusVariant(QVariant(shuffle)))) {
        emit interfaceInfo(Property, "Shuffle", "Setting Shuffle did not return an error");
    }
}

void PlayerInterfaceTest::testSetVolume(double volume)
{
    // FIXME: look at whether volume is out of bounds
    if (setProp("Volume", QDBusVariant(QVariant(volume)))) {
        emit interfaceInfo(Property, "Volume", "Setting Volume did not return an error");
    }
}

void PlayerInterfaceTest::testSetRate(double rate)
{
    // FIXME: look at whether rate is out of bounds
    if (setProp("Rate", QDBusVariant(QVariant(rate)))) {
        emit interfaceInfo(Property, "Rate", "Setting Rate did not return an error");
    }
}

// vim:et:sw=4:sts=4
