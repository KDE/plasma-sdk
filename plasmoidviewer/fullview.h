/*
 * Copyright 2007 Aaron Seigo <aseigo@kde.org
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

#ifndef FULLVIEW_H
#define FULLVIEW_H

#include <Plasma/Applet>

#include <Plasma/Corona>

#include <QGraphicsView>

class QTimer;

namespace Plasma
{
    class AccessAppletJob;
}

class FullView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit FullView(const QString &formfactor = "planar", const QString &location = "floating", bool persistentConfig = false, QWidget *parent = 0);
    ~FullView();

    void addApplet(const QString &name, const QString& containment,
                   const QString& wallpaper, const QVariantList &args = QVariantList());
    void screenshotAll();

protected:
    void showEvent(QShowEvent *event);

private Q_SLOTS:
    void appletTransformedItself();
    void sceneRectChanged(const QRectF &rect);
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
    void appletRemoved(Plasma::Applet *applet);
    void plasmoidAccessFinished(Plasma::AccessAppletJob *job);
    void screenshotPlasmoid();

private:
    void shootNextPlasmoid();
    bool checkShotTimer();
    KConfigGroup storageGroup(Plasma::Applet *applet) const;
    bool hasStorageGroupFor(Plasma::Applet *applet) const;
    void storeCurrentApplet();

    Plasma::Corona m_corona;
    Plasma::FormFactor m_formfactor;
    Plasma::Location m_location;
    Plasma::Containment *m_containment;
    Plasma::Applet *m_applet;
    QStringList m_appletsToShoot;
    QTimer *m_appletShotTimer;
    // passed through cli by default, config files
    // are non-persistent in plasmoidviewer.
    bool m_persistentConfig;
};

#endif

