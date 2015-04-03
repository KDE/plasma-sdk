/*
Copyright 2009 Riccardo Iaconelli <riccardo@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#include <QIcon>

#include <qcommandlineparser.h>
#include <qcommandlineoption.h>

#include <KAboutData>
#include <KLocalizedString>

#include "plasmateapp.h"

static const char version[] = "5.2.90";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationVersion(version);

    KAboutData aboutData(QStringLiteral("plasmate"), i18n("Plasmate"), version,
                         i18n("Plasma Add-Ons Creator"), KAboutLicense::GPL,
                         i18n("Copyright 2009-2014 Plasma Development Team"));

    aboutData.setOrganizationDomain("kde.org");

    aboutData.addAuthor(i18n("Sebastian Kügler"), i18n("Author"), "sebas@kde.org");
    aboutData.addAuthor(i18n("Shantanu Tushar Jha"), i18n("Author"), "shaan7in@gmail.com");
    aboutData.addAuthor(i18n("Diego Casella"), i18n("Author"), "polentino911@gmail.com");
    aboutData.addAuthor(i18n("Yuen Hoe Lim"), i18n("Author"), "yuenhoe@hotmail.com");
    aboutData.addAuthor(i18n("Richard Moore"), i18n("Author"), "rich@kde.org");
    aboutData.addAuthor(i18n("Artur Duque de Souza"), i18n("Author"), "asouza@kde.org");
    aboutData.addAuthor(i18n("Frerich Raabe"), i18n("Author"), "raabe@kde.org");
    aboutData.addAuthor(i18n("Aaron Seigo"), i18n("Author"), "aseigo@kde.org");
    aboutData.addAuthor(i18n("Riccardo Iaconelli"), i18n("Author"), "riccardo@kde.org");
    aboutData.addAuthor(i18n("Giorgos Tsiapaliokas"), i18n("Author"), "terietor@gmail.com");
    aboutData.addAuthor(i18n("Antonis Tsiapaliokas"), i18n("Author"), "kok3rs@gmail.com");

    app.setWindowIcon(QIcon::fromTheme("plasmagik"));

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    PlasmateApp *plasmateApp = new PlasmateApp();
    return app.exec();
}

