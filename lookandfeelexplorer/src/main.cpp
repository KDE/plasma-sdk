/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "lnflistmodel.h"
#include "lnflogic.h"
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
    QCommandLineParser parser;
    QApplication app(argc, argv);

    app.setApplicationVersion(PROJECT_VERSION);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription(i18n("Plasma Look And Feel Explorer"));

    QCommandLineOption themeOption(QCommandLineOption(QStringList() << "l"
                                                                    << "lookandfeel",
                                                      i18n("Look And Feel to open"),
                                                      "lookandfeel"));

    parser.addOption(themeOption);

    parser.process(app);

    const QString packagePath("org.kde.plasma.lookandfeelexplorer");

    // usually we have an ApplicationWindow here, so we do not need to create a window by ourselves
    KDeclarative::QmlObject obj;
    obj.setTranslationDomain(packagePath);
    obj.setInitializationDelayed(true);
    obj.engine()->rootContext()->setContextProperty("commandlineTheme", parser.value(themeOption));
    obj.engine()->rootContext()->setContextProperty("commandlineArguments", parser.positionalArguments());
    obj.loadPackage(packagePath);

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    qmlRegisterType<LnfLogic>();
    qmlRegisterType<LnfListModel>();
#else
    qmlRegisterAnonymousType<LnfLogic>("org.kde.plasma.sdk", 1);
    qmlRegisterAnonymousType<LnfListModel>("org.kde.plasma.sdk", 1);
#endif

    LnfLogic *lnfLogic = new LnfLogic(&obj);
    obj.engine()->rootContext()->setContextProperty("lnfLogic", QVariant::fromValue(lnfLogic));

    obj.completeInitialization();

    if (!obj.package().metadata().isValid()) {
        return -1;
    }

    KPluginMetaData data = obj.package().metadata();
    // About data
    KAboutData aboutData(data.pluginId(), data.name(), data.version(), data.description(), KAboutLicense::byKeyword(data.license()).key());

    for (auto author : data.authors()) {
        aboutData.addAuthor(author.name(), author.task(), author.emailAddress(), author.webAddress(), author.ocsUsername());
    }

    // The root is not a window?
    // have to use a normal QQuickWindow since the root item is already created
    QWindow *window = qobject_cast<QWindow *>(obj.rootObject());
    if (window) {
        window->setTitle(obj.package().metadata().name());
        window->setIcon(QIcon::fromTheme(obj.package().metadata().iconName()));
    } else {
        qWarning() << "Error loading the ApplicationWindow";
    }

    return app.exec();
}
