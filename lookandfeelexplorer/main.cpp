// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "../plasma-sdk-version.h"
#include "lnflistmodel.h"
#include "lnflogic.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>

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
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    KAboutData::setApplicationData(aboutData);

    QCommandLineOption themeOption({u"l"_s, u"lookandfeel"_s}, i18n("Look And Feel to open"), u"lookandfeel"_s);
    parser.addOption(themeOption);

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    QQmlApplicationEngine engine;

    KLocalization::setupLocalizedContext(&engine);

    // usually we have an ApplicationWindow here, so we do not need to create a window by ourselves
    engine.rootContext()->setContextProperty("commandlineTheme", parser.value(themeOption));
    engine.rootContext()->setContextProperty("commandlineArguments", parser.positionalArguments());

    engine.loadFromModule(u"org.kde.plasma.lookandfeelexplorer"_s, u"Main"_s);

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
