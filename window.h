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

#include <QMainWindow>
#include <QTimer>

class QActionGroup;
class QAction;
class QDBusServiceWatcher;
namespace Mpris2 {
    class RootInterfaceTest;
    class PlayerInterfaceTest;
}

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

private:
    void setPlayer(const QString& dbusAddress);
    void clear();

private:
    Ui_MainWindow m_ui;
    QTimer m_timer;

    QMap<QString, QAction*> m_playerActions;
    QActionGroup* m_playerActionGroup;

    QString m_currentPlayer;

    QTabWidget* m_tabWidget;

    Mpris2::RootInterfaceTest* m_rootTest;
    QWidget* m_rootWidget;

    Mpris2::PlayerInterfaceTest* m_playerTest;
    QWidget* m_playerWidget;

    QDBusServiceWatcher* m_watcher;
};

#endif // WINDOW_H
