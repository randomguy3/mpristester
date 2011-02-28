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

#ifndef WINDOW_H
#define WINDOW_H

#include "ui_window.h"
#include "metadatamodel.h"
#include "dbus/mpristypes.h"

#include <QDBusError>
#include <QMainWindow>
#include <QTimer>

class QActionGroup;
class MprisRoot;
class MprisPlayer;
class MprisTracklist;
class QAction;

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QWidget* parent = 0);

private slots:
    void serviceChange(const QString& name,
                       const QString& oldOwner,
                       const QString& newOwner);
    void changePlayer(QAction* action);
    void updateChangables();
    void setCaps(int caps);
    void setStatus(Mpris::Status);
    void capsChangeNotify(int caps);
    void statusChangeNotify(Mpris::Status);
    void trackChangeNotify(const QVariantMap& metadata);
    void trackListChangeNotify(int length);
    void quitPlayer();
    void play();
    void pause();
    void stop();
    void prev();
    void next();
    void randomOn();
    void randomOff();
    void trackRepeatOn();
    void trackRepeatOff();
    void playlistRepeatOn();
    void playlistRepeatOff();
    void setPosition();
    void setVolume();
    void updateTracklist(int tracks);
    void fetchTrackMetadata();
    void addTrack();
    void deleteTrack();
    void forceMetadataUpdate();
    void forceCapsUpdate();
    void forceStatusUpdate();
    void forceTracklistUpdate();
    void updateTotalTrackCount(int trackCount);

private:
    void clear();
    void consoleMessage(const QString& message);
    void printDBusError(const QDBusError& error);
    void setPlayer(const QString& dbusAddress);

private:
    Ui_MainWindow m_ui;
    MetadataModel m_currentTrackModel;
    MetadataModel m_tracklistModel;
    QTimer m_timer;

    MprisRoot* m_mprisRoot;
    MprisPlayer* m_mprisPlayer;
    MprisTracklist* m_mprisTracklist;

    QMap<QString, QAction*> m_playerActions;
    QActionGroup* m_playerActionGroup;

    QString m_currentPlayer;
};

#endif // WINDOW_H
