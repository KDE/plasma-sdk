/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "thememodel.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>

#include <KAboutData>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setOrganizationName(u"KDE"_s);
    app.setOrganizationDomain(u"kde.org"_s);

    QCommandLineParser parser;
    KAboutData aboutData("plasma-themeexplorer",
                         i18n("Plasma Theme Explorer"),
                         QStringLiteral(PROJECT_VERSION),
                         i18n("Explore and edit your Plasma themes"),
                         KAboutLicense::GPL_V2);
    aboutData.addAuthor(u"Macro Martin"_s, {}, u"mart@kde.org"_s);
    KAboutData::setApplicationData(aboutData);
    aboutData.setupCommandLine(&parser);
    app.setDesktopFileName(u"org.kde.plasma.themeexplorer"_s);

    QCommandLineOption themeOption(QCommandLineOption(QStringList{"t", "theme"}, i18n("The theme to open"), "default"));

    parser.addOption(themeOption);
    aboutData.processCommandLine(&parser);
    parser.process(app);

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("org.kde.plasma.themeexplorer"));

    QQmlApplicationEngine engine;
    auto themeModel = new ThemeModel(&engine);
    if (parser.isSet(themeOption)) {
        themeModel->setTheme(parser.value(themeOption));
        engine.rootContext()->setContextProperty("commandlineTheme", parser.value(themeOption));
    } else {
        themeModel->setTheme("default");
        engine.rootContext()->setContextProperty("commandlineTheme", "default");
    }
    engine.rootContext()->setContextProperty("themeModel", QVariant::fromValue(themeModel));

    engine.rootContext()->setContextObject(new KLocalizedQmlContext(&engine));
    engine.loadFromModule("org.kde.plasma.themeexplorer", "Main");

    return app.exec();
}
