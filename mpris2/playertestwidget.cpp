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

#include <math.h>

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
    ui.seekTimeUnits->addItem(QString::fromUtf8("µs"));
    ui.seekTimeUnits->addItem("ms");
    ui.seekTimeUnits->addItem("s");
    ui.seekTimeUnits->setCurrentIndex(0);
    ui.setPosTimeUnits->addItem(QString::fromUtf8("µs"));
    ui.setPosTimeUnits->addItem("ms");
    ui.setPosTimeUnits->addItem("s");
    ui.setPosTimeUnits->setCurrentIndex(0);
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

static QString formatTimeUs(qlonglong time)
{
    qlonglong secs = static_cast<qlonglong>(round(time / 1000000.0));
    qlonglong mins = secs / 60;
    secs = secs % 60;
    return QString::number(time) + QString::fromUtf8("µs (")
            + QString::number(mins) + ":"
            + QString::number(secs).rightJustified(2, '0') + ")";
}

void PlayerTestWidget::propertiesChanged(const QStringList& properties)
{
    Q_UNUSED(properties)

    updateStringPropLabel("PlaybackStatus", ui.playbackStatusLbl);
    updateStringPropLabel("LoopStatus", ui.loopStatusLbl);
    updateBoolPropLabel("CanGoNext", ui.canGoNextLbl);
    updateBoolPropLabel("CanGoPrevious", ui.canGoPrevLbl);
    updateBoolPropLabel("CanPlay", ui.canPlayLbl);
    updateBoolPropLabel("CanPause", ui.canPauseLbl);
    updateBoolPropLabel("CanSeek", ui.canSeekLbl);
    updateBoolPropLabel("CanControl", ui.canControlLbl);
    updateBoolPropLabel("Shuffle", ui.shuffleLbl);
    updateDoublePropLabel("Rate", ui.rateLbl);
    updateDoublePropLabel("MinimumRate", ui.minRateLbl);
    updateDoublePropLabel("MaximumRate", ui.maxRateLbl);
    updateDoublePropLabel("Volume", ui.volumeLbl);
    if (test->properties().contains("Position")) {
        ui.lastKnownPosLbl->setText(formatTimeUs(test->properties().value("Position").toLongLong()));
        ui.lastKnownPosLbl->setEnabled(true);
    } else {
        ui.lastKnownPosLbl->setText("<unknown>");
        ui.lastKnownPosLbl->setEnabled(false);
    }
    if (test->predictedPosition() >= 0) {
        ui.estPosLbl->setText(formatTimeUs(test->predictedPosition()));
        ui.estPosLbl->setEnabled(true);
        if (!estPosTimer->isActive())
            estPosTimer->start();
    } else {
        ui.estPosLbl->setText("<unknown>");
        ui.estPosLbl->setEnabled(false);
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
    } else {
        ui.metadataTableView->setEnabled(false);
    }
}

void PlayerTestWidget::updateEstPos()
{
    if (test->predictedPosition() >= 0) {
        ui.estPosLbl->setText(formatTimeUs(test->predictedPosition()));
        ui.estPosLbl->setEnabled(true);
    } else {
        ui.estPosLbl->setText("<unknown>");
        ui.estPosLbl->setEnabled(false);
    }
}

void PlayerTestWidget::testSeek()
{
    qint64 offset = (qint64)ui.seekSpinBox->value(); // µs
    if (ui.seekTimeUnits->currentIndex() > 0)
        offset *= 1000;  // ms
    if (ui.seekTimeUnits->currentIndex() > 1)
        offset *= 1000;  // s
    test->testSeek(offset);
}

void PlayerTestWidget::testSetPos()
{
    qint64 pos = (qint64)ui.setPosSpinBox->value();  // µs
    if (ui.setPosTimeUnits->currentIndex() > 0)
        pos *= 1000;  // ms
    if (ui.setPosTimeUnits->currentIndex() > 1)
        pos *= 1000;  // s
    test->testSetPosition(QDBusObjectPath(ui.setPosTrackIdEdit->text()), pos);
}

void PlayerTestWidget::testOpenUri()
{
    test->testOpenUri(ui.openUriEdit->text());
}

void PlayerTestWidget::Seeked(qint64 position)
{
    ui.lastKnownPosLbl->setText(formatTimeUs(position));
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

void PlayerTestWidget::updateBoolPropLabel(const QString& name, QLabel *label)
{
    if (test->properties().contains(name)) {
        label->setText(test->properties().value(name).toBool() ? "Yes" : "No");
        label->setEnabled(true);
    } else {
        label->setText("<unknown>");
        label->setEnabled(false);
    }
}

void PlayerTestWidget::updateStringPropLabel(const QString& name, QLabel *label)
{
    if (test->properties().contains(name)) {
        label->setText(test->properties().value(name).toString());
        label->setEnabled(true);
    } else {
        label->setText("<unknown>");
        label->setEnabled(false);
    }
}

void PlayerTestWidget::updateDoublePropLabel(const QString& name, QLabel *label)
{
    if (test->properties().contains(name)) {
        label->setText(QString::number(test->properties().value(name).toDouble(), 'g', 2));
        label->setEnabled(true);
    } else {
        label->setText("<unknown>");
        label->setEnabled(false);
    }
}

// vim:et:sw=4:sts=4
