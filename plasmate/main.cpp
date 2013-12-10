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

#include <QDebug>
#include <QApplication>
#include <KLocale>

#include <qcommandlineparser.h>
#include <qcommandlineoption.h>

#include "mainwindow.h"
#include "konsole/konsolepreviewer.h"

static const char version[] = "2.0";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCommandLineParser parser;

    app.setApplicationVersion(version);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    qInstallMsgHandler(KonsolePreviewer::customMessageHandler);
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();
    return app.exec();
}

