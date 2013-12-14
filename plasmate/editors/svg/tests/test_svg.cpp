/*
  Copyright (c) 2009 Richard Moore, <rich@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <QDir>
#include <QVBoxLayout>
#include <QPushButton>

#include "../svgviewer.h"

int main(int argc, char **argv)
{
//    KCmdLineArgs::init(argc, argv);
//    QApplication app;

    QApplication app(argc, argv);

    QWidget *top = new QWidget();
    QVBoxLayout *vbox = new QVBoxLayout(top);

    SvgViewer *viewer = new SvgViewer(top);
    vbox->addWidget(viewer);

    QString filename;
    if (argc != 2) {
        filename = QDir::currentPath() + "/tests/battery-oxygen.svgz";
    } else {
        filename = QDir::currentPath() + "/" + argv[1];
    }

    viewer->setFilename(filename);
    viewer->readFile();
    top->resize(400, 300);
    top->show();

    return app.exec();
}
