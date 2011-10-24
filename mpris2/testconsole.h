/*
    Copyright (C) 2011  Alex Merry <dev@randomguy3.me.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef TESTCONSOLE_H
#define TESTCONSOLE_H

#include <QtGui/QWidget>
#include "interfacetest.h"

class QTextEdit;
namespace Mpris2
{
    class TestConsole : public QWidget
    {

    public:
        TestConsole(QWidget* parent = 0);
        virtual ~TestConsole();

    public slots:
        void interfaceError(InterfaceTest::LocationType locType, const QString& location, const QString& desc);
        void interfaceWarning(InterfaceTest::LocationType locType, const QString& location, const QString& desc);
        void interfaceInfo(InterfaceTest::LocationType locType, const QString& location, const QString& desc);

    private:
        QTextEdit* edit;
    };
}

#endif // TESTCONSOLE_H
