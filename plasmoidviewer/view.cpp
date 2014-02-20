/*
 *  Copyright 2013 Giorgos Tsiapaliokas <terietor@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QFileDialog>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQmlContext>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

#include <KLocalizedString>

#include <Plasma/Package>
#include <Plasma/PluginLoader>

#include "view.h"

View::View(Plasma::Corona *cor, bool konsoleVisible, QWindow *parent)
    : PlasmaQuick::View(cor, parent)
{
    m_konsoleVisible = konsoleVisible;
    engine()->rootContext()->setContextProperty("desktop", this);
    setSource(QUrl::fromLocalFile(cor->package().filePath("views", "Desktop.qml")));
}

View::~View()
{
}

void View::addApplet(const QString &applet)
{
    Plasma::Containment *c = containment();

    if (!c) {
        qCritical("Containment doesn't exist");
        return;
    }

    Plasma::Applet *a = containment()->createApplet(applet);
    if (!a->pluginInfo().isValid()) {
        qCritical("Applet %s doesn't exist!", qPrintable(applet));
        return;
    }
    m_lastAppletName = applet;
}

void View::addContainment(const QString &cont)
{
    Plasma::Containment *c = corona()->createContainment(cont);

    if (!c->pluginInfo().isValid()) {
        qCritical("Containment %s doesn't exist", qPrintable(cont));
        return;
    }

    setContainment(c);

    connect(containment(), &Plasma::Containment::appletRemoved, [=](Plasma::Applet *applet) {
        if (applet && applet->pluginInfo().isValid()) {
            addApplet(applet->pluginInfo().pluginName());
        }
    });
}

void View::addFormFactor(const QString &formFactor)
{
    Plasma::Types::FormFactor formFactorType = Plasma::Types::Planar;
    const QString ff = formFactor.toLower();

    if (ff.isEmpty() || ff == QStringLiteral("planar")) {
        formFactorType = Plasma::Types::Planar;
    } else if (ff == QStringLiteral("vertical")) {
        formFactorType = Plasma::Types::Vertical;
    } else if (ff == QStringLiteral("horizontal")) {
        formFactorType = Plasma::Types::Horizontal;
    } else if (ff == QStringLiteral("mediacenter")) {
        formFactorType = Plasma::Types::MediaCenter;
    } else if (ff == QStringLiteral("application")) {
        formFactorType = Plasma::Types::Application;
    } else {
        qWarning() << "FormFactor " << ff << "doesn't exist. Planar formFactor has been used!!";
    }

    Plasma::Containment *c = containment();
    if (!c) {
        qCritical("Containment doesn't exist!");
        return;
    }

    c->setFormFactor(formFactorType);
}

void View::changeFormFactor(int formFactor)
{
    QString formFactorType = "planar";
    switch (formFactor) {
        case Plasma::Types::Planar:
            formFactorType = "planar";
            break;
        case Plasma::Types::Vertical:
            formFactorType = "vertical";
            break;
        case Plasma::Types::Horizontal:
            formFactorType = "horizontal";
            break;
        case Plasma::Types::MediaCenter:
            formFactorType = "mediacenter";
            break;
        case Plasma::Types::Application:
            formFactorType = "application";
            break;
    }

    addFormFactor(formFactorType);
}

void View::addLocation(const QString &location)
{
    Plasma::Types::Location locationType = Plasma::Types::Floating;

    const QString l = location.toLower();

    if (l.isEmpty() || l == QStringLiteral("floating")) {
        locationType = Plasma::Types::Floating;
    } else if (l == QStringLiteral("desktop")) {
        locationType = Plasma::Types::Desktop;
    } else if (l == QStringLiteral("fullscreen")) {
        locationType = Plasma::Types::FullScreen;
    } else if (l == QStringLiteral("top")) {
        locationType = Plasma::Types::TopEdge;
    } else if (l == QStringLiteral("bottom")) {
        locationType = Plasma::Types::BottomEdge;
    } else if (l == QStringLiteral("right")) {
        locationType = Plasma::Types::RightEdge;
    } else if (l == QStringLiteral("left")) {
        locationType = Plasma::Types::LeftEdge;
    } else {
        qWarning() << "Location " << l << "doesn't exist. Floating location has been used!!";
    }

    Plasma::Containment *c = containment();
    if (!c) {
        qCritical("Containment doesn't exist!");
        return;
    }

    setLocation(locationType);
}

bool View::konsoleVisible()
{
    return m_konsoleVisible;
}

void View::changeLocation(int location)
{
    QString locationType = "floating";
    switch (location) {
        case Plasma::Types::Floating:
            locationType = "floating";
            break;
        case Plasma::Types::Desktop:
            locationType = "desktop";
            break;
        case Plasma::Types::FullScreen:
            locationType = "fullscreen";
            break;
        case Plasma::Types::TopEdge:
            locationType = "topedge";
            break;
        case Plasma::Types::BottomEdge:
            locationType = "bottomedge";
            break;
        case Plasma::Types::RightEdge:
            locationType = "rightedge";
            break;
        case Plasma::Types::LeftEdge:
            locationType = "leftedge";
            break;
    }

    addLocation(locationType);
}

Plasma::Corona *View::createCorona()
{
    Plasma::Package package = Plasma::PluginLoader::self()->loadPackage("Plasma/Shell");
    package.setPath("org.kde.plasmoidviewershell");

    Plasma::Corona *cor = new Plasma::Corona();
    cor->setPackage(package);

    return cor;
}

void View::takeScreenShot()
{
    QDBusInterface interface(QStringLiteral("org.kde.kwin"), QStringLiteral("/Screenshot"),
                             QStringLiteral("org.kde.kwin.Screenshot"));

    QDBusPendingCall async = interface.asyncCall(QStringLiteral("screenshotArea"), x(), y(),
                                                 width(), height());

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher,&QDBusPendingCallWatcher::finished, [](QDBusPendingCallWatcher *call) {
        QDBusPendingReply<QString> reply = *call;
        call->deleteLater();

        if (!reply.isValid()) {
            qDebug() << "The screenshot has failed, the reply is invalid with error" << reply.error().message();
            return;
        }

        QString dest = QFileDialog::getSaveFileName(0,i18n("Save Screenshot"),
                                                    QDir::homePath(), QStringLiteral("Images (.*png)"));

        if (dest.isEmpty()) {
            return;
        }

        if (!dest.endsWith(QStringLiteral(".png"))) {
            dest.append(QStringLiteral(".png"));
        }

        QFile f(reply.value());
        f.rename(dest);
    });
}
#include "moc_view.cpp"
