/***************************************************************************
 *                                                                         *
 *   Copyright 2014 Sebastian Kügler <sebas@kde.org>                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

// Qt
#include <QGuiApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>

// Frameworks
#include <KConfigGroup>
#include <KLocalizedString>
#include <Plasma/Theme>

// Own
#include "view.h"

static const char description[] = I18N_NOOP("Cuttlefish Icon Browser");
static const char version[] = PROJECT_VERSION;
static const char HOME_URL[] = "http://kde.org";

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    app.setApplicationVersion(version);

    const static auto _category = QStringLiteral("category");
    QCommandLineOption category = QCommandLineOption(QStringList() << QStringLiteral("c") << _category,
                               i18n("Start with category"), i18n("category"));

    const static auto _f = QStringLiteral("fullscreen");
    QCommandLineOption fullscreen = QCommandLineOption(QStringList() << QStringLiteral("f") << _f,
                               i18n("Start full-screen"));

    const static auto _p = QStringLiteral("picker");
    QCommandLineOption picker = QCommandLineOption(QStringList() << QStringLiteral("p") << _p,
                               i18n("Run in icon-picker mode"));

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.setApplicationDescription(description);
    parser.addHelpOption();
    parser.addOption(category);
    parser.addOption(fullscreen);
    parser.addOption(picker);

    parser.process(app);

    QString _cc = parser.value(category);

    auto settingsapp = new CuttleFish::View(_cc, parser);
    if (parser.isSet(fullscreen)) {
        settingsapp->setVisibility(QWindow::FullScreen);
    }

    return app.exec();
}
