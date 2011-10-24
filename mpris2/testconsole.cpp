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


#include "testconsole.h"
#include <QTextEdit>
#include <QVBoxLayout>

using namespace Mpris2;

TestConsole::TestConsole(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    edit = new QTextEdit();
    layout->addWidget(edit);
    edit->setReadOnly(true);
}

TestConsole::~TestConsole()
{
}

static QString locationName(InterfaceTest::LocationType locType, const QString& location)
{
    switch (locType) {
        case InterfaceTest::Method:
            return "method " + location;
        case InterfaceTest::Property:
            return "property " + location;
        case InterfaceTest::Signal:
            return "signal " + location;
        default:
            return location;
    }
}

void TestConsole::interfaceError(InterfaceTest::LocationType locType, const QString& location, const QString& desc)
{
    if (locType == Mpris2::InterfaceTest::Other && location.isEmpty())
        edit->append("Error: " + desc + "\n");
    else
        edit->append("Error at " + locationName(locType, location) + ": " + desc + "\n");
}

void TestConsole::interfaceWarning(InterfaceTest::LocationType locType, const QString& location, const QString& desc)
{
    if (locType == Mpris2::InterfaceTest::Other && location.isEmpty())
        edit->append("Warning: " + desc + "\n");
    else
        edit->append("Warning at " + locationName(locType, location) + ": " + desc + "\n");
}

void TestConsole::interfaceInfo(InterfaceTest::LocationType locType, const QString& location, const QString& desc)
{
    if (locType == Mpris2::InterfaceTest::Other && location.isEmpty())
        edit->append("Info: " + desc + "\n");
    else
        edit->append("Info at " + locationName(locType, location) + ": " + desc + "\n");
}
