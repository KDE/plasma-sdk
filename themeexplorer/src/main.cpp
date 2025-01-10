/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "coloreditor.h"
#include "themelistmodel.h"
#include "thememodel.h"
#include <QApplication>

#include <QQuickItem>
#include <klocalizedstring.h>
#include <qcommandlineoption.h>
#include <qcommandlineparser.h>

#include <KAboutData>
#include <PlasmaQuick/SharedQmlEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlExpression>
#include <QQmlProperty>
#include <QQuickWindow>
#include <kpackage/package.h>
#include <kpackage/packageloader.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    app.setApplicationVersion(PROJECT_VERSION);
    app.setDesktopFileName(QStringLiteral("org.kde.themeexplorer"));

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription(i18n("Plasma Theme Explorer"));

    QCommandLineOption themeOption(QCommandLineOption(QStringList() << "t"
                                                                    << "theme",
                                                      i18n("The theme to open"),
                                                      "theme"));

    parser.addOption(themeOption);

    parser.process(app);

    const QString packagePath("org.kde.themeexplorer");

    // usually we have an ApplicationWindow here, so we do not need to create a window by ourselves
    auto obj = new PlasmaQuick::SharedQmlEngine();
    obj->setTranslationDomain(packagePath);
    obj->setInitializationDelayed(true);
    obj->engine()->rootContext()->setContextProperty("commandlineArguments", parser.positionalArguments());

    QObject::connect(obj->engine().get(), &QQmlEngine::quit, &app, &QApplication::quit);

    qmlRegisterAnonymousType<ThemeListModel>("org.kde.plasma.sdk", 1);
    qmlRegisterAnonymousType<ColorEditor>("org.kde.plasma.sdk", 1);

    KPackage::Package package = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("KPackage/GenericQML"));
    package.setPath(packagePath);
    obj->setSource(QUrl::fromLocalFile(package.filePath("mainscript")));
    const KPluginMetaData data = package.metadata();
    ThemeModel *themeModel = new ThemeModel(package);
    if (parser.isSet(themeOption)) {
        themeModel->setTheme(parser.value(themeOption));
        obj->engine()->rootContext()->setContextProperty("commandlineTheme", parser.value(themeOption));
    } else {
        themeModel->setTheme(parser.value("default"));
        obj->engine()->rootContext()->setContextProperty("commandlineTheme", "default");
    }
    obj->engine()->rootContext()->setContextProperty("themeModel", QVariant::fromValue(themeModel));

    obj->completeInitialization();

    if (!data.isValid()) {
        return -1;
    }

    // About data
    KAboutData aboutData(data.pluginId(), data.name(), data.version(), data.description(), KAboutLicense::byKeyword(data.license()).key());

    for (auto author : data.authors()) {
        aboutData.addAuthor(author.name(), author.task(), author.emailAddress(), author.webAddress());
    }

    // The root is not a window?
    // have to use a normal QQuickWindow since the root item is already created
    QWindow *window = qobject_cast<QWindow *>(obj->rootObject());
    if (window) {
        window->setTitle(data.name());
        window->setIcon(QIcon::fromTheme(data.iconName()));
    } else {
        qWarning() << "Error loading the ApplicationWindow";
    }

    return app.exec();
}
