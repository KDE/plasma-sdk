/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "globaltheme.h"
#include "globalthememodel.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlExpression>
#include <QQmlProperty>
#include <QQuickItem>
#include <QQuickWindow>

#include <KAboutData>
#include <KDeclarative/QmlObject>
#include <KLocalizedString>
#include <KPackage/Package>
#include <KPackage/PackageLoader>

int main(int argc, char **argv)
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCommandLineParser parser;
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("org.kde.plasma.globalthemeexplorer");

    KAboutData aboutData(QStringLiteral("lookandfeelexplorer"),
                         i18nc("@title", "Global Theme Explorer"),
                         QStringLiteral(PROJECT_VERSION),
                         i18nc("@title", "Explore and edit your Plasma Global Themes"),
                         KAboutLicense::LGPL_V2);

    aboutData.addAuthor(i18nc("@info:credit", "Marco Martin"), i18nc("@info:credit", "Creator"),
                        QStringLiteral("mart@kde.org"));

    aboutData.addAuthor(i18nc("@info:credit", "Carl Schwan"), i18nc("@info:credit", "Developer"),
                        QStringLiteral("carl@carlschwan.eu"), QStringLiteral("https://carlschwan.eu"));

    KAboutData::setApplicationData(aboutData);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop-theme-global")));

    aboutData.setupCommandLine(&parser);

    QCommandLineOption themeOption(QCommandLineOption(QStringList() << "l" << "lookandfeel",
                i18n("Global Theme to open"), "lookandfeel"));
    parser.addOption(themeOption);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    // Initialize QmlEngine
    QQmlApplicationEngine engine;
    qmlRegisterAnonymousType<GlobalThemeModel>("org.kde.plasma.sdk", 1);
    GlobalTheme globalTheme;
    qmlRegisterSingletonInstance<GlobalTheme>("org.kde.plasma.sdk", 1, 0, "GlobalTheme", &globalTheme);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.rootContext()->setContextProperty("commandlineTheme", parser.value(themeOption));
    engine.rootContext()->setContextProperty("commandlineArguments", parser.positionalArguments());
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}

