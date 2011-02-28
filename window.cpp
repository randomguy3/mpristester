/*
 * Copyright 2008  Alex Merry <alex.merry@kdemail.net>
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

#include "window.h"

#include "dbus/mprisroot.h"
#include "dbus/mprisplayer.h"
#include "dbus/mpristracklist.h"

#include <QDBusConnection>
#include <QAction>
#include <QActionGroup>

Window::Window(QWidget* parent)
    : QMainWindow(parent),
      m_mprisRoot(0),
      m_mprisPlayer(0),
      m_mprisTracklist(0),
      m_playerActionGroup(new QActionGroup(this))
{
    m_ui.setupUi(this);
    m_ui.metadataView->setModel(&m_currentTrackModel);
    m_ui.tracklistView->setModel(&m_tracklistModel);

    connect(m_ui.action_Quit, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(m_playerActionGroup, SIGNAL(triggered(QAction*)),
            this, SLOT(changePlayer(QAction*)));

    m_timer.setInterval(500);
    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(updateChangables()));

    connect(m_ui.quitButton, SIGNAL(clicked()),
            this, SLOT(quitPlayer()));

    connect(m_ui.playButton, SIGNAL(clicked()),
            this, SLOT(play()));
    connect(m_ui.pauseButton, SIGNAL(clicked()),
            this, SLOT(pause()));
    connect(m_ui.stopButton, SIGNAL(clicked()),
            this, SLOT(stop()));
    connect(m_ui.prevButton, SIGNAL(clicked()),
            this, SLOT(prev()));
    connect(m_ui.nextButton, SIGNAL(clicked()),
            this, SLOT(next()));
    connect(m_ui.randomOnButton, SIGNAL(clicked()),
            this, SLOT(randomOn()));
    connect(m_ui.randomOffButton, SIGNAL(clicked()),
            this, SLOT(randomOff()));
    connect(m_ui.trackRepeatOnButton, SIGNAL(clicked()),
            this, SLOT(trackRepeatOn()));
    connect(m_ui.trackRepeatOffButton, SIGNAL(clicked()),
            this, SLOT(trackRepeatOff()));
    connect(m_ui.playlistRepeatOnButton, SIGNAL(clicked()),
            this, SLOT(playlistRepeatOn()));
    connect(m_ui.playlistRepeatOffButton, SIGNAL(clicked()),
            this, SLOT(playlistRepeatOff()));

    connect(m_ui.setPositionButton, SIGNAL(clicked()),
            this, SLOT(setPosition()));

    connect(m_ui.setVolumeButton, SIGNAL(clicked()),
            this, SLOT(setVolume()));

    connect(m_ui.fetchMetadataButton, SIGNAL(clicked()),
            this, SLOT(fetchTrackMetadata()));
    connect(m_ui.addTrackButton, SIGNAL(clicked()),
            this, SLOT(addTrack()));
    connect(m_ui.deleteTrackButton, SIGNAL(clicked()),
            this, SLOT(deleteTrack()));

    connect(m_ui.actionForce_metadata_update, SIGNAL(triggered()),
            this, SLOT(forceMetadataUpdate()));
    connect(m_ui.actionForce_capabilities_update, SIGNAL(triggered()),
            this, SLOT(forceCapsUpdate()));
    connect(m_ui.actionForce_status_update, SIGNAL(triggered()),
            this, SLOT(forceStatusUpdate()));
    connect(m_ui.actionForce_tracklist_update, SIGNAL(triggered()),
            this, SLOT(forceTracklistUpdate()));

    QDBusConnection sessionConn = QDBusConnection::sessionBus();
    if (sessionConn.isConnected()) {
        QDBusConnectionInterface* bus = sessionConn.interface();
        connect(bus, SIGNAL(serviceOwnerChanged(QString,QString,QString)),
                this,  SLOT(serviceChange(QString,QString,QString)));

        QDBusReply<QStringList> reply = bus->registeredServiceNames();
        if (reply.isValid()) {
            QStringList services = reply.value();
            foreach (const QString& name, services) {
                if (name.startsWith("org.mpris.")) {
                    serviceChange(name, QString(), "dummy");
                }
            }
        } else {
            consoleMessage("Couldn't get service names: " + reply.error().message());
        }
    } else {
        consoleMessage("Could not connect to session bus");
    }
}

void Window::serviceChange(const QString& name,
                           const QString& oldOwner,
                           const QString& newOwner)
{
    if (!name.startsWith("org.mpris.")) {
        return;
    }
    if (oldOwner.isEmpty() && !newOwner.isEmpty()) {
        // new player
        m_ui.actionNo_players_found->setVisible(false);
        QAction* action = m_ui.menuPlayers->addAction(name);
        action->setActionGroup(m_playerActionGroup);
        m_playerActions[name] = action;
    } else if (!oldOwner.isEmpty() && newOwner.isEmpty()) {
        // player disappeared
        if (m_playerActions.contains(name)) {
            if (name == m_currentPlayer) {
                clear();
                consoleMessage(tr("Player %1 disappeared").arg(name));
            }
            m_playerActionGroup->removeAction(m_playerActions[name]);
            delete m_playerActions[name];
            m_playerActions.remove(name);
        } else {
            consoleMessage(tr("Player %1 disappeared, but we didn't know about it").arg(name));
        }
        if (m_playerActions.isEmpty()) {
            m_ui.actionNo_players_found->setVisible(true);
        }
    }
}

void Window::changePlayer(QAction* action)
{
    if (action->text() != m_currentPlayer) {
        setPlayer(action->text());
    }
}

void Window::updateChangables() // slot
{
    if (m_mprisPlayer) {
        QDBusReply<int> position = m_mprisPlayer->PositionGet();
        QDBusReply<int> volume = m_mprisPlayer->VolumeGet();
        if (position.isValid()) {
            m_ui.currentPosition->setText(QString::number(position));
        } else {
            printDBusError(position.error());
        }
        if (volume.isValid()) {
            m_ui.currentVolume->setText(QString::number(volume));
        } else {
            printDBusError(volume.error());
        }
    }
}

static void setCap(QLabel* label,
                   bool enabled,
                   const QString& enabledText,
                   const QString& disabledText)
{
    label->setEnabled(enabled);
    label->setText(enabled ? enabledText : disabledText);
}

void Window::setCaps(int caps) // slot
{
    ::setCap(m_ui.capGoNext, (caps & Mpris::CAN_GO_NEXT),
                             tr("Can go next"),
                             tr("Cannot go next"));
    ::setCap(m_ui.capGoPrev, (caps & Mpris::CAN_GO_PREV),
                             tr("Can go previous"),
                             tr("Cannot go previous"));
    ::setCap(m_ui.capPause, (caps & Mpris::CAN_PAUSE),
                            tr("Can pause"),
                            tr("Cannot pause"));
    ::setCap(m_ui.capPlay, (caps & Mpris::CAN_PLAY),
                           tr("Can play"),
                           tr("Cannot play"));
    ::setCap(m_ui.capSeek, (caps & Mpris::CAN_SEEK),
                           tr("Can seek"),
                           tr("Cannot seek"));
    ::setCap(m_ui.capMetadata, (caps & Mpris::CAN_PROVIDE_METADATA),
                               tr("Can provide metadata"),
                               tr("Cannot provide metadata"));
    ::setCap(m_ui.capTracklist, (caps & Mpris::CAN_HAS_TRACKLIST),
                                tr("Has tracklist"),
                                tr("No tracklist"));

    const int excess = (caps | Mpris::ALL_KNOWN_CAPS) - Mpris::ALL_KNOWN_CAPS;
    m_ui.capsExcess->setText(QString::number(excess));
}

void Window::capsChangeNotify(int caps) // slot
{
    consoleMessage(tr("CapsChange(%1) signal received").arg(QString::number(caps)));
}

void Window::setStatus(Mpris::Status status) // slot
{
    switch (status.play) {
        case Mpris::Status::Playing:
            m_ui.statusPlaying->setText(tr("Playing"));
            break;
        case Mpris::Status::Paused:
            m_ui.statusPlaying->setText(tr("Paused"));
            break;
        case Mpris::Status::Stopped:
            m_ui.statusPlaying->setText(tr("Stopped"));
            break;
        default:
            m_ui.statusPlaying->setText(tr("Invalid play mode"));
    }
    switch (status.random) {
        case Mpris::Status::Linear:
            m_ui.statusRandom->setText(tr("Linear play"));
            break;
        case Mpris::Status::Random:
            m_ui.statusRandom->setText(tr("Random play"));
            break;
        default:
            m_ui.statusRandom->setText(tr("Invalid random mode"));
    }
    switch (status.trackRepeat) {
        case Mpris::Status::GoToNext:
            m_ui.statusTrackRepeat->setText(tr("Go to next track"));
            break;
        case Mpris::Status::RepeatCurrent:
            m_ui.statusTrackRepeat->setText(tr("Repeat track"));
            break;
        default:
            m_ui.statusTrackRepeat->setText(tr("Invalid track repeat mode"));
    }
    switch (status.playlistRepeat) {
        case Mpris::Status::StopWhenFinished:
            m_ui.statusPlaylistRepeat->setText(tr("Stop at end of playlist"));
            break;
        case Mpris::Status::RepeatCurrent:
            m_ui.statusPlaylistRepeat->setText(tr("Play forever"));
            break;
        default:
            m_ui.statusPlaylistRepeat->setText(tr("Invalid playlist repeat mode"));
    }
}

void Window::statusChangeNotify(Mpris::Status status) // slot
{
    consoleMessage(tr("StatusChange(%1-%2-%3-%4) signal received")
                   .arg(QString::number(status.play))
                   .arg(QString::number(status.random))
                   .arg(QString::number(status.trackRepeat))
                   .arg(QString::number(status.playlistRepeat)));
}

void Window::trackChangeNotify(const QVariantMap&) // slot
{
    consoleMessage(tr("TrackChange([new track metadata]) signal received"));
}

void Window::trackListChangeNotify(int length) // slot
{
    consoleMessage(tr("TrackListChange(%1) signal received").arg(QString::number(length)));
}

void Window::quitPlayer()
{
    if (m_mprisRoot) {
        QDBusReply<void> reply = m_mprisRoot->Quit();
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::play()
{
    if (m_mprisPlayer) {
        QDBusReply<void> reply = m_mprisPlayer->Play();
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::pause()
{
    if (m_mprisPlayer) {
        QDBusReply<void> reply = m_mprisPlayer->Pause();
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::stop()
{
    if (m_mprisPlayer) {
        QDBusReply<void> reply = m_mprisPlayer->Stop();
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::prev()
{
    if (m_mprisPlayer) {
        QDBusReply<void> reply = m_mprisPlayer->Prev();
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::next()
{
    if (m_mprisPlayer) {
        QDBusReply<void> reply = m_mprisPlayer->Next();
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::randomOn()
{
    if (m_mprisTracklist) {
        QDBusReply<void> reply = m_mprisTracklist->SetRandom(true);
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::randomOff()
{
    if (m_mprisTracklist) {
        QDBusReply<void> reply = m_mprisTracklist->SetRandom(false);
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::trackRepeatOn()
{
    if (m_mprisPlayer) {
        QDBusReply<void> reply = m_mprisPlayer->Repeat(true);
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::trackRepeatOff()
{
    if (m_mprisPlayer) {
        QDBusReply<void> reply = m_mprisPlayer->Repeat(false);
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::playlistRepeatOn()
{
    if (m_mprisTracklist) {
        QDBusReply<void> reply = m_mprisTracklist->SetLoop(true);
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::playlistRepeatOff()
{
    if (m_mprisTracklist) {
        QDBusReply<void> reply = m_mprisTracklist->SetLoop(false);
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::setPosition()
{
    if (m_mprisPlayer) {
        int position = m_ui.newPosition->value();
        QDBusReply<void> reply = m_mprisPlayer->PositionSet(position);
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::setVolume() // slot
{
    if (m_mprisPlayer) {
        int volume = m_ui.newVolume->value();
        QDBusReply<void> reply = m_mprisPlayer->VolumeSet(volume);
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::updateTotalTrackCount(int trackCount)
{
    if (!trackCount) {
        m_ui.currentTrack->setText(tr("N/A"));
        m_ui.deleteTrackNumber->setMaximum(0);
        m_ui.fetchTrackNumber->setMaximum(0);
    } else {
        m_ui.deleteTrackNumber->setMaximum(trackCount - 1);
        m_ui.fetchTrackNumber->setMaximum(trackCount - 1);
    }
    m_ui.totalTracks->setText(QString::number(trackCount));
}

void Window::updateTracklist(int tracks)
{
    updateTotalTrackCount(tracks);
    if (tracks && m_mprisTracklist) {
        QDBusReply<int> current = m_mprisTracklist->GetCurrentTrack();
        if (current.isValid()) {
            m_ui.currentTrack->setText(QString::number(current));
        } else {
            m_ui.currentTrack->setText(tr("Invalid reply"));
            printDBusError(current.error());
        }
    }
}

void Window::fetchTrackMetadata()
{
    if (m_mprisTracklist) {
        int track = m_ui.fetchTrackNumber->value();
        QDBusReply<QVariantMap> metadata = m_mprisTracklist->GetMetadata(track);
        if (metadata.isValid()) {
            m_tracklistModel.setMetadata(metadata);
        } else {
            printDBusError(metadata.error());
        }
    }
}

void Window::addTrack()
{
    if (m_mprisTracklist) {
        QString uri = m_ui.trackUri->text();
        bool playNow = m_ui.playNow->isChecked();
        QDBusReply<int> reply = m_mprisTracklist->AddTrack(uri, playNow);
        if (reply.isValid()) {
            if (reply.value()) {
                consoleMessage(tr("Player reported adding track was successful"));
                m_ui.trackUri->clear();
            } else {
                consoleMessage(tr("Player reported adding track failed"));
            }
        } else {
            printDBusError(reply.error());
        }
    }
}

void Window::deleteTrack()
{
    if (m_mprisTracklist) {
        int track = m_ui.deleteTrackNumber->value();
        QDBusReply<void> reply = m_mprisTracklist->DelTrack(track);
        if (!reply.isValid()) {
            printDBusError(reply.error());
        }
    }
}

void Window::forceMetadataUpdate()
{
    if (m_mprisPlayer) {
        QDBusReply<QVariantMap> metadata = m_mprisPlayer->GetMetadata();
        if (metadata.isValid()) {
            m_currentTrackModel.setMetadata(metadata);
        } else {
            printDBusError(metadata.error());
        }
    }
}

void Window::forceCapsUpdate()
{
    if (m_mprisPlayer) {
        QDBusReply<int> caps = m_mprisPlayer->GetCaps();
        if (caps.isValid()) {
            setCaps(caps);
        } else {
            printDBusError(caps.error());
        }
    }
}

void Window::forceStatusUpdate()
{
    if (m_mprisPlayer) {
        QDBusReply<Mpris::Status> status = m_mprisPlayer->GetStatus();
        if (status.isValid()) {
            setStatus(status);
        } else {
            printDBusError(status.error());
        }
    }
}

void Window::forceTracklistUpdate()
{
    if (m_mprisTracklist) {
        QDBusReply<int> current = m_mprisTracklist->GetCurrentTrack();
        QDBusReply<int> length = m_mprisTracklist->GetLength();
        if (current.isValid()) {
            m_ui.currentTrack->setText(QString::number(current));
        } else {
            printDBusError(current.error());
        }
        if (length.isValid()) {
            updateTotalTrackCount(length);
        } else {
            printDBusError(length.error());
        }
    }
}

void Window::clear()
{
    m_timer.stop();

    delete m_mprisRoot;
    m_mprisRoot = 0;
    delete m_mprisPlayer;
    m_mprisPlayer = 0;
    delete m_mprisTracklist;
    m_mprisTracklist = 0;

    m_ui.appName->setText(tr("N/A"));
    m_ui.mprisVersion->setText("0");

    m_ui.currentPosition->setText("0");
    m_ui.currentVolume->setText("0");

    m_currentTrackModel.setMetadata(QVariantMap());
    m_tracklistModel.setMetadata(QVariantMap());

    setCaps(0);
    setStatus(Mpris::Status());
    updateTracklist(0);

    m_currentPlayer.clear();
}

void Window::consoleMessage(const QString& message)
{
    QString time = QTime::currentTime().toString();
    m_ui.console->appendPlainText("[ " + time + " ]: " + message);
}

void Window::printDBusError(const QDBusError& error)
{
    consoleMessage(tr("Got the error %1: %2").arg(error.name()).arg(error.message()));
}

void Window::setPlayer(const QString& dbusAddress)
{
    clear();

    consoleMessage(tr("Connecting to player %1").arg(dbusAddress));

    m_mprisRoot = new MprisRoot(dbusAddress,
                                "/",
                                QDBusConnection::sessionBus());
    m_mprisPlayer = new MprisPlayer(dbusAddress,
                                    "/Player",
                                    QDBusConnection::sessionBus());
    m_mprisTracklist = new MprisTracklist(dbusAddress,
                                          "/TrackList",
                                          QDBusConnection::sessionBus());

    if (!m_mprisRoot->isValid() || !m_mprisPlayer->isValid() || !m_mprisTracklist->isValid()) {
        consoleMessage(tr("Connection to player %1 failed").arg(dbusAddress));
        clear();
    } else {
        m_currentPlayer = dbusAddress;

        QDBusReply<QString> identity = m_mprisRoot->Identity();
        QDBusReply<Mpris::Version> version = m_mprisRoot->MprisVersion();
        if (identity.isValid()) {
            m_ui.appName->setText(identity);
        } else {
            m_ui.appName->setText(tr("Invalid reply"));
            printDBusError(identity.error());
        }
        if (version.isValid()) {
            Mpris::Version mprisVersion = version.value();
            m_ui.mprisVersion->setText(QString::number(mprisVersion.major) +
                                       QString('.') +
                                       QString::number(mprisVersion.minor));
            if (mprisVersion.major < 1) {
                consoleMessage(tr("Player reports an unknown MPRIS specification version"));
            } else if (mprisVersion.major > 1) {
                consoleMessage(tr("Player reports that it implements a (more recent) incompatible version of the MPRIS specification.  This tool cannot be used to usefully test compliance with this version of MPRIS."));
            } else if (mprisVersion.minor > 0) {
                consoleMessage(tr("Player reports that it implements a compatible but more recent version of the MPRIS specification than this tool knows about.  This tool can only be used to test those part of the MPRIS specification that were included in version 1.0."));
            }
        } else {
            m_ui.mprisVersion->setText(tr("Invalid reply"));
            printDBusError(version.error());
        }
        connect(m_mprisPlayer, SIGNAL(CapsChange(int)),
                this, SLOT(setCaps(int)));
        connect(m_mprisPlayer, SIGNAL(CapsChange(int)),
                this, SLOT(capsChangeNotify(int)));
        connect(m_mprisPlayer, SIGNAL(StatusChange(Mpris::Status)),
                this, SLOT(setStatus(Mpris::Status)));
        connect(m_mprisPlayer, SIGNAL(StatusChange(Mpris::Status)),
                this, SLOT(statusChangeNotify(Mpris::Status)));
        connect(m_mprisPlayer, SIGNAL(TrackChange(QVariantMap)),
                this, SLOT(trackChangeNotify(QVariantMap)));
        connect(m_mprisPlayer, SIGNAL(TrackChange(QVariantMap)),
                &m_currentTrackModel, SLOT(setMetadata(QVariantMap)));
        connect(m_mprisTracklist, SIGNAL(TrackListChange(int)),
                this, SLOT(trackListChangeNotify(int)));
        connect(m_mprisTracklist, SIGNAL(TrackListChange(int)),
                this, SLOT(updateTracklist(int)));
        forceCapsUpdate();
        forceStatusUpdate();
        forceMetadataUpdate();
        forceTracklistUpdate();
        m_timer.start();
    }
}


// vim: sw=4 sts=4 et tw=100
