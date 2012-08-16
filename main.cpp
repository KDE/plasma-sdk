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

#include <QPointer>

#include <KDebug>
#include <KApplication>
#include <KAboutData>
#include <KLocale>
#include <KCmdLineArgs>

#include "mainwindow.h"

QPointer<MainWindow> mainwindow;

void customMessageHandler(QtMsgType type, const char *msg)
{
    if (mainwindow) {
        mainwindow->emitSendMessage(type, msg);
    } else {
        kDebug() << "*****************************************************";
        kDebug() << "Plasmate has fails to set a qInstallMsgHandler!!!!!!!";
        kDebug() << "*****************************************************";
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    KAboutData aboutData("plasmate", 0, ki18n("Plasmate"),
                         "0.1alpha3", ki18n("Plasma Add-Ons Creator"),
                         KAboutData::License_GPL,
                         ki18n("Copyright 2009-2011 Plasma Development Team"),
                         KLocalizedString(), "", "plasma-devel@kde.org");

    aboutData.addAuthor(ki18n("Sebastian Kügler"), ki18n("Author"), "sebas@kde.org");
    aboutData.addAuthor(ki18n("Shantanu Tushar Jha"), ki18n("Author"), "shaan7in@gmail.com");
    aboutData.addAuthor(ki18n("Diego Casella"), ki18n("Author"), "polentino911@gmail.com");
    aboutData.addAuthor(ki18n("Yuen Hoe Lim"), ki18n("Author"), "yuenhoe@hotmail.com");
    aboutData.addAuthor(ki18n("Richard Moore"), ki18n("Author"), "rich@kde.org");
    aboutData.addAuthor(ki18n("Artur Duque de Souza"), ki18n("Author"), "asouza@kde.org");
    aboutData.addAuthor(ki18n("Frerich Raabe"), ki18n("Author"), "raabe@kde.org");
    aboutData.addAuthor(ki18n("Aaron Seigo"), ki18n("Author"), "aseigo@kde.org");
    aboutData.addAuthor(ki18n("Riccardo Iaconelli"), ki18n("Author"), "riccardo@kde.org");

    aboutData.setProgramIconName("plasmagik");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs::parsedArgs();
    KApplication app;

    mainwindow = new MainWindow();
    qInstallMsgHandler(customMessageHandler);
    mainwindow->show();

    return app.exec();
}

