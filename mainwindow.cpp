/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QDockWidget>
#include <QListWidgetItem>
#include <QModelIndex>
#include <QLabel>

#include <KAction>
#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <KMenu>
#include <KMenuBar>
#include <KStandardAction>
#include <KUrl>
#include <KListWidget>

#include "startpage.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : KMainWindow(parent)
{   
    createMenus();
    createDockWidgets();
    
    m_startPage = new StartPage(this);
    connect(m_startPage, SIGNAL(projectSelected(KUrl)), this, SLOT(loadProject(KUrl)));
    setCentralWidget(m_startPage);
    int oldTab = 0; // always startPage
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
    
    KListWidget *list = new KListWidget(workflow);
    list->addItem(new QListWidgetItem(KIcon("go-home"), i18n("Start page")));
    list->addItem(new QListWidgetItem(KIcon("accessories-text-editor"), i18n("Edit")));
    list->addItem(new QListWidgetItem(KIcon("krfb"), i18n("Publish")));
    list->addItem(new QListWidgetItem(KIcon("help-contents"), i18n("Documentation")));
    list->addItem(new QListWidgetItem(KIcon("system-run"), i18n("Preview")));
    list->setIconSize(QSize(48, 48));
    list->setViewMode(QListView::IconMode);
    list->setFlow(QListView::TopToBottom);
    list->setMovement(QListView::Static);
    list->setResizeMode(QListView::Adjust);
    
    connect(list, SIGNAL(currentRowChanged(int)),
            this, SLOT(changeTab(int)));
    
//     SidebarDelegate *delegate = new SidebarDelegate(list);
//     list->setItemDelegate(delegate);
    
    workflow->setWidget(list);
    addDockWidget(Qt::LeftDockWidgetArea, workflow);
}

void MainWindow::quit()
{
    deleteLater();
}

void MainWindow::changeTab(int tab)
{
//     kDebug() << "Clicked sidebar item number" << tab;
    
    if (tab == oldTab) { // user clicked on the current tab 
        if (tab == 0) {
            m_startPage->resetStatus();
        }
        
        return;
    }
    
    centralWidget()->deleteLater(); // clean
    
    if (tab == 0) {
        m_startPage = new StartPage(this);
        setCentralWidget(m_startPage);
    } else if (tab == 1) {
        QLabel *l = new QLabel(i18n("Edit widget will go here!"));
        setCentralWidget(l);
    } else if (tab == 2) {
        QLabel *l = new QLabel(i18n("Publish widget will go here!"));
        setCentralWidget(l);
    } else if (tab == 3) {
        QLabel *l = new QLabel(i18n("Documentation widget will go here!"));
        setCentralWidget(l);
    } else if (tab == 4) {
        QLabel *l = new QLabel(i18n("Preview widget will go here!"));
        setCentralWidget(l);
    }
    
    oldTab = tab;
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
//     kDebug() << l.toStringList();
    
    return l;
}

