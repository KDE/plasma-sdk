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

#include <QApplication>
#include <KAboutData>
#include <KLocalizedString>

#include <Plasma/PluginLoader>
#include <qcommandlineparser.h>
#include <qcommandlineoption.h>

#include "engineexplorer.h"

static const char description[] = I18N_NOOP("Explore the data published by Plasma DataEngines");
static const char version[] = "5.2.90";

void listEngines()
{
    int maxLen = 0;
    QMap<QString, QString> engines;
    foreach (const KPluginInfo &info, Plasma::PluginLoader::listEngineInfo()) {
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
    KAboutData aboutData("plasmaengineexplorer", i18n("Plasma Engine Explorer"),
                         version, i18n(description), KAboutLicense::GPL,
                         i18n("(c) 2006, The KDE Team"));
    aboutData.addAuthor(i18n("Aaron J. Seigo"),
                        i18n( "Author and maintainer" ),
                        "aseigo@kde.org");
    aboutData.setProgramIconName("plasma");

    QApplication app(argc, argv);

    QCommandLineParser parser;
    app.setApplicationVersion(version);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(QCommandLineOption(QStringList() << "list", i18n("Displays a list of known engines and their descriptions")));
    parser.addOption(QCommandLineOption(QStringList() << "height", i18n("The desired height in pixels"), "pixels"));
    parser.addOption(QCommandLineOption(QStringList() << "width", i18n("The desired width in pixels"), "pixels"));
    parser.addOption(QCommandLineOption(QStringList() << "x", i18n("The desired x position in pixels"), "pixels"));
    parser.addOption(QCommandLineOption(QStringList() << "y", i18n("The desired y position in pixels"), "pixels"));
    parser.addOption(QCommandLineOption(QStringList() << "engine", i18n("The data engine to use"), "data engine"));
    parser.addOption(QCommandLineOption(QStringList() << "source", i18n("The source to request"), "data engine"));
    parser.addOption(QCommandLineOption(QStringList() << "interval", i18n("Update interval in milliseconds"), "ms"));
    parser.addOption(QCommandLineOption(QStringList() << "app", i18n("Only show engines associated with the parent application; "
                                           "maps to the X-KDE-ParentApp entry in the DataEngine's .desktop file."), "application"));


    parser.process(app);

    if (parser.isSet("list")) {
        listEngines();
        return 0;
    }

    EngineExplorer* w = new EngineExplorer;

    bool ok1, ok2 = false;
    //get size
    int x = parser.value("height").toInt(&ok1);
    int y = parser.value("width").toInt(&ok2);
    if (ok1 && ok2) {
        w->resize(x,y);
    }

    //get pos if available
    x = parser.value("x").toInt(&ok1);
    y = parser.value("y").toInt(&ok2);
    if (ok1 && ok2) {
        w->move(x,y);
    }

    //set interval
    int interval = parser.value("interval").toInt(&ok1);
    if (ok1) {
        w->setInterval(interval);
    }

    //set engine
    QString engine = parser.value("engine");
    if (!engine.isEmpty()) {
        w->setEngine(engine);

        QString source = parser.value("source");
        if (!source.isEmpty()) {
            w->requestSource(source);
        }
    }

    if (parser.isSet("app")) {
        w->setApp(parser.value("app"));
    }

    w->show();
    return app.exec();
}
