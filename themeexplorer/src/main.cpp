/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QApplication>
#include "themelistmodel.h"
#include "thememodel.h"

#include <klocalizedstring.h>
#include <qcommandlineparser.h>
#include <qcommandlineoption.h>
#include <QQuickItem>
#include <QtQml>

#include <kpackage/package.h>
#include <kpackage/packageloader.h>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlExpression>
#include <QQmlProperty>
#include <QQuickWindow>
#include <kdeclarative/qmlobject.h>
#include <KAboutData>

int main(int argc, char **argv)
{
    QCommandLineParser parser;
    QApplication app(argc, argv);

    const QString description = i18n("Plasma Theme Explorer");
    const char version[] = "0.1";

    app.setApplicationVersion(version);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription(description);

    QCommandLineOption themeOption(QCommandLineOption(QStringList() << "t" << "theme", i18n("The theme to open"), "theme"));

    parser.addOption(themeOption);

    parser.process(app);

    const QString packagePath("org.kde.plasma.themeexplorer");

    //usually we have an ApplicationWindow here, so we do not need to create a window by ourselves
    KDeclarative::QmlObject *obj = new KDeclarative::QmlObject();
    obj->setTranslationDomain(packagePath);
    obj->setInitializationDelayed(true);
    obj->loadPackage(packagePath);
    obj->engine()->rootContext()->setContextProperty("commandlineArguments", parser.positionalArguments());

    qmlRegisterType<ThemeListModel>();
    ThemeModel *themeModel = new ThemeModel(obj->package());
    if (parser.isSet(themeOption)) {
        themeModel->setTheme(parser.value(themeOption));
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

    //The root is not a window?
    //have to use a normal QQuickWindow since the root item is already created
    QWindow *window = qobject_cast<QWindow *>(obj->rootObject());
    if (window) {
        window->setTitle(obj->package().metadata().name());
        window->setIcon(QIcon::fromTheme(obj->package().metadata().iconName()));
    } else {
        qWarning() << "Error loading the ApplicationWindow";
    }

    return app.exec();
}

