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
}

PlayerInterfaceTest::~PlayerInterfaceTest()
{
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
    checkPropValid("CanControl", QVariant::Bool, oldProps);
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
    checkPosition(oldProps);

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
    if (playbackStatus != "None" &&
        playbackStatus != "Track" &&
        playbackStatus != "Playlist")
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

void PlayerInterfaceTest::checkMetadata(const QVariantMap& oldProps)
{
    if (!properties().contains("Metadata")) {
        emit interfaceError(Property, "Metadata", "Property Metadata is missing");
        return;
    }
    if (!properties().value("Metadata").canConvert<QDBusArgument>()) {
        const char * gotTypeCh = QDBusMetaType::typeToSignature(props["Metadata"].userType());
        QString gotType = gotTypeCh ? QString::fromAscii(gotTypeCh) : "<unknown>";
        emit interfaceError(Property, "Metadata", "Property Metadata has type " + gotType + ", but should be type a{sv}");
        return;
    }
    QVariantMap metadata;
    QDBusArgument arg = properties().value("Metadata").value<QDBusArgument>();
    arg >> metadata;

    if (oldProps.contains("Metadata") &&
        oldProps.value("Metadata").canConvert<QDBusArgument>())
    {
        QVariantMap oldMetadata;
        oldProps.value("Metadata").value<QDBusArgument>() >> oldMetadata;
        if (metadata != oldMetadata) {
            outOfDateProperties.insert("Metadata", props["Metadata"]);
            props["Metadata"] = oldProps["Metadata"];
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
    } else if (metadata.value("mpris:trackid").type() != QVariant::String) {
        emit interfaceError(Property, "Metadata",
                            "mpris:trackid entry is not a string");
    } else if (metadata.value("mpris:trackid").toString().isEmpty()) {
        emit interfaceError(Property, "Metadata",
                            "mpris:trackid entry is an empty string");
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
            // FIXME: check network files
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

bool PlayerInterfaceTest::checkMetadataEntry(const QVariantMap& metadata, const QString& entry, QVariant::Type expType)
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

void PlayerInterfaceTest::checkPosition(const QVariantMap& oldProps)
{
    if (!checkPropValid("Position", QVariant::LongLong, oldProps))
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
    checkPredictedPosition();
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

void PlayerInterfaceTest::checkPredictedPosition()
{
    // FIXME: check predicted position
}

