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


#include "roottestwidget.h"
#include "rootinterfacetest.h"

using namespace Mpris2;

RootTestWidget::RootTestWidget(RootInterfaceTest *test, QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->test = test;
    connect(ui.raiseBtn, SIGNAL(clicked(bool)),
            this->test, SLOT(testRaise()));
    connect(ui.quitBtn, SIGNAL(clicked(bool)),
            this->test, SLOT(testQuit()));
}

RootTestWidget::~RootTestWidget()
{
}

void RootTestWidget::runInitialTest()
{
    test->initialTest();
    ui.raiseBtn->setEnabled(true);
    ui.quitBtn->setEnabled(true);
    updateProperties();
}

void RootTestWidget::runIncrementalTest()
{
    test->incrementalTest();
}

void RootTestWidget::propertiesChanged(const QStringList& properties)
{
    Q_UNUSED(properties)
    updateProperties();
}

void RootTestWidget::updateProperties()
{
    if (test->properties().contains("Identity")) {
        ui.identityLbl->setText(test->properties().value("Identity").toString());
        ui.identityLbl->setEnabled(true);
    }
    if (test->properties().contains("DesktopEntry")) {
        ui.desktopFileLbl->setText(test->properties().value("DesktopEntry").toString());
        ui.desktopFileLbl->setEnabled(true);
    }
    if (test->properties().contains("CanRaise")) {
        ui.canRaiseLbl->setText(test->properties().value("CanRaise").toBool() ? "Yes" : "No");
        ui.canRaiseLbl->setEnabled(true);
    }
    if (test->properties().contains("CanQuit")) {
        ui.canQuitLbl->setText(test->properties().value("CanQuit").toBool() ? "Yes" : "No");
        ui.canQuitLbl->setEnabled(true);
    }
    if (test->properties().contains("HasTrackList")) {
        ui.hasTracklistLbl->setText(test->properties().value("HasTrackList").toBool() ? "Yes" : "No");
        ui.hasTracklistLbl->setEnabled(true);
    }
    if (test->properties().contains("SupportedUriSchemes")) {
        QStringList uriSchemes = test->properties().value("SupportedUriSchemes").toStringList();
        ui.uriSchemesList->addItems(uriSchemes);
        ui.uriSchemesList->setEnabled(true);
    }
    if (test->properties().contains("SupportedMimeTypes")) {
        QStringList mimetypes = test->properties().value("SupportedMimeTypes").toStringList();
        ui.mimetypesList->addItems(mimetypes);
        ui.mimetypesList->setEnabled(true);
    }
}

