/*
 * Copyright 2007 Frerich Raabe <raabe@kde.org>
 * Copyright 2007 Aaron Seigo <aseigo@kde.org>
 * Copyright 2008 Aleix Pol <aleixpol@gmail.com>
 * Copyright 2009 Artur Duque de Souza <morpheuz@gmail.com>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
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


#include "previewcontainment.h"

#include <QGraphicsLayoutItem>
#include <QWidget>

#include <KAction>
#include <KConfigGroup>
#include <KDebug>
#include <KDesktopFile>
#include <KIcon>
#include <KMenu>
#include <KStandardDirs>
#include <Plasma/IconWidget>
#include <Plasma/Label>
#include <Plasma/Package>
#include <Plasma/Wallpaper>

PreviewContainment::PreviewContainment(QObject *parent, const QVariantList &args)
        : Containment(parent, args),
          m_applet(0),
          m_menu(new KMenu())
{
    setContainmentType(Plasma::Containment::CustomContainment);
    setBackgroundHints(Plasma::Applet::NoBackground);
    setHasConfigurationInterface(false);

    connect(this, SIGNAL(appletAdded(Plasma::Applet *, const QPointF &)),
            this, SLOT(onAppletAdded(Plasma::Applet *, const QPointF &)));
    connect(this, SIGNAL(appletRemoved(Plasma::Applet *)),
            this, SLOT(onAppletRemoved(Plasma::Applet *)));

    m_layout = new QGraphicsLinearLayout(Qt::Vertical);
    m_layout->setContentsMargins(5, 0, 5, 10);

    m_header = new QGraphicsLinearLayout(Qt::Horizontal);
    m_header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_header->setSpacing(0);
    m_header->setContentsMargins(0, 4, 0, 0);

    m_controls = new QQuickItem(this);
    m_controls->setLayout(m_header);
    m_controls->setZValue(99);
    m_controls->resize(100, 32);
    m_controls->setMaximumHeight(32);
    m_layout->addItem(m_controls);
    setupHeader();

    m_tmp = new QQuickItem(this);
    m_layout->addItem(m_tmp);

    setLayout(m_layout);
    //setWallpaper("image", "Color");
}

PreviewContainment::~PreviewContainment()
{
    delete m_menu;
}

void PreviewContainment::setupHeader()
{
    // setup toolboxes
    Plasma::IconWidget *form = new Plasma::IconWidget(this);
    Plasma::IconWidget *refresh = new Plasma::IconWidget(this);
    Plasma::IconWidget *location = new Plasma::IconWidget(this);
    Plasma::IconWidget *wallpaper = new Plasma::IconWidget(this);
    Plasma::IconWidget *konsole = new Plasma::IconWidget(this);

    // add actions
    KAction *action0 = new KAction(KIcon("user-desktop"), "", this);
    connect(action0, SIGNAL(triggered()), this, SLOT(changeWallpaper()));
    wallpaper->setAction(action0);

    KAction *action1 = new KAction(KIcon("distribute-horizontal-x"), "", this);
    connect(action1, SIGNAL(triggered()), this, SLOT(changeFormFactor()));
    form->setAction(action1);

    KAction *action2 = new KAction(KIcon("align-vertical-bottom"), "", this);
    connect(action2, SIGNAL(triggered()), this, SLOT(changeLocation()));
    location->setAction(action2);

    KAction *action3 = new KAction(KIcon("view-refresh"), "", this);
    connect(action3, SIGNAL(triggered()), this, SLOT(refreshAndEmit()));
    refresh->setAction(action3);

    KAction *action4 = new KAction(KIcon("utilities-terminal"), "", this);
    connect(action4, SIGNAL(triggered()), this, SIGNAL(showKonsole()));
    konsole->setAction(action4);

    //add the items to the toolboxes
    //we will need them in order to modify them later.
    m_toolbox.insert("refresh", refresh);
    m_toolbox.insert("form", form);
    m_toolbox.insert("location", location);
    m_toolbox.insert("wallpaper", wallpaper);
    m_toolbox.insert("konsole", konsole);

    // add the toolboxes
    m_header->addItem(refresh);
    m_header->addItem(form);
    m_header->addItem(location);
    m_header->addItem(wallpaper);
    m_header->addItem(konsole);
}

void PreviewContainment::setKonsolePreviewerVisible(bool visible)
{
    int count = 0;
    QHashIterator<QString, Plasma::IconWidget*> it(m_toolbox);
    while (it.hasNext()) {
        it.next();
        if (it.key() != "konsole") {
            count ++;
        }
    }

    if (visible == true) {
        m_toolbox["konsole"]->setVisible(true);
        if (!m_header->itemAt(count)) {
            m_header->addItem(m_toolbox["konsole"]);
        }
    } else {
        m_toolbox["konsole"]->setVisible(false);
        if (m_header->itemAt(count)) {
            m_header->removeItem(m_toolbox["konsole"]);
        }
    }
}

QString PreviewContainment::plasmoidApi()
{
    QString plasmaterc = KStandardDirs::locateLocal("config", "plasmaterc");
    if (plasmaterc.isEmpty()) {
        return QString();
    }

    KConfig c(plasmaterc);
    KConfigGroup projectrcPreferences(&c, "PackageModel::package");
    QString projectPath = projectrcPreferences.readEntry("lastLoadedPackage", QString());
    if (projectPath.isEmpty()) {
        return QString();
    }

    KDesktopFile desktopFile(projectPath + "/metadata.desktop");
    QString projectApi = desktopFile.desktopGroup().readEntry("X-Plasma-API", QString());
    return projectApi;
}


// Fixme(?): Does not currently respect arguments passed in during
// initialization. Will reload applet without arguments
void PreviewContainment::refreshApplet()
{
    if (!m_applet)
        return;
    clearApplets();

    if (m_applet->package()) {
        QString path = m_applet->package()->path();
        delete m_applet;
        m_applet = Applet::loadPlasmoid(path);
        addApplet(m_applet, QPointF(-1, -1), false);
    } else {
        QString pluginName = m_applet->pluginName();
        delete m_applet;
        m_applet = addApplet(pluginName,
                             QVariantList(), QRectF(0, 0, -1, -1));
    }

    m_applet->setFlag(QGraphicsItem::ItemIsMovable, false);
}

void PreviewContainment::refreshAndEmit() {
    // improve me?
    // basically if we're running inside plasmate, we want
    // to show the latest state of the code, so we emit a
    // signal that tells the main window to save its contents
    // and then do one more refresh
    refreshApplet();
    emit refreshClicked();
}

void PreviewContainment::changeFormFactor()
{
    initMenu(i18n("Form Factor"));
    connect(m_menu, SIGNAL(triggered(QAction*)), this, SLOT(formFactorSelected(QAction*)));

    QHash<Plasma::FormFactor, QAction *> actions;
    actions.insert(Plasma::Planar, m_menu->addAction(KIcon("user-desktop"), i18n("Planar")));
    actions.insert(Plasma::MediaCenter, m_menu->addAction(KIcon("video-television"), i18n("Media Center")));
    actions.insert(Plasma::Horizontal, m_menu->addAction(KIcon("distribute-horizontal-x"), i18n("Horizontal")));
    actions.insert(Plasma::Vertical, m_menu->addAction(KIcon("distribute-vertical-y"), i18n("Vertical")));

    QHashIterator<Plasma::FormFactor, QAction *> it(actions);
    while (it.hasNext()) {
        it.next();
        it.value()->setCheckable(true);
        it.value()->setData(it.key());
    }

    if (actions.contains(formFactor())) {
        actions[formFactor()]->setChecked(true);
    }

    showMenu();
}

void PreviewContainment::formFactorSelected(QAction *action)
{
    setFormFactor(static_cast<Plasma::FormFactor>(action->data().toInt()));
}

void PreviewContainment::changeWallpaper()
{
    initMenu(i18n("Wallpaper"));
    connect(m_menu, SIGNAL(triggered(QAction*)), this, SLOT(wallpaperSelected(QAction*)));

    const QString currentPlugin = wallpaper() ? wallpaper()->pluginName() : QString();

    // create a menu item for each wallpaper plugin installed
    foreach (const KPluginInfo &info, Plasma::Wallpaper::listWallpaperInfo()) {
        QAction *plugin = m_menu->addAction(KIcon("user-desktop"), info.pluginName());
        plugin->setCheckable(true);
        plugin->setData(info.pluginName());
        if (currentPlugin == info.pluginName()) {
            plugin->setChecked(true);
        }
    }

    m_menu->addSeparator();
    m_menu->addAction(i18n("Wallpaper Settings"));
    showMenu();
}

void PreviewContainment::wallpaperSelected(QAction *action)
{
    const QString &newPlugin = action->data().toString();
    if (newPlugin.isEmpty()) {
        if (wallpaper()) {
            QWidget *widget = new QWidget();
            wallpaper()->createConfigurationInterface(widget);
            widget->show();
        }
    } else {
        setWallpaper(action->data().toString());
    }
}

void PreviewContainment::changeLocation()
{
    initMenu(i18n("Location"));
    connect(m_menu, SIGNAL(triggered(QAction*)), this, SLOT(locationSelected(QAction*)));

    QHash<Plasma::Location, QAction *> actions;
    actions.insert(Plasma::Floating, m_menu->addAction(KIcon("tab-detach"), i18n("Floating")));
    actions.insert(Plasma::Desktop, m_menu->addAction(KIcon("user-desktop"), i18n("Desktop")));
    actions.insert(Plasma::FullScreen, m_menu->addAction(KIcon("view-fullscreen"), i18n("FullScreen")));
    actions.insert(Plasma::TopEdge, m_menu->addAction(KIcon("align-vertical-top"), i18n("Top Edge")));
    actions.insert(Plasma::BottomEdge, m_menu->addAction(KIcon("align-vertical-bottom"), i18n("Bottom Edge")));
    actions.insert(Plasma::LeftEdge, m_menu->addAction(KIcon("align-horizontal-left"), i18n("Left Edge")));
    actions.insert(Plasma::RightEdge, m_menu->addAction(KIcon("align-horizontal-right"), i18n("Right Edge")));

    QHashIterator<Plasma::Location, QAction *> it(actions);
    while (it.hasNext()) {
        it.next();
        it.value()->setCheckable(true);
        it.value()->setData(it.key());
    }

    if (actions.contains(location())) {
        actions[location()]->setChecked(true);
    }

    showMenu();
}

void PreviewContainment::locationSelected(QAction *action)
{
    setLocation(static_cast<Plasma::Location>(action->data().toInt()));
}

void PreviewContainment::constraintsEvent(Plasma::Constraints constraints)
{
    Q_UNUSED(constraints);
}

void PreviewContainment::onAppletAdded(Plasma::Applet *applet, const QPointF &pos)
{
    Q_UNUSED(pos);
    m_applet = applet;
    m_layout->removeItem(m_tmp);

    m_layout->addItem(applet);
    if (plasmoidApi() != "declarativeappletscript") {
        //python bindings don't support kDebug, so the konsole previewer
        //cannot return any debug code. So until this is fixed,
        //we disable the konsolepreviewer support for the python bindings.
        setKonsolePreviewerVisible(false);
    } else {
        setKonsolePreviewerVisible(true);
    }
}

void PreviewContainment::onAppletRemoved(Plasma::Applet *applet)
{
    disconnect(applet, SIGNAL(geometryChanged()), this, SLOT(onAppletGeometryChanged()));
    m_layout->removeItem(applet);
    m_layout->addItem(m_tmp);
}

void PreviewContainment::hoverEnterEvent(QHoverEvent* event)
{
    Containment::hoverEnterEvent(event);
}

void PreviewContainment::hoverLeaveEvent(QHoverEvent* event)
{
    Containment::hoverLeaveEvent(event);
}

void PreviewContainment::initMenu(const QString &title)
{
    disconnect(m_menu, 0, this, 0);
    m_menu->clear();
    m_menu->addTitle(title);
}

void PreviewContainment::showMenu()
{
    if (m_menu) {
        m_menu->popup(QCursor::pos());
    }
}

K_EXPORT_PLASMA_APPLET(studiopreviewer, PreviewContainment)

