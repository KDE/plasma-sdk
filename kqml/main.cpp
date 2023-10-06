/*
    SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KF6/PlasmaQuick/plasmaquick/sharedqmlengine.h"
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QScopedPointer>
#include <QStringList>

#include <KLocalizedContext>
#include <KLocalizedString>

#include <PlasmaQuick/QuickViewSharedEngine>

#include <stdio.h>

void noFilesGiven()
{
    printf("kqml: No files specified. Terminating.\n");
    exit(1);
}

void contain(QQuickWindow *window, QQuickItem *item)
{
    if (!item) {
        return;
    }

    auto contentItem = window->contentItem();

    window->setWidth(item->width());
    window->setHeight(item->height());
    item->setParentItem(contentItem);
    item->bindableWidth().setBinding(contentItem->bindableWidth().makeBinding());
    item->bindableHeight().setBinding(contentItem->bindableHeight().makeBinding());
    window->setVisible(true);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("KDE QML Runtime");
    app.setOrganizationName("KDE");
    app.setOrganizationDomain("kde.org");
    QApplication::setApplicationVersion(QLatin1String(PROJECT_VERSION));

    QStringList files;

    // Handle main arguments
    QCommandLineParser parser;
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption applicationDomainOption(QStringList{QStringLiteral("d"), QStringLiteral("domain")},
                                               QStringLiteral("The main localization domain"),
                                               QStringLiteral("domain"));
    parser.addOption(applicationDomainOption);

    // Positional arguments
    parser.addPositionalArgument("files", QStringLiteral("Any number of QML files can be loaded. They will share the same engine."), "[files...]");
    parser.addPositionalArgument("args",
                                 QStringLiteral("Arguments after '--' are ignored, but passed through to the application.arguments variable in QML."),
                                 "[-- args...]");

    parser.process(app);

    QString applicationDomain;
    if (parser.isSet(applicationDomainOption)) {
        applicationDomain = parser.value(applicationDomainOption);
    } else {
        applicationDomain = QStringLiteral("kqml");
    }

    for (QString posArg : parser.positionalArguments()) {
        if (posArg == QLatin1String("--")) {
            break;
        } else {
            files.append(posArg);
        }
    }

    if (files.size() <= 0) {
        noFilesGiven();
    }

    // Load files
    for (const QString &path : std::as_const(files)) {
        QUrl url = QUrl::fromUserInput(path, QDir::currentPath(), QUrl::AssumeLocalFile);
        auto window = new PlasmaQuick::QuickViewSharedEngine();
        window->setTitle(url.fileName());
        window->setSource(url);
        window->show();
    }

    return app.exec();
}

#include "main.moc"
