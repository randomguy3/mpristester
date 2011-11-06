/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Alex Merry <alex.merry@cs.ox.ac.uk>

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


#ifndef MPRIS2_PLAYERTESTWIDGET_H
#define MPRIS2_PLAYERTESTWIDGET_H

#include <QtGui/QWidget>
#include "ui_playertest.h"
#include "../metadatamodel.h"

class QTimer;
namespace Mpris2 {

    class PlayerInterfaceTest;

    class PlayerTestWidget : public QWidget
    {
            Q_OBJECT

    public:
        PlayerTestWidget(PlayerInterfaceTest *test, QWidget *parent = 0);
        virtual ~PlayerTestWidget();

    public slots:
        void runInitialTest();
        void runIncrementalTest();
        void testSeek();
        void testSetPos();
        void testOpenUri();
        void Seeked(qint64 position);

    private slots:
        void propertiesChanged(const QStringList& properties);
        void updateEstPos();

    private:
        Ui_PlayerTestForm    ui;
        PlayerInterfaceTest *test;
        QTimer              *estPosTimer;
        MetadataModel       *metadataModel;
        QString              lastSetTrackId;
    };

}

#endif // MPRIS2_PLAYERTESTWIDGET_H
