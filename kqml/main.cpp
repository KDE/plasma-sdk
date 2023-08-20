/*
    SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

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

#include <stdio.h>

void noFilesGiven()
{
    printf("kqml: No files specified. Terminating.\n");
    exit(1);
}

// Listens to the appEngine signals to determine if all files failed to load
class LoadWatcher : public QObject
{
    Q_OBJECT

public:
    explicit LoadWatcher(QQmlApplicationEngine *engine, int expected);

    int returnCode = 0;
    bool earlyExit = false;

public Q_SLOTS:
    void checkFinished(QObject *object, const QUrl &url);
    void quit();
    void exit(int retCode);

private:
    void contain(QQuickItem *item);

private:
    int expectedFileCount;
    int createdObjects;
};

LoadWatcher::LoadWatcher(QQmlApplicationEngine *engine, int expected)
    : QObject(engine)
    , expectedFileCount(expected)
    , createdObjects(0)
{
    connect(engine, &QQmlApplicationEngine::objectCreated, this, &LoadWatcher::checkFinished);
    // QQmlApplicationEngine also connects quit() to QCoreApplication::quit
    // and exit() to QCoreApplication::exit but if called before exec()
    // then QCoreApplication::quit or QCoreApplication::exit does nothing
    connect(engine, &QQmlEngine::quit, this, &LoadWatcher::quit);
    connect(engine, &QQmlEngine::exit, this, &LoadWatcher::exit);
}

void LoadWatcher::checkFinished(QObject *object, const QUrl &url)
{
    Q_UNUSED(url);

    expectedFileCount -= 1;

    if (object) {
        createdObjects += 1;
        if (auto item = qobject_cast<QQuickItem *>(object)) {
            contain(item);
        }
    }

    if (expectedFileCount == 0 && createdObjects == 0) {
        printf("kqml: Did not load any objects, exiting.\n");
        exit(2);
        QCoreApplication::exit(2);
    }
}

void LoadWatcher::quit()
{
    // Will be checked before calling exec()
    earlyExit = true;
    returnCode = 0;
}

void LoadWatcher::exit(int retCode)
{
    earlyExit = true;
    returnCode = retCode;
}

void LoadWatcher::contain(QQuickItem *item)
{
    if (!item) {
        return;
    }

    auto window = new QQuickWindow;
    auto contentItem = window->contentItem();

    window->setWidth(item->width());
    window->setHeight(item->height());
    item->setParentItem(contentItem);
    item->bindableWidth().setBinding(contentItem->bindableWidth().makeBinding());
    item->bindableHeight().setBinding(contentItem->bindableHeight().makeBinding());
    window->setVisible(true);
    static_cast<QObject *>(window)->setParent(this);
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

    QQmlApplicationEngine engine;

    QString applicationDomain;
    if (parser.isSet(applicationDomainOption)) {
        applicationDomain = parser.value(applicationDomainOption);
    } else {
        applicationDomain = QStringLiteral("kqml");
    }
    KLocalizedString::setApplicationDomain(qPrintable(applicationDomain));

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

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    // Load files
    QScopedPointer<LoadWatcher> lw(new LoadWatcher(&engine, files.size()));

    for (const QString &path : std::as_const(files)) {
        QUrl url = QUrl::fromUserInput(path, QDir::currentPath(), QUrl::AssumeLocalFile);
        engine.load(url);
    }

    if (lw->earlyExit) {
        return lw->returnCode;
    }

    return app.exec();
}

#include "main.moc"
