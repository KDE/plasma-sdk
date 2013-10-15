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

#include <QApplication>
#include <KAboutData>
#include <KLocalizedString>

#include <qcommandlineparser.h>
#include <qcommandlineoption.h>

#include "view.h"

static const char version[] = "2.0";
static const char description[] = I18N_NOOP("Run Plasma widgets in their own window");

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    app.setApplicationVersion(version);

    parser.addVersionOption();

    parser.addOption(QCommandLineOption(QStringList() << "c" << "containment", i18n("The name of the containment plugin [null]"), "containment"));
    parser.addOption(QCommandLineOption(QStringList() << "a" << "applet", i18n("The name of the applet plugin [null]"), "applet"));
    parser.addOption(QCommandLineOption(QStringList() << "f" << "formfactor",
                i18n("The formfactor to use (horizontal, vertical, mediacenter, planar or application) [planar]"), "formfactor"));
    parser.addOption(QCommandLineOption(QStringList() << "l" << "location",
                i18n("The location constraint to start the Containment with (floating, desktop, fullscreen, top, bottom, left, right) [floating]"), "location"));

    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    View *v = new View(View::s_createCorona());

    if (parser.isSet("containment")) {
        v->addContainment(parser.value("containment"));
    } else {
        v->addContainment("org.kde.desktopcontainment");
    }

    v->addFormFactor(parser.value("formfactor"));
    v->addApplet(parser.value("applet"));
    v->addLocation(parser.value("location"));

    v->show();

    return app.exec();
}

#include "main.moc"

