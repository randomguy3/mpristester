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
#ifndef ROOT_INTERFACE_TEST_H
#define ROOT_INTERFACE_TEST_H

#include "interfacetest.h"

class QDBusInterface;

namespace Mpris2
{
    class RootInterfaceTest : public InterfaceTest
    {
        Q_OBJECT

        public:
            /**
             * Contructs a root interface test for a given D-Bus service.
             *
             * @param service  a D-Bus service, such as org.mpris.MediaPlayer2.amarok
             */
            RootInterfaceTest(const QString& service, QObject* parent = 0);

            virtual ~RootInterfaceTest();

        public slots:
            /**
             * Attempt to call the Quit method
             *
             * This should make the service go away.
             */
            void testQuit();

            /**
             * Attempt to call the Raise method
             *
             * There is no reasonable way to automatically test that this was
             * successful.  However, if CanRaise is true and the method was
             * not found, an error will be reported.
             */
            void testRaise();

        protected:
            virtual void checkProps(const QVariantMap& oldProps = QVariantMap());
            virtual void checkUpdatedProperty(const QString& propName);
            virtual void checkConsistency(const QVariantMap& oldProps = QVariantMap());

        private:
            void checkPropertyIdentity(const QVariantMap& oldProps = QVariantMap());
            void checkPropertyDesktopEntry(const QVariantMap& oldProps = QVariantMap());
            void checkPropertySupportedUriSchemes(const QVariantMap& oldProps = QVariantMap());
            void checkPropertySupportedMimeTypes(const QVariantMap& oldProps = QVariantMap());
    };
}

#endif // ROOT_INTERFACE_TEST_H
// vim:et:sw=4:sts=4
