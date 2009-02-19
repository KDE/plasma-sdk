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

#include "../previewer.h"

#include <QPixmapCache>

#include <KApplication>
#include <KAboutData>
#include <KAction>
#include <KCmdLineArgs>
#include <KLocale>
#include <KStandardAction>

// for --list
#include <Plasma/Applet>
#include <iostream>

using namespace Plasma;

static const char description[] = I18N_NOOP("Test plasma-studio previewer");

int main(int argc, char **argv)
{
    KAboutData aboutData("previewer", 0, ki18n("Plasma-Studio Previewer"),
                         "1.0", ki18n(description), KAboutData::License_BSD,
                         ki18n("XXXX"));
    aboutData.setProgramIconName("plasma");
    aboutData.addAuthor(ki18n("XXX"),
                         ki18n("Original author"),
                        "xxx@kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("list", ki18n("Displays a list of known applets"));
    options.add("f");
    options.add("formfactor <name>", ki18nc("Do not translate horizontal, vertical, mediacenter nor planar", "The formfactor to use (horizontal, vertical, mediacenter or planar)"), "planar");
    options.add("l");
    options.add("location <name>", ki18nc("Do not translate floating, desktop, fullscreen, top, bottom, left nor right", "The location constraint to start the Containment with (floating, desktop, fullscreen, top, bottom, left, right)"), "floating");
    options.add("c");
    options.add("containment <name>", ki18n("Name of the containment plugin"), "null");
    options.add("w");
    options.add("wallpaper <name>", ki18n("Name of the wallpaper plugin"), QByteArray());
    options.add("p");
    options.add("pixmapcache <size>", ki18n("The size in KB to set the pixmap cache to"));
    options.add("+applet", ki18n("Name of applet to add (required)"));
    options.add("+[args]", ki18n("Optional arguments of the applet to add"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs() ;

    if (args->isSet("list")) {
        int maxLen = 0;
        QMap<QString, QString> applets;
        foreach (const KPluginInfo &info, Plasma::Applet::listAppletInfo()) {
            if (info.property("NoDisplay").toBool())
                continue;

            int len = info.pluginName().length();
            if (len > maxLen)
                maxLen = len;

            QString name = info.pluginName();
            QString comment = info.comment();

            if(comment.isEmpty())
                comment = i18n("No description available");

            applets.insert(name, comment);
        }

        QMap<QString, QString>::const_iterator it;
        for(it = applets.constBegin(); it != applets.constEnd(); it++) {
            QString applet("%1 - %2");

            applet = applet.arg(it.key().leftJustified(maxLen, ' ')).arg(it.value());
            std::cout << applet.toLocal8Bit().data() << std::endl;
        }

        return 0;
    }

    if (args->count() == 0) {
        KCmdLineArgs::usageError(i18n("No applet name specified"));
    }

    //At this point arg(0) is always set
    QString pluginName = args->arg(0);

    QVariantList appletArgs;
    for (int i = 1; i < args->count(); ++i) {
        appletArgs << args->arg(i);
    }

    Previewer preview;
    preview.addApplet(pluginName, appletArgs);
    preview.show();

    QAction *action = KStandardAction::quit(&app, SLOT(quit()), &preview);
    preview.addAction(action);
    args->clear();

    return app.exec();
}

