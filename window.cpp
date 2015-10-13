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

#include "mpris2/rootinterfacetest.h"
#include "mpris2/roottestwidget.h"
#include "mpris2/playerinterfacetest.h"
#include "mpris2/playertestwidget.h"
#include "mpris2/testconsole.h"

#include <QAction>
#include <QActionGroup>
#include <QtDBus>
#include <QDebug>
#include <QMetaObject>

static const QString playerPrefix = QLatin1String("org.mpris.MediaPlayer2.");

Window::Window(QWidget* parent)
    : QMainWindow(parent),
      m_playerActionGroup(new QActionGroup(this)),
      m_tabWidget(0),
      m_rootTest(0),
      m_rootWidget(0),
      m_playerTest(0),
      m_playerWidget(0),
      m_watcher(0)
{
    m_ui.setupUi(this);

    connect(m_ui.action_Quit, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_playerActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(changePlayer(QAction*)));

    m_timer.setInterval(2500);

    QDBusConnection sessionConn = QDBusConnection::sessionBus();
    if (sessionConn.isConnected()) {
        m_watcher = new QDBusServiceWatcher(QString(),
                                            sessionConn,
                                            QDBusServiceWatcher::WatchForOwnerChange,
                                            this);
        connect(m_watcher, SIGNAL(serviceOwnerChanged(QString,QString,QString)),
                this,  SLOT(serviceChange(QString,QString,QString)));

        QDBusConnectionInterface* bus = sessionConn.interface();

        QDBusReply<QStringList> reply = bus->registeredServiceNames();
        if (reply.isValid()) {
            QStringList services = reply.value();
            foreach (const QString& name, services) {
                if (name.startsWith(playerPrefix)) {
                    serviceChange(name, QString(), "dummy");
                }
            }
        } else {
            qDebug() << "Couldn't get service names:" << reply.error().message();
        }
    } else {
        qDebug() << "Could not connect to session bus";
    }
}

void Window::serviceChange(const QString& name,
                           const QString& oldOwner,
                           const QString& newOwner)
{
    if (!name.startsWith(playerPrefix)) {
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
                qDebug() << "Player" << name << "disappeared";
            }
            m_playerActionGroup->removeAction(m_playerActions[name]);
            delete m_playerActions[name];
            m_playerActions.remove(name);
        } else {
            qDebug() << "Player" << name << "disappeared, but we didn't know about it";
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

void Window::clear()
{
    m_timer.stop();

    setCentralWidget(new QWidget());

    m_tabWidget->deleteLater();
    m_tabWidget = 0;

    m_rootTest->deleteLater();
    m_rootTest = 0;
    m_rootWidget->deleteLater();
    m_rootWidget = 0;

    m_playerTest->deleteLater();
    m_playerTest = 0;
    m_playerWidget->deleteLater();
    m_playerWidget = 0;

    m_currentPlayer.clear();
}

void Window::setPlayer(const QString& dbusAddress)
{
    if (!m_currentPlayer.isEmpty()) {
        clear();
    }

    m_currentPlayer = dbusAddress;

    qDebug() << "Connecting to player" << dbusAddress;

    m_tabWidget = new QTabWidget(this);

    m_rootTest = new Mpris2::RootInterfaceTest(dbusAddress, this);
    connect(&m_timer, SIGNAL(timeout()),
            m_rootTest, SLOT(incrementalTest()));

    m_rootWidget = new QWidget(m_tabWidget);
    QBoxLayout *layout = new QVBoxLayout(m_rootWidget);
    layout->addWidget(new Mpris2::RootTestWidget(m_rootTest));
    Mpris2::TestConsole* console = new Mpris2::TestConsole();
    connect(m_rootTest, SIGNAL(interfaceError(Mpris2::InterfaceTest::LocationType,QString,QString)),
            console, SLOT(interfaceError(Mpris2::InterfaceTest::LocationType,QString,QString)));
    connect(m_rootTest, SIGNAL(interfaceWarning(Mpris2::InterfaceTest::LocationType,QString,QString)),
            console, SLOT(interfaceWarning(Mpris2::InterfaceTest::LocationType,QString,QString)));
    connect(m_rootTest, SIGNAL(interfaceInfo(Mpris2::InterfaceTest::LocationType,QString,QString)),
            console, SLOT(interfaceInfo(Mpris2::InterfaceTest::LocationType,QString,QString)));
    layout->addWidget(console);
    m_tabWidget->addTab(m_rootWidget, "Root iface");

    m_playerTest = new Mpris2::PlayerInterfaceTest(dbusAddress, this);
    connect(&m_timer, SIGNAL(timeout()),
            m_playerTest, SLOT(incrementalTest()));

    m_playerWidget = new QWidget(m_tabWidget);
    layout = new QVBoxLayout(m_playerWidget);
    layout->addWidget(new Mpris2::PlayerTestWidget(m_playerTest));
    console = new Mpris2::TestConsole();
    connect(m_playerTest, SIGNAL(interfaceError(Mpris2::InterfaceTest::LocationType,QString,QString)),
            console, SLOT(interfaceError(Mpris2::InterfaceTest::LocationType,QString,QString)));
    connect(m_playerTest, SIGNAL(interfaceWarning(Mpris2::InterfaceTest::LocationType,QString,QString)),
            console, SLOT(interfaceWarning(Mpris2::InterfaceTest::LocationType,QString,QString)));
    connect(m_playerTest, SIGNAL(interfaceInfo(Mpris2::InterfaceTest::LocationType,QString,QString)),
            console, SLOT(interfaceInfo(Mpris2::InterfaceTest::LocationType,QString,QString)));
    layout->addWidget(console);
    m_tabWidget->addTab(m_playerWidget, "Player iface");

    this->setCentralWidget(m_tabWidget);

    QMetaObject::invokeMethod(m_rootTest, "initialTest", Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_playerTest, "initialTest", Qt::QueuedConnection);
    m_timer.start();
}


// vim: sw=4 sts=4 et tw=100
