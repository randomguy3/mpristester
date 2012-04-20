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
    connect(ui.fullscreenOnBtn, SIGNAL(clicked(bool)),
            this, SLOT(testSetFullScreenOn()));
    connect(ui.fullscreenOffBtn, SIGNAL(clicked(bool)),
            this, SLOT(testSetFullScreenOff()));
    connect(test, SIGNAL(propertiesChanged(QStringList)),
            this, SLOT(propertiesChanged(QStringList)));
}

RootTestWidget::~RootTestWidget()
{
}

void RootTestWidget::runInitialTest()
{
    test->initialTest();
}

void RootTestWidget::runIncrementalTest()
{
    test->incrementalTest();
}

void RootTestWidget::updateBoolPropLabel(const QString& name, QLabel *label)
{
    if (test->properties().contains(name)) {
        label->setText(test->properties().value(name).toBool() ? "Yes" : "No");
        label->setEnabled(true);
    } else {
        label->setText("<unknown>");
        label->setEnabled(false);
    }
}

void RootTestWidget::updateStringPropLabel(const QString& name, QLabel *label)
{
    if (test->properties().contains(name)) {
        label->setText(test->properties().value(name).toString());
        label->setEnabled(true);
    } else {
        label->setText("<unknown>");
        label->setEnabled(false);
    }
}

void RootTestWidget::propertiesChanged(const QStringList& properties)
{
    Q_UNUSED(properties)

    updateStringPropLabel("Identity", ui.identityLbl);
    updateStringPropLabel("DesktopEntry", ui.desktopFileLbl);
    updateBoolPropLabel("CanRaise", ui.canRaiseLbl);
    updateBoolPropLabel("CanQuit", ui.canQuitLbl);
    updateBoolPropLabel("CanSetFullscreen", ui.canSetFullscreenLbl);
    updateBoolPropLabel("Fullscreen", ui.fullscreenLbl);
    updateBoolPropLabel("HasTrackList", ui.hasTracklistLbl);
    if (test->properties().contains("SupportedUriSchemes")) {
        QStringList uriSchemes = test->properties().value("SupportedUriSchemes").toStringList();
        ui.uriSchemesList->clear();
        ui.uriSchemesList->addItems(uriSchemes);
        ui.uriSchemesList->setEnabled(true);
    } else {
        ui.uriSchemesList->setEnabled(false);
    }
    if (test->properties().contains("SupportedMimeTypes")) {
        QStringList mimetypes = test->properties().value("SupportedMimeTypes").toStringList();
        ui.mimetypesList->clear();
        ui.mimetypesList->addItems(mimetypes);
        ui.mimetypesList->setEnabled(true);
    } else {
        ui.mimetypesList->setEnabled(false);
    }
}

void RootTestWidget::testSetFullScreenOn()
{
    test->testSetFullscreen(true);
}

void RootTestWidget::testSetFullScreenOff()
{
    test->testSetFullscreen(false);
}

// vim:et:sw=4:sts=4
