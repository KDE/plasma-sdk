/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <KMenu>
#include <KMenuBar>
#include <KConfig>
#include <KConfigGroup>

#include "startpage.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : KMainWindow(parent)
{   
    createMenus();
    setCentralWidget(new StartPage(this));
}

MainWindow::~MainWindow()
{

}

void MainWindow::createMenus()
{
    KMenu *file = new KMenu("File", this);
    
    QAction *quit;
    quit = file->addAction(KIcon("application-exit"), "Quit", this, SLOT(quit()));
    
    menuBar()->addMenu(file);
    menuBar()->addMenu(helpMenu());
}

void MainWindow::quit()
{
    deleteLater();
}

void MainWindow::loadProject(const KUrl &url)
{
    // Add it to the recent files first.
    
    KConfig c;
    KConfigGroup cg = c.group("General");
    QList<KUrl> recentFiles = recentFiles();
    
    if (recentFiles.contains(url));
        recentFiles.removeAt(recentFiles.indexOf(url));
    }
    
    recentFiles.prepend(url);
    cg.writeEntry("recentFiles", recentFiles);
    c.sync();
    
    // Load the needed widgets...
}

QList<KUrl> MainWindow::recentFiles() // TODO Limit to 5 
{
    KConfig c;
    KConfigGroup cg = c.group("General");
    QList<KUrl> l = cg.readEntry("recentFiles", QList<KUrl>());
    
    return l;
}

