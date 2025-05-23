// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "../plasma-sdk-version.h"
#include "lnflogic.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <KAboutData>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

using namespace Qt::StringLiterals;

int main(int argc, char **argv)
{
    QCommandLineParser parser;
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("org.kde.plasma.lookandfeelexplorer");

    KAboutData aboutData("lookandfeelexplorer",
                         i18nc("@info:credit", "Plasma Global Theme Explorer"),
                         QStringLiteral(PLASMA_SDK_VERSION_STRING),
                         i18nc("@info:credit", "Explore and edit your Plasma Global Themes"),
                         KAboutLicense::GPL_V2,
                         i18n("© KDE Community"));
    aboutData.addAuthor(i18nc("@info:credit", "Marco Martin"), i18nc("@info:credit", "Developer"));
    aboutData.addAuthor(i18nc("@info:credit", "Carl Schwan"),
                        i18nc("@info:credit", "Developer"),
                        u"carl@carlschwan.eu"_s,
                        u"https://carlschwan.eu"_s,
                        QUrl(u"https://carlschwan.eu/avatar.png"_s));
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    aboutData.setDesktopFileName(u"org.kde.plasma.lookandfeelexplorer"_s);

    KAboutData::setApplicationData(aboutData);

    QGuiApplication::setWindowIcon(QIcon::fromTheme(u"preferences-desktop-theme"_s));

    QCommandLineOption themeOption({u"l"_s, u"lookandfeel"_s}, i18n("Look And Feel to open"), u"lookandfeel"_s);
    parser.addOption(themeOption);

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    QQmlApplicationEngine engine;

    KLocalization::setupLocalizedContext(&engine);

    engine.loadFromModule(u"org.kde.plasma.lookandfeelexplorer"_s, u"Main"_s);

    auto lnfLogic = engine.singletonInstance<LnfLogic *>(u"org.kde.plasma.lookandfeelexplorer"_s, u"LnfLogic"_s);

    const auto commandlineTheme = parser.value(themeOption);
    if (!commandlineTheme.isEmpty()) {
        lnfLogic->setTheme(commandlineTheme);
    }

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
