/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QDockWidget>
#include <QListWidget>

#include <KAction>
#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <KMenu>
#include <KMenuBar>
#include <KStandardAction>
#include <KUrl>

#include "startpage.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : KMainWindow(parent)
{   
    createMenus();
    createDockWidgets();
    StartPage *startPage = new StartPage(this);
    connect(startPage, SIGNAL(projectSelected(KUrl)), this, SLOT(loadProject(KUrl)));
    setCentralWidget(startPage);
}

MainWindow::~MainWindow()
{

}

void MainWindow::createMenus()
{
    //FIXME: should be using XMLGUI for this
    KMenu *file = new KMenu("File", this);

    file->addAction(KStandardAction::quit(this, SLOT(quit()), file));

    menuBar()->addMenu(file);
    menuBar()->addMenu(helpMenu());
}

void MainWindow::createDockWidgets()
{
    QDockWidget *workflow = new QDockWidget(i18n("Workflow"), this);
    
    QListWidget *list = new QListWidget(workflow);
    list->addItem("test");
    
    workflow->setWidget(list);
    addDockWidget(Qt::LeftDockWidgetArea, workflow);
}

void MainWindow::quit()
{
    deleteLater();
}

void MainWindow::loadProject(const KUrl &url)
{
    kDebug() << "Loading project at" << url << "...";

    // Add it to the recent files first.
    
    KUrl::List recentFiles;
    KConfig c;
    KConfigGroup cg = c.group("General");
    recentFiles = recentProjects();
    
    if (recentFiles.contains(url)) {
        recentFiles.removeAt(recentFiles.indexOf(url));
    }
    
    if (url.isValid()) {
        recentFiles.prepend(url);
    }
    
    kDebug() << "Writing the following list of recent files to the config:" << recentFiles.toStringList();
    
    cg.writeEntry("recentFiles", recentFiles.toStringList());
    
    c.sync();
    
    // Load the needed widgets...
}

KUrl::List MainWindow::recentProjects() // TODO Limit to 5 
{
    KConfig c;
    KConfigGroup cg = c.group("General");
    KUrl::List l = cg.readEntry("recentFiles", QStringList());
    kDebug() << l.toStringList();
    
    return l;
}

