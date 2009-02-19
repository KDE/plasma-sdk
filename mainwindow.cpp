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
    
    m_startPage = new StartPage(this);
    connect(m_startPage, SIGNAL(projectSelected(QString)), this, SLOT(loadProject(QString)));
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
    
    sidebar = new KListWidget(workflow);
    sidebar->addItem(new QListWidgetItem(KIcon("go-home"), i18n("Start page")));
    sidebar->addItem(new QListWidgetItem(KIcon("accessories-text-editor"), i18n("Edit")));
    sidebar->addItem(new QListWidgetItem(KIcon("krfb"), i18n("Publish")));
    sidebar->addItem(new QListWidgetItem(KIcon("help-contents"), i18n("Documentation")));
    sidebar->addItem(new QListWidgetItem(KIcon("system-run"), i18n("Preview")));
    sidebar->setIconSize(QSize(48, 48));
    sidebar->setViewMode(QListView::IconMode);
    sidebar->setFlow(QListView::TopToBottom);
    sidebar->setMovement(QListView::Static);
    sidebar->setResizeMode(QListView::Adjust);
    
    connect(sidebar, SIGNAL(currentRowChanged(int)),
            this, SLOT(changeTab(int)));
    
//     SidebarDelegate *delegate = new SidebarDelegate(sidebar);
//     sidebar->setItemDelegate(delegate);
    
    workflow->setWidget(sidebar);
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

void MainWindow::loadProject(const QString &name)
{
    kDebug() << "Loading project named" << name << "...";

    // Add it to the recent files first.
    
    QStringList recentFiles;
    KConfig c;
    KConfigGroup cg = c.group("General");
    recentFiles = recentProjects();
    
    if (recentFiles.contains(name)) {
        recentFiles.removeAt(recentFiles.indexOf(name));
    }
    
    if (!name.isEmpty()) {
        recentFiles.prepend(name);
    }
    
    kDebug() << "Writing the following sidebar of recent files to the config:" << recentFiles;
    
    cg.writeEntry("recentFiles", recentFiles);
    
    c.sync();
    
    // Load the needed widgets, switch to page 1 (edit)...
    createDockWidgets();
    sidebar->setCurrentRow(1);
}

QStringList MainWindow::recentProjects() // TODO Limit to 5 
{
    KConfig c;
    KConfigGroup cg = c.group("General");
    QStringList l = cg.readEntry("recentFiles", QStringList());
//     kDebug() << l.toStringList();
    
    return l;
}

