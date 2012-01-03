/*
 * Copyright 2007 Frerich Raabe <raabe@kde.org>
 * Copyright 2007 Aaron Seigo <aseigo@kde.org>
 * Copyright 2008 Aleix Pol <aleixpol@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fullview.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QResizeEvent>
#include <QTimer>

#include <KCmdLineArgs>
#include <KIconLoader>

#include <Plasma/AccessManager>
#include <Plasma/AccessAppletJob>
#include <Plasma/Containment>
#include <Plasma/ContainmentActions>
#include <Plasma/Package>
#include <Plasma/Wallpaper>

using namespace Plasma;

FullView::FullView(const QString &ff, const QString &loc, bool persistent, QWidget *parent)
    : QGraphicsView(parent),
      m_formfactor(Plasma::Planar),
      m_location(Plasma::Floating),
      m_containment(0),
      m_applet(0),
      m_appletShotTimer(0),
      m_persistentConfig(persistent)
{
    setFrameStyle(QFrame::NoFrame);
    QString formfactor = ff.toLower();
    if (formfactor.isEmpty() || formfactor == "planar") {
        m_formfactor = Plasma::Planar;
    } else if (formfactor == "vertical") {
        m_formfactor = Plasma::Vertical;
    } else if (formfactor == "horizontal") {
        m_formfactor = Plasma::Horizontal;
    } else if (formfactor == "mediacenter") {
        m_formfactor = Plasma::MediaCenter;
    }

    QString location = loc.toLower();
    if (loc.isEmpty() || loc == "floating") {
        m_location = Plasma::Floating;
    } else if (loc == "desktop") {
        m_location = Plasma::Desktop;
    } else if (loc == "fullscreen") {
        m_location = Plasma::FullScreen;
    } else if (loc == "top") {
        m_location = Plasma::TopEdge;
    } else if (loc == "bottom") {
        m_location = Plasma::BottomEdge;
    } else if (loc == "right") {
        m_location = Plasma::RightEdge;
    } else if (loc == "left") {
        m_location = Plasma::LeftEdge;
    }

    Plasma::ContainmentActionsPluginsConfig containmentActionPlugins;
    containmentActionPlugins.addPlugin(Qt::NoModifier, Qt::RightButton, "contextmenu");

    m_corona.setContainmentActionsDefaults(Plasma::Containment::DesktopContainment, containmentActionPlugins);
    m_corona.setContainmentActionsDefaults(Plasma::Containment::CustomContainment, containmentActionPlugins);
    m_corona.setContainmentActionsDefaults(Plasma::Containment::PanelContainment, containmentActionPlugins);
    m_corona.setContainmentActionsDefaults(Plasma::Containment::CustomPanelContainment, containmentActionPlugins);

    setScene(&m_corona);
    connect(&m_corona, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChanged(QRectF)));
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

FullView::~FullView()
{
    storeCurrentApplet();
}

void FullView::addApplet(const QString &name, const QString &containment,
                         const QString& wallpaper, const QVariantList &args)
{
    kDebug() << "adding applet" << name << "in" << containment;
    if (!m_containment || m_containment->pluginName() != containment) {
        delete m_containment;
        m_containment = m_corona.addContainment(containment);
        connect(m_containment, SIGNAL(appletRemoved(Plasma::Applet*)), this, SLOT(appletRemoved(Plasma::Applet*)));
    }

    if (!wallpaper.isEmpty()) {
        m_containment->setWallpaper(wallpaper);
    }

    m_containment->setFormFactor(m_formfactor);
    m_containment->setLocation(m_location);
    m_containment->resize(size());
    setScene(m_containment->scene());

    if (name.startsWith("plasma:") || name.startsWith("zeroconf:")) {
        kDebug() << "accessing remote: " << name;
        AccessManager::self()->accessRemoteApplet(KUrl(name));
        connect(AccessManager::self(), SIGNAL(finished(Plasma::AccessAppletJob*)),
                this, SLOT(plasmoidAccessFinished(Plasma::AccessAppletJob*)));
        return;
    }

    if (m_applet) {
        // we already have an applet!
        storeCurrentApplet();
        disconnect(m_applet);
        m_applet->destroy();
    }

    QFileInfo info(name);
    if (!info.isAbsolute()) {
        info = QFileInfo(QDir::currentPath() + "/" + name);
    }

    if (info.exists()) {
        m_applet = Applet::loadPlasmoid(info.absoluteFilePath());
    }

    if (m_applet) {
        m_containment->addApplet(m_applet, QPointF(-1, -1), false);
    } else if (name.isEmpty()) {
        return;
    } else {
        m_applet = m_containment->addApplet(name, args, QRectF(0, 0, -1, -1));
    }

    if (!m_applet) {
        return;
    }

    if (hasStorageGroupFor(m_applet) && m_persistentConfig) {
        KConfigGroup cg = m_applet->config();
        KConfigGroup storage = storageGroup(m_applet);
        cg.deleteGroup();
        storage.copyTo(&cg);
        m_applet->configChanged();
    }

    setSceneRect(m_applet->sceneBoundingRect());
    m_applet->setFlag(QGraphicsItem::ItemIsMovable, false);
    setWindowTitle(m_applet->name());
    setWindowIcon(SmallIcon(m_applet->icon()));
    resize(m_applet->size().toSize());
    connect(m_applet, SIGNAL(appletTransformedItself()), this, SLOT(appletTransformedItself()));
    kDebug() << "connecting ----------------";

    checkShotTimer();
}

bool FullView::checkShotTimer()
{
    KCmdLineArgs *cliArgs = KCmdLineArgs::parsedArgs();
    if (cliArgs->isSet("screenshot") || cliArgs->isSet("screenshot-all")) {
        if (!m_appletShotTimer) {
            m_appletShotTimer = new QTimer(this);
            m_appletShotTimer->setSingleShot(true);
            m_appletShotTimer->setInterval(3000);
            connect(m_appletShotTimer, SIGNAL(timeout()), this, SLOT(screenshotPlasmoid()));
        }

        m_appletShotTimer->start();
        return true;
    }

    return false;
}

void FullView::screenshotAll()
{
    KPluginInfo::List infoList = Plasma::Applet::listAppletInfo();
    foreach (const KPluginInfo &info, infoList) {
        m_appletsToShoot.append(info.pluginName());
    }
    shootNextPlasmoid();
}

void FullView::shootNextPlasmoid()
{
    if (m_appletsToShoot.isEmpty()) {
        QApplication::quit();
        return;
    }

    if (m_applet) {
        m_applet->destroy();
        m_applet = 0;
    }

    resize(512, 512);
    QString next = m_appletsToShoot.takeFirst();
    addApplet(next, "null", QString(), QVariantList());
    if (!m_applet) {
        shootNextPlasmoid();
    } else if (m_applet->size().width() < 256 && m_applet->size().height() < 256) {
        resize(512, 512);
    }
}

void FullView::screenshotPlasmoid()
{
    if (!m_applet) {
        shootNextPlasmoid();
        return;
    }

    if (m_applet->hasFailedToLaunch()) {
        m_applet->destroy();
        return;
    } else if (m_applet->configurationRequired()) {
        QTimer::singleShot(3000, this, SLOT(screenshotPlasmoid()));
        return;
    }

    QStyleOptionGraphicsItem opt;
    opt.initFrom(this);
    opt.exposedRect = m_applet->boundingRect();
    QPixmap p(size());
    p.fill(Qt::transparent);
    {
    QPainter painter(&p);
    render(&painter);
    //m_applet->paint(&painter, &opt, this);
    }
    p.save(m_applet->pluginName() + ".png");

    shootNextPlasmoid();
}

void FullView::plasmoidAccessFinished(Plasma::AccessAppletJob *job)
{
    kDebug() << "!!!! PLASMOID ACCESS FINISHED!";
    if (!job->error() && job->applet()) {
        m_applet = job->applet();
        m_containment->addApplet(m_applet, QPointF(-1, -1), false);
        m_applet->setFlag(QGraphicsItem::ItemIsMovable, false);
        setSceneRect(m_applet->sceneBoundingRect());
        setWindowTitle(m_applet->name());
        setWindowIcon(SmallIcon(m_applet->icon()));
    } else {
        //TODO: some nice userfriendly error.
        kDebug() << "plasmoid access failed: " << job->errorString();
    }
}

void FullView::appletRemoved(Plasma::Applet *applet)
{
    if (m_applet == applet) {
        m_applet = 0;
        if (!checkShotTimer()) {
            close();
        }
    }
}

void FullView::showEvent(QShowEvent *)
{
    if (size().width() < 10 && size().height() < 10) {
        resize(400, 500);
    }
}

void FullView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    if (!m_containment) {
        return;
    }

    m_containment->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    m_containment->setMinimumSize(size());
    m_containment->setMaximumSize(size());
    m_containment->resize(size());
    if (m_containment->layout()) {
        return;
    }

    if (!m_applet) {
        return;
    }

    //kDebug() << size();
    qreal newWidth = 0;
    qreal newHeight = 0;

    if (false && m_applet->aspectRatioMode() == Plasma::KeepAspectRatio) {
        // The applet always keeps its aspect ratio, so let's respect it.
        qreal ratio = m_applet->size().width() / m_applet->size().height();
        qreal widthForCurrentHeight = (qreal)size().height() * ratio;
        if (widthForCurrentHeight > size().width()) {
            newHeight = size().width() / ratio;
            newWidth = newHeight * ratio;
        } else {
            newWidth = widthForCurrentHeight;
            newHeight = newWidth / ratio;
        }
    } else {
        newWidth = size().width();
        newHeight = size().height();
    }
    QSizeF newSize(newWidth, newHeight);

    // check if the rect is valid, or else it seems to try to allocate
    // up to infinity memory in exponential increments
    if (newSize.isValid()) {
        m_applet->resize(QSizeF(newWidth, newHeight));
        setSceneRect(m_applet->sceneBoundingRect());
    }
}

void FullView::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    qApp->quit();
}

void FullView::appletTransformedItself()
{
    resize(m_applet->size().toSize());
    setSceneRect(m_applet->sceneBoundingRect());
}

void FullView::sceneRectChanged(const QRectF &rect)
{
    Q_UNUSED(rect)
    if (m_applet) {
        setSceneRect(m_applet->sceneBoundingRect());
    }
}

bool FullView::hasStorageGroupFor(Plasma::Applet *applet) const
{
    KConfigGroup stored = KConfigGroup(KGlobal::config(), "StoredApplets");
    return stored.groupList().contains(applet->pluginName());
}

KConfigGroup FullView::storageGroup(Plasma::Applet *applet) const
{
    KConfigGroup stored = KConfigGroup(KGlobal::config(), "StoredApplets");
    return KConfigGroup(&stored, applet->pluginName());
}

void FullView::storeCurrentApplet()
{
    if (m_applet && m_persistentConfig) {
        KConfigGroup cg;
        m_applet->save(cg);
        cg = m_applet->config();
        KConfigGroup storage = storageGroup(m_applet);
        storage.deleteGroup();
        cg.copyTo(&storage);
        KGlobal::config()->sync();
    }
}

#include "fullview.moc"

