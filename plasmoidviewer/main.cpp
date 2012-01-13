/*
 * Copyright 2007 Frerich Raabe <raabe@kde.org>
 * Copyright 2007-2008 Aaron Seigo <aseigo@kde.org>
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

#include <iostream>

#include <QPixmapCache>

#include <KApplication>
#include <KAboutData>
#include <KAction>
#include <KCmdLineArgs>
#include <KLocale>
#include <KStandardAction>

// for --list
#include <Plasma/Applet>
#include <Plasma/Theme>
#include <Plasma/AccessManager>
#include <Plasma/AuthorizationManager>
#include <Plasma/Wallpaper>

using namespace Plasma;

static const char description[] = I18N_NOOP("Run Plasma widgets in their own window");

class RemotePlasmoidWatcher : public QObject
{
Q_OBJECT

public:
    RemotePlasmoidWatcher(AccessManager *manager)
        : QObject(manager)
    {
        kDebug();
        connect(manager, SIGNAL(remoteAppletAnnounced(Plasma::PackageMetadata)),
                this, SLOT(slotServiceAdded(Plasma::PackageMetadata)));
        connect(manager, SIGNAL(remoteAppletUnannounced(Plasma::PackageMetadata)),
                this, SLOT(slotServiceRemoved(Plasma::PackageMetadata)));
    }

    ~RemotePlasmoidWatcher() {}

public Q_SLOTS:
    void slotServiceAdded(Plasma::PackageMetadata metadata) {
        std::cout << "New service published:" << std::endl;
        std::cout << metadata.remoteLocation().prettyUrl().toLocal8Bit().data() << std::endl;
        std::cout << metadata.name().toLocal8Bit().data() << " - "
                  << metadata.description().toLocal8Bit().data() << std::endl;
    }

    void slotServiceRemoved(Plasma::PackageMetadata metadata) {
        std::cout << "Service removed:" << std::endl;
        std::cout << metadata.remoteLocation().prettyUrl().toLocal8Bit().data() << std::endl;
    }
};

void listPlugins(const KPluginInfo::List & plugins)
{
    int maxLen = 0;
    QMap<QString, QString> applets;
    foreach (const KPluginInfo &info, plugins) {
        if (info.property("NoDisplay").toBool()) {
            continue;
        }

        int len = info.pluginName().length();
        if (len > maxLen) {
            maxLen = len;
        }

        QString name = info.pluginName();
        QString comment = info.comment();

        if (comment.isEmpty()) {
            comment = i18n("No description available");
        }

        applets.insert(name, comment);
    }

    QMap<QString, QString>::const_iterator it;
    for (it = applets.constBegin(); it != applets.constEnd(); ++it) {
        QString applet("%1 - %2");

        applet = applet.arg(it.key().leftJustified(maxLen, ' ')).arg(it.value());
        std::cout << applet.toLocal8Bit().data() << std::endl;
    }
}

int main(int argc, char **argv)
{
    KAboutData aboutData("plasmoidviewer", 0, ki18n("Plasma Widget Viewer"),
                         "1.0", ki18n(description), KAboutData::License_BSD,
                         ki18n("2007-2008, Frerich Raabe"));
    aboutData.setProgramIconName("plasma");
    aboutData.addAuthor(ki18n("Frerich Raabe"),
                         ki18n("Original author"),
                        "raabe@kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("c");
    options.add("containment <name>", ki18n("Name of the containment plugin"), "null");
    options.add("f");
    options.add("formfactor <name>", ki18nc("Do not translate horizontal, vertical, mediacenter nor planar", "The formfactor to use (horizontal, vertical, mediacenter or planar)"), "planar");
    options.add("la");
    options.add("list", ki18n("Displays a list of known applets"));
    options.add("lw");
    options.add("list-wallpapers", ki18n("Displays a list of known wallpapers"));
    options.add("lc");
    options.add("list-containments", ki18n("Displays a list of known containments"));
    options.add("nosaveconfig", ki18n("Disables save and restore of the config between runs"));
    options.add("l");
    options.add("location <name>", ki18nc("Do not translate floating, desktop, fullscreen, top, bottom, left nor right", "The location constraint to start the Containment with (floating, desktop, fullscreen, top, bottom, left, right)"), "floating");
    options.add("p");
    options.add("pixmapcache <size>", ki18n("The size in kB to set the pixmap cache to"));
    options.add("s");
    options.add("screenshot", ki18n("Takes a screenshot of the widget and saves it the working directory as <pluginname>.png"));
    options.add("sa");
    options.add("screenshot-all", ki18n("Takes a screenshot of each widget and saves it the working directory as <pluginname>.png"));
    options.add("t");
    options.add("theme <name>", ki18n("Desktop SVG theme to use"));
    options.add("w");
    options.add("wallpaper <name>", ki18n("Name of the wallpaper plugin. Requires a containment plugin to be specified."), QByteArray());
    options.add("+applet", ki18n("Name of applet to view; may refer to the plugin name or be a path "
                                "(absolute or relative) to a package. If not provided, then an "
                                "attempt is made to load a package from the current directory."));
    options.add("+[args]", ki18n("Optional arguments of the applet to add"));
    options.add("list-remote", ki18n("List zeroconf announced remote widgets"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs() ;

    if (args->isSet("list")) {
        listPlugins(Plasma::Applet::listAppletInfo());
        return 0;
    }

    if (args->isSet("list-wallpapers")) {
        listPlugins(Plasma::Wallpaper::listWallpaperInfo());
        return 0;
    }

    if (args->isSet("list-containments")) {
        listPlugins(Plasma::Containment::listContainments());
        return 0;
    }

    QString pluginName;
    if (args->count() > 0) {
        pluginName = args->arg(0);
    }

    QString formfactor = args->getOption("formfactor");
    kDebug() << "setting FormFactor to" << args->getOption("formfactor");

    QString location = args->getOption("location");
    kDebug() << "setting Location to" << args->getOption("location");

    QString containment = args->getOption("containment");
    kDebug() << "setting containment to" << containment;

    if (args->isSet("theme")) {
        QString theme = args->getOption("theme");
        Plasma::Theme::defaultTheme()->setUseGlobalSettings(false);
        Plasma::Theme::defaultTheme()->setThemeName(theme);
        kDebug() << "setting theme to" << theme;
    }

    QString wallpaper;
    if (args->isSet("wallpaper")) {
        wallpaper = args->getOption("wallpaper");
        kDebug() << "setting wallpaper to" << wallpaper;
    }

    if (args->isSet("pixmapcache")) {
        kDebug() << "setting pixmap cache to" << args->getOption("pixmapcache").toInt();
        QPixmapCache::setCacheLimit(args->getOption("pixmapcache").toInt());
    }

    QVariantList appletArgs;
    for (int i = 1; i < args->count(); ++i) {
        appletArgs << args->arg(i);
    }
    kDebug() << "setting auth policy";
    Plasma::AuthorizationManager::self()->setAuthorizationPolicy(Plasma::AuthorizationManager::PinPairing);

    const bool persistentConfig = args->isSet("saveconfig");
    if (!persistentConfig) {
        kWarning() << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& WARNING: Applet configuration will not be restored or saved.";
    }

    FullView view(formfactor, location, persistentConfig);

    if (args->isSet("list-remote")) {
        kDebug() << "list remote...";
        /**
        QList<KUrl> list = AccessManager::self()->remotePlasmoids();
        foreach (const KUrl &url, list) {
            std::cout << url.prettyUrl().toLocal8Bit().data() << std::endl;
        }
        */
        new RemotePlasmoidWatcher(AccessManager::self());
    } else if (args->isSet("screenshot-all")) {
        view.show();
        view.screenshotAll();
    } else {
        kDebug() << "just load applet";
        view.addApplet(pluginName, containment, wallpaper, appletArgs);
        view.show();
    }


    QAction *action = KStandardAction::quit(&app, SLOT(quit()), &view);
    view.addAction(action);

    return app.exec();
}

#include "main.moc"

