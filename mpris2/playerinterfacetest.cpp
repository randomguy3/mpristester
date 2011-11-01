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

#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>

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
