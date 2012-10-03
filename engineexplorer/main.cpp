/*
 *   Copyright 2006 Aaron Seigo <aseigo@kde.org>
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

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>

#include <Plasma/DataEngineManager>

#include "engineexplorer.h"

static const char description[] = I18N_NOOP("Explore the data published by Plasma DataEngines");
static const char version[] = "0.2";

void listEngines()
{
    int maxLen = 0;
    QMap<QString, QString> engines;
    foreach (const KPluginInfo &info, Plasma::DataEngineManager::listEngineInfo()) {
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

        engines.insert(name, comment);
    }

    QMap<QString, QString>::const_iterator it;
    for (it = engines.constBegin(); it != engines.constEnd(); ++it) {
        QString engine("%1 - %2");
        engine = engine.arg(it.key().leftJustified(maxLen, ' ')).arg(it.value());
        std::cout << engine.toLocal8Bit().data() << std::endl;
    }
}

int main(int argc, char **argv)
{
    KAboutData aboutData("plasmaengineexplorer", 0, ki18n("Plasma Engine Explorer"),
                         version, ki18n(description), KAboutData::License_GPL,
                         ki18n("(c) 2006, The KDE Team"));
    aboutData.addAuthor(ki18n("Aaron J. Seigo"),
                        ki18n( "Author and maintainer" ),
                        "aseigo@kde.org");
    aboutData.setProgramIconName("plasma");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("list", ki18n("Displays a list of known engines and their descriptions"));
    options.add("height <pixels>", ki18n("The desired height in pixels"));
    options.add("width <pixels>", ki18n("The desired width in pixels"));
    options.add("x <pixels>", ki18n("The desired x position in pixels"));
    options.add("y <pixels>", ki18n("The desired y position in pixels"));
    options.add("engine <data engine>", ki18n("The data engine to use"));
    options.add("source <data engine>", ki18n("The source to request"));
    options.add("interval <ms>", ki18n("Update interval in milliseconds"));
    options.add("app <application>", ki18n("Only show engines associated with the parent application; "
                                           "maps to the X-KDE-ParentApp entry in the DataEngine's .desktop file."));

    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (args->isSet("list")) {
        listEngines();
        return 0;
    }

    KApplication app;
    EngineExplorer* w = new EngineExplorer;

    bool ok1, ok2 = false;
    //get size
    int x = args->getOption("height").toInt(&ok1);
    int y = args->getOption("width").toInt(&ok2);
    if (ok1 && ok2) {
        w->resize(x,y);
    }

    //get pos if available
    x = args->getOption("x").toInt(&ok1);
    y = args->getOption("y").toInt(&ok2);
    if (ok1 && ok2) {
        w->move(x,y);
    }

    //set interval
    int interval = args->getOption("interval").toInt(&ok1);
    if (ok1) {
        w->setInterval(interval);
    }

    //set engine
    QString engine = args->getOption("engine");
    if (!engine.isEmpty()) {
        w->setEngine(engine);

        QString source = args->getOption("source");
        if (!source.isEmpty()) {
            w->requestSource(source);
        }
    }

    if (args->isSet("app")) {
        w->setApp(args->getOption("app"));
    }

    args->clear();

    w->show();
    return app.exec();
}
