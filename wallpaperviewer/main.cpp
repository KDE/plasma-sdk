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

#include <iostream>

#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KLocale>

#include <Plasma/Wallpaper>

#include "wallpaperwidget.h"

static const char description[] = I18N_NOOP("Viewer for Plasma wallpapers");
static const char version[] = "0.0";

void listWallpapers()
{
    int maxLen = 0;
    QMap<QString, QString> wallpapers;
    QHash<QString, QString> modes;

    foreach (const KPluginInfo &info, Plasma::Wallpaper::listWallpaperInfo()) {
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

        wallpapers.insert(name, comment);

        const QList<KServiceAction>& modeActions = info.service()->actions();
        QStringList modeStrings;
        if (!modeActions.isEmpty()) {
            foreach (const KServiceAction& mode, modeActions) {
                modeStrings << mode.name();
            }

            qStableSort(modeStrings);
            modes[name] = modeStrings.join(", ");
        }
    }

    QMap<QString, QString>::const_iterator it;
    QString spaces;
    spaces.fill(' ', maxLen + 3);

    if (wallpapers.isEmpty()) {
        std::cout << i18n("No wallpaper plugins installed.").toLocal8Bit().data() << std::endl << std::endl;
        return;
    }

    std::cout << i18n("Installed wallpaper plugins:").toLocal8Bit().data() << std::endl << std::endl;

    for (it = wallpapers.constBegin(); it != wallpapers.constEnd(); ++it) {
        QString wallpaper = QString("%1 - %2").arg(it.key().leftJustified(maxLen, ' ')).arg(it.value());
        std::cout << wallpaper.toLocal8Bit().data() << std::endl;

        QString modeString = modes.value(it.key());
        if (!modeString.isEmpty()) {
            std::cout << spaces.toLocal8Bit().data()
                      << i18n("Modes: %1", modeString).toLocal8Bit().data() << std::endl;
        }
    }
}

int main(int argc, char **argv)
{
    KAboutData aboutData("plasmawallpaperviewer", 0, ki18n("Plasma Wallpaper Viewer"),
                         version, ki18n(description), KAboutData::License_GPL,
                         ki18n("(c) 2009, Aaron J. Seigo"));
    aboutData.addAuthor(ki18n("Aaron J. Seigo"),
                        ki18n( "Author and maintainer" ),
                        "aseigo@kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("h");
    options.add("height <pixels>", ki18n("The desired height in pixels"));
    options.add("w");
    options.add("width <pixels>", ki18n("The desired width in pixels"));
    options.add("p");
    options.add("wallpaper <plugin name>", ki18n("The wallpaper plugin to use"), "image");
    options.add("m");
    options.add("mode <mode name>", ki18n("The mode to put the wallpaper in"), "SingleImage");
    options.add("list", ki18n("List all the known wallpapers and their modes"));
    options.add("configure", ki18n("Open configuration dialog additionally to show the wallpaper plugin"));
    KCmdLineArgs::addCmdLineOptions(options);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    KApplication app;
    app.setQuitOnLastWindowClosed(false);
    KGlobal::setAllowQuit(true);

    if (args->isSet("list")) {
        listWallpapers();
        exit(0);
    }

    // set wallpaper
    QString pluginName = args->getOption("wallpaper");
    QString mode = args->getOption("mode");
    WallpaperWidget w(pluginName, mode);

    bool ok1, ok2 = false;
    // get size
    int width = args->getOption("width").toInt(&ok2);
    int height = args->getOption("height").toInt(&ok1);
    if (ok1 && ok2) {
        w.resize(width, height);
    }

    w.show();
    if(args->isSet("configure")) {
        w.configure();
    }
    args->clear();
    return app.exec();
}
