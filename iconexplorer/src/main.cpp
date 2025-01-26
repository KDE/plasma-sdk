/*
    SPDX-FileCopyrightText: 2014 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Qt
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QStandardPaths>

// Frameworks
#include <KAboutData>
#include <KConfigGroup>
#include <KCrash>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

// Plasma
#include <Plasma/Theme>

// Own
#include "colorschemes.h"
#include "iconmodel.h"
#include "sortfiltermodel.h"

using namespace Qt::Literals;

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if ((msg.contains("qt.svg") && msg.contains("Could not resolve property: #linearGradient")) || msg.contains("Could not resolve property: #pattern")) {
        return;
    }

    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "\u001b[33mWarning\u001b[0m: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "\u001b[33;1mCritical\u001b[0m: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "\u001b[31;1mFatal\u001b[0m: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }
}

int main(int argc, char **argv)
{
    qInstallMessageHandler(messageOutput);

    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("iconexplorer");

    KAboutData about(u"iconexplorer"_s, i18n("Icon Explorer"), PROJECT_VERSION);
    KAboutData::setApplicationData(about);

    KCrash::initialize();

    const static auto _category = QStringLiteral("category");
    QCommandLineOption category = QCommandLineOption(QStringList() << QStringLiteral("c") << _category, i18n("Start with category"), i18n("category"));

    const static auto _f = QStringLiteral("fullscreen");
    QCommandLineOption fullscreen = QCommandLineOption(QStringList() << QStringLiteral("f") << _f, i18n("Start full-screen"));

    const static auto _p = QStringLiteral("picker");
    QCommandLineOption picker = QCommandLineOption(QStringList() << QStringLiteral("p") << _p, i18n("Run in icon-picker mode"));

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.setApplicationDescription("Icon Browser");
    parser.addHelpOption();
    parser.addOption(category);
    parser.addOption(fullscreen);
    parser.addOption(picker);

    parser.process(app);

    QString _cc = parser.value(category);

    // Clean up old config files
    const QString configFile = QStandardPaths::locate(QStandardPaths::ConfigLocation, "KDE/Icon Explorer.conf");
    if (!configFile.isEmpty()) {
        QFile::remove(configFile);
    }

    const QString oldConfigFile = QStandardPaths::locate(QStandardPaths::ConfigLocation, "KDE/Cuttlefish.conf");
    if (!oldConfigFile.isEmpty()) {
        QFile::remove(oldConfigFile);
    }

    QQmlApplicationEngine engine;

    auto l10nContext = new KLocalizedQmlContext(&engine);
    l10nContext->setTranslationDomain(QStringLiteral("iconexplorer"));
    engine.rootContext()->setContextObject(l10nContext);

    auto iconModel = new CuttleFish::IconModel(engine.rootContext());
    auto proxyModel = new CuttleFish::SortFilterModel(engine.rootContext());
    proxyModel->setSourceModel(iconModel);
    proxyModel->sort(0);
    auto colorSchemes = new CuttleFish::ColorSchemes(engine.rootContext());

    engine.rootContext()->setContextProperty("iconModel", iconModel);
    engine.rootContext()->setContextProperty("proxyModel", proxyModel);
    engine.rootContext()->setContextProperty("pickerMode", parser.isSet("picker"));
    engine.rootContext()->setContextProperty("colorSchemes", colorSchemes);

    engine.loadFromModule("org.kde.iconexplorer", "Iconexplorer");
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
