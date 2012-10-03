/*
 *   Copyright 2009 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "wallpaperwidget.h"

#include <QContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QWheelEvent>

#include <KDebug>
#include <KDialog>
#include <KGlobal>
#include <KStandardAction>
#include <KAction>

#include <Plasma/Wallpaper>

WallpaperWidget::WallpaperWidget(const QString &paper, const QString &mode, QWidget *parent)
    : QWidget(parent),
      m_wallpaper(Plasma::Wallpaper::load(paper)),
      m_configDialog(0)
{
    if (m_wallpaper) {
        if (!mode.isEmpty()) {
            m_wallpaper->setRenderingMode(mode);
        }

        connect(m_wallpaper, SIGNAL(update(QRectF)), this, SLOT(updatePaper(QRectF)));
        connect(m_wallpaper, SIGNAL(configNeedsSaving()), this, SLOT(syncConfig()));
    }

    addAction(KStandardAction::quit(this, SLOT(quit()), this));
    KGlobal::ref();
}

WallpaperWidget::~WallpaperWidget()
{
}

void WallpaperWidget::quit()
{
    close();
}

void WallpaperWidget::closeEvent(QCloseEvent *)
{
    if (m_wallpaper && m_wallpaper->isInitialized()) {
        KConfigGroup config = configGroup();
        m_wallpaper->save(config);
        delete m_wallpaper;
        m_wallpaper = 0;
    }

    KGlobal::deref();
}

void WallpaperWidget::paintEvent(QPaintEvent *event)
{
    if (m_wallpaper) {
        if (!m_wallpaper->isInitialized()) {
            // delayed paper initialization
            m_wallpaper->restore(configGroup());
        }

        QPainter p(this);
        m_wallpaper->paint(&p, event->rect());
    }
}

void WallpaperWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    if (m_wallpaper) {
        m_wallpaper->setBoundingRect(rect());
    }
}

void WallpaperWidget::mousePressEvent(QMouseEvent *event)
{
    if (m_wallpaper && m_wallpaper->isInitialized()) {
        m_mousePressPoint = event->pos();
        m_mousePressScreenPoint = event->globalPos();
        m_mouseMovePoint = m_mousePressPoint;
        m_mouseMoveScreenPoint = m_mousePressScreenPoint;

        QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
        mouseEvent.setWidget(this);
        mouseEvent.setButtonDownScenePos(event->button(), event->pos());
        mouseEvent.setButtonDownScreenPos(event->button(), event->pos());
        mouseEvent.setScenePos(m_mousePressPoint);
        mouseEvent.setScreenPos(m_mousePressScreenPoint);
        mouseEvent.setLastScenePos(m_mousePressPoint);
        mouseEvent.setLastScreenPos(m_mousePressScreenPoint);
        mouseEvent.setButtons(event->buttons());
        mouseEvent.setButton(event->button());
        mouseEvent.setModifiers(event->modifiers());
        mouseEvent.setAccepted(false);

        m_wallpaper->mousePressEvent(&mouseEvent);

        if (mouseEvent.isAccepted()) {
            return;
        }
    }

    QWidget::mousePressEvent(event);
}

void WallpaperWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_wallpaper && m_wallpaper->isInitialized()) {
        QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
        mouseEvent.setWidget(this);
        mouseEvent.setButtonDownScenePos(event->button(), m_mousePressPoint);
        mouseEvent.setButtonDownScreenPos(event->button(), m_mousePressScreenPoint);
        mouseEvent.setScenePos(event->pos());
        mouseEvent.setScreenPos(event->globalPos());
        mouseEvent.setLastScenePos(m_mouseMovePoint);
        mouseEvent.setLastScreenPos(m_mouseMoveScreenPoint);
        mouseEvent.setButtons(event->buttons());
        mouseEvent.setButton(event->button());
        mouseEvent.setModifiers(event->modifiers());
        mouseEvent.setAccepted(false);
        m_mouseMovePoint = event->pos();
        m_mouseMoveScreenPoint = event->globalPos();

        m_wallpaper->mouseMoveEvent(&mouseEvent);

        if (mouseEvent.isAccepted()) {
            return;
        }
    }

    QWidget::mouseMoveEvent(event);
}

void WallpaperWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_wallpaper && m_wallpaper->isInitialized()) {
        QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
        mouseEvent.setWidget(this);
        mouseEvent.setButtonDownScenePos(event->button(), m_mousePressPoint);
        mouseEvent.setButtonDownScreenPos(event->button(), m_mousePressScreenPoint);
        mouseEvent.setScenePos(event->pos());
        mouseEvent.setScreenPos(event->globalPos());
        mouseEvent.setLastScenePos(m_mouseMovePoint);
        mouseEvent.setLastScreenPos(m_mouseMoveScreenPoint);
        mouseEvent.setButtons(event->buttons());
        mouseEvent.setButton(event->button());
        mouseEvent.setModifiers(event->modifiers());
        mouseEvent.setAccepted(false);

        m_wallpaper->mouseReleaseEvent(&mouseEvent);

        if (mouseEvent.isAccepted()) {
            return;
        }
    }

    QWidget::mouseReleaseEvent(event);
}

void WallpaperWidget::wheelEvent(QWheelEvent *event)
{
    if (m_wallpaper && m_wallpaper->isInitialized()) {
        QGraphicsSceneWheelEvent wheelEvent(QEvent::GraphicsSceneWheel);
        wheelEvent.setWidget(this);
        wheelEvent.setScenePos(event->pos());
        wheelEvent.setScreenPos(event->globalPos());
        wheelEvent.setButtons(event->buttons());
        wheelEvent.setModifiers(event->modifiers());
        wheelEvent.setDelta(event->delta());
        wheelEvent.setOrientation(event->orientation());
        wheelEvent.setAccepted(false);

        m_wallpaper->wheelEvent(&wheelEvent);

        if (wheelEvent.isAccepted()) {
            return;
        }
    }

    QWidget::wheelEvent(event);
}

void WallpaperWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu m;
    if (m_wallpaper) {
        QList<QAction *> actions = m_wallpaper->contextualActions();
        foreach (QAction *action, actions) {
            m.addAction(action);
        }

        if (actions.count() > 1) {
            m.addSeparator();
        }
    }

    m.addAction(KStandardAction::preferences(this, SLOT(configure()), &m));
    m.exec(event->globalPos());
}

void WallpaperWidget::updatePaper(const QRectF &exposedRect)
{
    update(exposedRect.toRect());
}

void WallpaperWidget::configure()
{
    if (m_wallpaper) {
        if (!m_wallpaper->isInitialized()) {
            // delayed paper initialization
            m_wallpaper->restore(configGroup());
        }
        
        if (!m_configDialog) {
            m_configDialog = new KDialog(this);
            m_configDialog->setCaption(i18n("Configure %1 Wallpaper", m_wallpaper->name()));
            m_configDialog->setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);

            QWidget *w = new WallpaperConfigWidget(m_configDialog);
            QVBoxLayout *layout = new QVBoxLayout(w);
            layout->addWidget(m_wallpaper->createConfigurationInterface(w));
            m_configDialog->setMainWidget(w);

            connect(w, SIGNAL(modified(bool)), m_configDialog, SLOT(enableButtonApply(bool)));
            connect(m_configDialog, SIGNAL(applyClicked()), this, SLOT(saveConfig()));
            connect(m_configDialog, SIGNAL(okClicked()), this, SLOT(saveConfig()));
            connect(m_configDialog, SIGNAL(finished()), this, SLOT(configDone()));
        }

        m_configDialog->show();
        m_configDialog->raise();
    }
}

void WallpaperWidget::saveConfig()
{
    if (m_wallpaper) {
        KConfigGroup config = configGroup();
        m_wallpaper->save(config);
        m_wallpaper->restore(config);
    }
}

void WallpaperWidget::syncConfig()
{
    KGlobal::config()->sync();
}

void WallpaperWidget::configDone()
{
    m_configDialog->deleteLater();
    m_configDialog = 0;
}

KConfigGroup WallpaperWidget::configGroup()
{
    KConfigGroup wallpaperConfig(KGlobal::config(), "Wallpaper");

    if (m_wallpaper) {
        wallpaperConfig = KConfigGroup(&wallpaperConfig, m_wallpaper->pluginName());
    }

    return wallpaperConfig;
}

#include "wallpaperwidget.moc"

