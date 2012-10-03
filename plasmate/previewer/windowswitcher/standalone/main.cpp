/*
Copyright 2012 Antonis Tsiapaliokas <kok3rs@gmail.com>

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

#include <KApplication>
#include <KAboutData>
#include <KLocale>
#include <KCmdLineArgs>
#include <QPointer>

#include "windowswitcherpreviewer.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData("WindowSwitcherPreviewer", 0, ki18n("Window Switcher Previewer"),
                         "0.1alpha3", ki18n("Previewer for Window Switcher"),
                         KAboutData::License_GPL,
                         ki18n("Copyright 2012 Plasma Development Team"),
                         KLocalizedString(), "", "plasma-devel@kde.org");

    aboutData.addAuthor(ki18n("Antonis Tsiapaliokas"), ki18n("Author"), "kok3rs@gmail.com");


    aboutData.setProgramIconName("window-duplicate");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs::parsedArgs();
    KApplication app;

    QPointer<WindowSwitcherPreviewer> preview = new WindowSwitcherPreviewer();
    preview->show();

    return app.exec();
}

