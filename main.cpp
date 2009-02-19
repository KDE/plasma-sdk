/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <KApplication>
#include <KAboutData>
#include <KLocale>
#include <KCmdLineArgs>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData("PlasMate", 0, ki18n("PlasMate"),
                         "0.1", ki18n("A Plasma friendly IDE"), 
                         KAboutData::License_GPL,
                         ki18n("(c) 2009  Riccardo Iaconelli"), 
                         KLocalizedString(), "", "riccardo@kde.org");
    aboutData.addAuthor(ki18n("Riccardo Iaconelli"), ki18n("Author"), "riccardo@kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs::parsedArgs();
    KApplication app;
    
    MainWindow *w = new MainWindow();
    w->show();

    return app.exec();
}

