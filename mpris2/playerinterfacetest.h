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


#ifndef MPRIS2_PLAYERINTERFACETEST_H
#define MPRIS2_PLAYERINTERFACETEST_H

#include <../../home/alex/src/mpristester/mpris2/interfacetest.h>


namespace Mpris2 {
    class PlayerInterfaceTest : public InterfaceTest
    {
        Q_OBJECT

    public:
        PlayerInterfaceTest(const QString& service, QObject* parent = 0);
        virtual ~PlayerInterfaceTest();

    protected:
        virtual void checkUpdatedProperty(const QString& propName);
        virtual void checkProps(const QVariantMap& oldProps = QVariantMap());

    private:
        void checkControlProp(const QString& propName, const QVariantMap& oldProps = QVariantMap());
        void checkVolume(const QVariantMap& oldProps = QVariantMap());
        void checkLoopStatus(const QVariantMap& oldProps = QVariantMap());
        void checkPlaybackStatus(const QVariantMap& oldProps = QVariantMap());
    };
}

#endif // MPRIS2_PLAYERINTERFACETEST_H
