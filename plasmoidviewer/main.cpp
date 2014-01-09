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
#include <QtQml/QQmlDebuggingEnabler>

#include <KAboutData>
#include <KLocalizedString>

#include <qcommandlineparser.h>
#include <qcommandlineoption.h>

#include "view.h"

static const char version[] = "2.0";

int main(int argc, char **argv)
{
    QQmlDebuggingEnabler debugEnabler;

    QApplication app(argc, argv);
    app.setApplicationVersion(version);

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("Run Plasma widgets in their own window"));

    parser.addVersionOption();

    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("c") << QStringLiteral("containment"),
                i18n("The name of the containment plugin"), QStringLiteral("containment"), QStringLiteral("org.kde.desktopcontainment")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("a") << QStringLiteral("applet"),
                i18n("The name of the applet plugin"), QStringLiteral("applet")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("f") << QStringLiteral("formfactor"),
                i18n("The formfactor to use (horizontal, vertical, mediacenter, planar or application)"), QStringLiteral("formfactor"), QStringLiteral("planar")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("l") << QStringLiteral("location"),
                i18n("The location constraint to start the Containment with (floating, desktop, fullscreen, top, bottom, left, right)"),
                QStringLiteral("location"), QStringLiteral("floating")));

    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    View *v = new View(View::createCorona());

    v->addContainment(parser.value("containment"));
    v->addFormFactor(parser.value("formfactor"));
    v->addApplet(parser.value("applet"));
    v->addLocation(parser.value("location"));

    v->show();

    return app.exec();
}

