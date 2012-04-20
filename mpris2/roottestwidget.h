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


#ifndef ROOTTESTWIDGET_H
#define ROOTTESTWIDGET_H

#include <QWidget>
#include "ui_roottest.h"

namespace Mpris2
{
    class RootInterfaceTest;
    class RootTestWidget : public QWidget
    {
        Q_OBJECT

    public:
        RootTestWidget(RootInterfaceTest *test, QWidget *parent = 0);
        virtual ~RootTestWidget();

    public slots:
        void runInitialTest();
        void runIncrementalTest();

    private slots:
        void propertiesChanged(const QStringList& properties);
        void testSetFullScreenOn();
        void testSetFullScreenOff();

    private:
        void updateBoolPropLabel(const QString& name, QLabel *label);
        void updateStringPropLabel(const QString& name, QLabel *label);

        Ui_RootTestForm ui;
        RootInterfaceTest* test;
    };
}

#endif // ROOTTESTWIDGET_H
// vim:et:sw=4:sts=4
