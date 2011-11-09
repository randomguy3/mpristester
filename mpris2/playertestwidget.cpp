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


#include "playertestwidget.h"
#include "playerinterfacetest.h"
#include <qdebug.h>
#include <QTimer>
#include <QDBusObjectPath>

using namespace Mpris2;

PlayerTestWidget::PlayerTestWidget(PlayerInterfaceTest* test, QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.loopStatusCombo->addItem("None");
    ui.loopStatusCombo->addItem("Track");
    ui.loopStatusCombo->addItem("Playlist");
    metadataModel = new MetadataModel(this);
    ui.metadataTableView->setModel(metadataModel);
    this->test = test;
    connect(test, SIGNAL(propertiesChanged(QStringList)),
            this, SLOT(propertiesChanged(QStringList)));
    connect(test, SIGNAL(Seeked(qint64)),
            this, SLOT(Seeked(qint64)));
    estPosTimer = new QTimer(this);
    estPosTimer->setInterval(500);
    estPosTimer->setSingleShot(false);
    connect(estPosTimer, SIGNAL(timeout()),
            this, SLOT(updateEstPos()));

    connect(ui.loopStatusSetBtn, SIGNAL(clicked(bool)),
            this, SLOT(testSetLoopStatus()));
    connect(ui.shuffleOnBtn, SIGNAL(clicked(bool)),
            this, SLOT(testShuffleOn()));
    connect(ui.shuffleOffBtn, SIGNAL(clicked(bool)),
            this, SLOT(testShuffleOff()));
    connect(ui.volumeSetBtn, SIGNAL(clicked(bool)),
            this, SLOT(testSetVolume()));
    connect(ui.rateSetBtn, SIGNAL(clicked(bool)),
            this, SLOT(testSetRate()));
    connect(ui.nextBtn, SIGNAL(clicked(bool)),
            test, SLOT(testNext()));
    connect(ui.prevBtn, SIGNAL(clicked(bool)),
            test, SLOT(testPrevious()));
    connect(ui.pauseBtn, SIGNAL(clicked(bool)),
            test, SLOT(testPause()));
    connect(ui.playBtn, SIGNAL(clicked(bool)),
            test, SLOT(testPlay()));
    connect(ui.playPauseBtn, SIGNAL(clicked(bool)),
            test, SLOT(testPlayPause()));
    connect(ui.stopBtn, SIGNAL(clicked(bool)),
            test, SLOT(testStop()));
    connect(ui.seekBtn, SIGNAL(clicked(bool)),
            this, SLOT(testSeek()));
    connect(ui.setPosBtn, SIGNAL(clicked(bool)),
            this, SLOT(testSetPos()));
    connect(ui.openUriBtn, SIGNAL(clicked(bool)),
            this, SLOT(testOpenUri()));
}

PlayerTestWidget::~PlayerTestWidget()
{
}

void PlayerTestWidget::runInitialTest()
{
    test->initialTest();
}

void PlayerTestWidget::runIncrementalTest()
{
    test->incrementalTest();
}

void PlayerTestWidget::propertiesChanged(const QStringList& properties)
{
    Q_UNUSED(properties)

    if (test->properties().contains("PlaybackStatus")) {
        ui.playbackStatusLbl->setText(test->properties().value("PlaybackStatus").toString());
        ui.playbackStatusLbl->setEnabled(true);
    }
    if (test->properties().contains("LoopStatus")) {
        ui.loopStatusLbl->setText(test->properties().value("LoopStatus").toString());
        ui.loopStatusLbl->setEnabled(true);
    }
    if (test->properties().contains("CanGoNext")) {
        ui.canGoNextLbl->setText(test->properties().value("CanGoNext").toBool() ? "Yes" : "No");
        ui.canGoNextLbl->setEnabled(true);
    }
    if (test->properties().contains("CanGoPrevious")) {
        ui.canGoPrevLbl->setText(test->properties().value("CanGoPrevious").toBool() ? "Yes" : "No");
        ui.canGoPrevLbl->setEnabled(true);
    }
    if (test->properties().contains("CanPlay")) {
        ui.canPlayLbl->setText(test->properties().value("CanPlay").toBool() ? "Yes" : "No");
        ui.canPlayLbl->setEnabled(true);
    }
    if (test->properties().contains("CanPause")) {
        ui.canPauseLbl->setText(test->properties().value("CanPause").toBool() ? "Yes" : "No");
        ui.canPauseLbl->setEnabled(true);
    }
    if (test->properties().contains("CanSeek")) {
        ui.canSeekLbl->setText(test->properties().value("CanSeek").toBool() ? "Yes" : "No");
        ui.canSeekLbl->setEnabled(true);
    }
    if (test->properties().contains("CanControl")) {
        ui.canControlLbl->setText(test->properties().value("CanControl").toBool() ? "Yes" : "No");
        ui.canControlLbl->setEnabled(true);
    }
    if (test->properties().contains("Shuffle")) {
        ui.shuffleLbl->setText(test->properties().value("Shuffle").toBool() ? "Yes" : "No");
        ui.shuffleLbl->setEnabled(true);
    }
    if (test->properties().contains("Rate")) {
        ui.rateLbl->setText(QString::number(test->properties().value("Rate").toDouble(), 'g', 2));
        ui.rateLbl->setEnabled(true);
    }
    if (test->properties().contains("MinimumRate")) {
        ui.minRateLbl->setText(QString::number(test->properties().value("MinimumRate").toDouble(), 'g', 2));
        ui.minRateLbl->setEnabled(true);
    }
    if (test->properties().contains("MaximumRate")) {
        ui.maxRateLbl->setText(QString::number(test->properties().value("MaximumRate").toDouble(), 'g', 2));
        ui.maxRateLbl->setEnabled(true);
    }
    if (test->properties().contains("Volume")) {
        ui.volumeLbl->setText(QString::number(test->properties().value("Volume").toDouble(), 'g', 2));
        ui.volumeLbl->setEnabled(true);
    }
    if (test->properties().contains("Position")) {
        ui.lastKnownPosLbl->setText(QString::number(test->properties().value("Position").toLongLong()) + "ns");
        ui.lastKnownPosLbl->setEnabled(true);
    }
    if (test->predictedPosition() >= 0) {
        ui.estPosLbl->setText(QString::number(test->predictedPosition()) + "ns");
        ui.estPosLbl->setEnabled(true);
        if (!estPosTimer->isActive())
            estPosTimer->start();
    }
    if (test->properties().contains("Metadata")) {
        if (test->properties().value("Metadata").type() != QVariant::Map) {
            qDebug() << "Metadata map was wrong type";
        }
        QVariantMap metadata = test->properties().value("Metadata").toMap();
        metadataModel->setMetadata(metadata);
        ui.metadataTableView->setEnabled(true);
        if (ui.setPosTrackIdEdit->text().isEmpty() ||
            ui.setPosTrackIdEdit->text() == lastSetTrackId)
        {
            QString trackId = metadata.value("mpris:trackid").value<QDBusObjectPath>().path();
            ui.setPosTrackIdEdit->setText(trackId);
            lastSetTrackId = trackId;
        }
    }
}

void PlayerTestWidget::updateEstPos()
{
    ui.estPosLbl->setText(QString::number(test->predictedPosition()) + "ns");
}

void PlayerTestWidget::testSeek()
{
    qint64 offset = (qint64)ui.seekSpinBox->value() * 1000;
    test->testSeek(offset);
}

void PlayerTestWidget::testSetPos()
{
    qint64 pos = (qint64)ui.setPosSpinBox->value() * 1000;
    test->testSetPosition(QDBusObjectPath(ui.setPosTrackIdEdit->text()), pos);
}

void PlayerTestWidget::testOpenUri()
{
    test->testOpenUri(ui.openUriEdit->text());
}

void PlayerTestWidget::Seeked(qint64 position)
{
    ui.lastKnownPosLbl->setText(QString::number(position) + "ns");
    ui.lastKnownPosLbl->setEnabled(true);
}

void PlayerTestWidget::testSetLoopStatus()
{
    test->testSetLoopStatus(ui.loopStatusCombo->currentText());
}

void PlayerTestWidget::testShuffleOn()
{
    test->testSetShuffle(true);
}

void PlayerTestWidget::testShuffleOff()
{
    test->testSetShuffle(false);
}

void PlayerTestWidget::testSetVolume()
{
    test->testSetVolume(ui.volumeSpinBox->value());
}

void PlayerTestWidget::testSetRate()
{
    test->testSetRate(ui.rateSpinBox->value());
}
