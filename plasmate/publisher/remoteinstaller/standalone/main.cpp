/*
 * Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <KAboutData>
#include <KLocale>
#include <KCmdLineArgs>

#include <QPointer>

#include "plasmaremoteinstaller.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData("plasmaremoteinstaller", 0, ki18n("Plasma Remote installer"),
                         "0.1alpha3", ki18n("Plasma Remote Installer"),
                         KAboutData::License_GPL,
                         ki18n("Copyright 2012 Plasma Development Team"),
                         KLocalizedString(), "", "plasma-devel@kde.org");

    aboutData.addAuthor(ki18n("Giorgos Tsiapaliwkas"), ki18n("Author"), "terietor@gmail.com");
    aboutData.setProgramIconName("plasmagik");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    QApplication app;

    QPointer<PlasmaRemoteInstaller> dialog = new PlasmaRemoteInstaller();
    dialog->exec();
    delete dialog;

    return app.exec();
}

