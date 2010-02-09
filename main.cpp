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
    KAboutData aboutData("plasmate", 0, ki18n("Plasmate"),
                         "0.1alpha1", ki18n("A Plasma Add-ons Creator"),
                         KAboutData::License_GPL,
                         ki18n("(c) 2009-2010 Plasma Development Team"),
                         KLocalizedString(), "", "plasma-devel@kde.org");

    aboutData.addAuthor(ki18n("Shantanu Tushar Jha"), ki18n("Author"), "jhahoneyk@gmail.com");
    aboutData.addAuthor(ki18n("Diego Casella"), ki18n("Author"), "polentino911@gmail.com");
    aboutData.addAuthor(ki18n("Yuen Hoe Lim"), ki18n("Author"), "yuenhoe86@gmail.com");
    aboutData.addAuthor(ki18n("Richard Moore"), ki18n("Author"), "richmoore44@gmail.com");
    aboutData.addAuthor(ki18n("Artur Duque de Souza"), ki18n("Author"), "morpheuz@gmail.com");
    aboutData.addAuthor(ki18n("Frerich Raabe"), ki18n("Author"), "raabe@kde.org");
    aboutData.addAuthor(ki18n("Aaron Seigo"), ki18n("Author"), "aseigo@kde.org");
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

