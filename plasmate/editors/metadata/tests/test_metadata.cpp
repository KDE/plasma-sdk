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

#include "../metadataeditor.h"

int main(int argc, char **argv)
{
//    KCmdLineArgs::init(argc, argv);
//    QApplication app;

    QApplication app(argc, argv);

    QWidget *top = new QWidget();
    QVBoxLayout *vbox = new QVBoxLayout(top);

    MetaDataEditor *ed = new MetaDataEditor(top);
    vbox->addWidget(ed);

    QPushButton *save = new QPushButton(top);
    save->setText("&Save");
    vbox->addWidget(save);

    QString filename;
    if (argc != 2) {
        filename = QDir::currentPath() + "/tests/plasma-applet-systemtray.desktop";
    } else {
        filename = QDir::currentPath() + "/" + argv[1];
    }

    ed->setFilename(filename);
    ed->readFile();

    QObject::connect(save, SIGNAL(clicked()), ed, SLOT(writeFile()));
    top->show();

    return app.exec();
}
