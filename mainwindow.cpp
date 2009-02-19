/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <KMenu>
#include <KMenuBar>

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

