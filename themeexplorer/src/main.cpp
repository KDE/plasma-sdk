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
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlExpression>
#include <QQmlProperty>
#include <QQuickWindow>
#include <kdeclarative/qmlobject.h>
#include <kpackage/package.h>
#include <kpackage/packageloader.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    app.setApplicationVersion(PROJECT_VERSION);

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

    const QString packagePath("org.kde.plasma.themeexplorer");

    // usually we have an ApplicationWindow here, so we do not need to create a window by ourselves
    KDeclarative::QmlObject *obj = new KDeclarative::QmlObject();
    obj->setTranslationDomain(packagePath);
    obj->setInitializationDelayed(true);
    obj->loadPackage(packagePath);
    obj->engine()->rootContext()->setContextProperty("commandlineArguments", parser.positionalArguments());

    QObject::connect(obj->engine(), &QQmlEngine::quit, &app, &QApplication::quit);

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    qmlRegisterType<ThemeListModel>();
    qmlRegisterType<ColorEditor>();
#else
    qmlRegisterAnonymousType<ThemeListModel>("org.kde.plasma.sdk", 1);
    qmlRegisterAnonymousType<ColorEditor>("org.kde.plasma.sdk", 1);
#endif
    ThemeModel *themeModel = new ThemeModel(obj->package());
    if (parser.isSet(themeOption)) {
        themeModel->setTheme(parser.value(themeOption));
        obj->engine()->rootContext()->setContextProperty("commandlineTheme", parser.value(themeOption));
    } else {
        themeModel->setTheme(parser.value("default"));
        obj->engine()->rootContext()->setContextProperty("commandlineTheme", "default");
    }
    obj->engine()->rootContext()->setContextProperty("themeModel", QVariant::fromValue(themeModel));

    obj->completeInitialization();

    if (!obj->package().metadata().isValid()) {
        return -1;
    }

    KPluginMetaData data = obj->package().metadata();
    // About data
    KAboutData aboutData(data.pluginId(), data.name(), data.version(), data.description(), KAboutLicense::byKeyword(data.license()).key());

    for (auto author : data.authors()) {
        aboutData.addAuthor(author.name(), author.task(), author.emailAddress(), author.webAddress(), author.ocsUsername());
    }

    // The root is not a window?
    // have to use a normal QQuickWindow since the root item is already created
    QWindow *window = qobject_cast<QWindow *>(obj->rootObject());
    if (window) {
        window->setTitle(obj->package().metadata().name());
        window->setIcon(QIcon::fromTheme(obj->package().metadata().iconName()));
    } else {
        qWarning() << "Error loading the ApplicationWindow";
    }

    return app.exec();
}
