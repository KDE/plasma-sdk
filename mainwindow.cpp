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
#include <QGridLayout>

#include <KTextEdit>

#include <KAction>
#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <KMenu>
#include <KMenuBar>
#include <KStandardAction>
#include <KUrl>
#include <KListWidget>
#include <KParts/Part>

#include "startpage.h"
#include "sidebar.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : KParts::MainWindow(parent, 0)
{    
   createMenus();
    
    m_factory = 0;
    m_part = 0;
    
    m_startPage = new StartPage(this);
    connect(m_startPage, SIGNAL(projectSelected(QString)), this, SLOT(loadProject(QString)));
    setCentralWidget(m_startPage);
    int oldTab = 0; // always startPage

    setXMLFile("plasmateui.rc");
//     createShellGUI();
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
    workflow = new QDockWidget(i18n("Workflow"), this);
    
    sidebar = new Sidebar(workflow);
    
//     sidebar = new KListWidget(workflow);
    sidebar->addItem(KIcon("go-home"), i18n("Start page"));
    sidebar->addItem(KIcon("accessories-text-editor"), i18n("Edit"));
    sidebar->addItem(KIcon("krfb"), i18n("Publish"));
    sidebar->addItem(KIcon("help-contents"), i18n("Documentation"));
    sidebar->addItem(KIcon("system-run"), i18n("Preview"));
//     sidebar->addItem(new QListWidgetItem(KIcon("go-home"), i18n("Start page")));
//     sidebar->addItem(new QListWidgetItem(KIcon("accessories-text-editor"), i18n("Edit")));
//     sidebar->addItem(new QListWidgetItem(KIcon("krfb"), i18n("Publish")));
//     sidebar->addItem(new QListWidgetItem(KIcon("help-contents"), i18n("Documentation")));
//     sidebar->addItem(new QListWidgetItem(KIcon("system-run"), i18n("Preview")));
//     sidebar->setIconSize(QSize(48, 48));
//     sidebar->setViewMode(QListView::IconMode);
//     sidebar->setFlow(QListView::TopToBottom);
//     sidebar->setMovement(QListView::Static);
//     sidebar->setResizeMode(QListView::Adjust);
    
    workflow->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        
    connect(sidebar, SIGNAL(currentIndexChanged(int)),
            this, SLOT(changeTab(int)));
    
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
    
//     centralWidget()->deleteLater(); // clean
//     if (oldTab == 1) {
//         m_part->closeUrl();
//     }
    kDebug() << "here we are";
    
//     if (centralWidget() && m_part && centralWidget() != m_part->widget()) {
        centralWidget()->deleteLater();
//     }
    
    kDebug() << "deletalater";
    
    if (tab == 0) {
        m_startPage = new StartPage(this);
        setCentralWidget(m_startPage);
    } else if (tab == 1) {
        kDebug() << "tab 1";
//         centralWidget()->deleteLater();
        kDebug() << m_factory;
        kDebug() << "and m_part";
//         kDebug() << m_part;

        if (!m_factory) {
            m_factory = KLibLoader::self()->factory("katepart");
        }
        m_part = 0;
        if (m_factory && !m_part) {
            kDebug() << "babla";
                // now that the Part is loaded, we cast it to a Part to get
                // our hands on it
            m_part = static_cast<KParts::ReadWritePart *>(m_factory->create(this, "KatePart"));
            if (m_part) {
                kDebug() << "if mpart";
                    // tell the KParts::MainWindow that this is indeed
                    // the main widget
                setCentralWidget(m_part->widget());
//                     m_part->widget()->show();
                kDebug() << "set";
//                 setupGUI(ToolBar | Keys | StatusBar | Save);
//                     setupGUI();
                kDebug() << "setup or create?!;";
                    // and integrate the part's GUI with the shell's
                //createGUI(m_part);
                kDebug() << "created";
            } 
        }
        
            if (m_part) {
                    // tell the KParts::MainWindow that this is indeed
                    // the main widget
                setCentralWidget(m_part->widget());
                kDebug() << "setCentralWidget i said";
//                     m_part->widget()->show();
        
//                     setupGUI();
        
                    // and integrate the part's GUI with the shell's
                createGUI(m_part);
            } 
//             } 
//         }
//         KTextEdit *l = new KTextEdit(this);
//         l->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
//         setCentralWidget(l);
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
//     removeDockWidget(workflow);
//     addDockWidget(Qt::LeftDockWidgetArea, workflow);
    workflow->hide();
    workflow->show();
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
    sidebar->setCurrentIndex(1);
}

QStringList MainWindow::recentProjects() // TODO Limit to 5 
{
    KConfig c;
    KConfigGroup cg = c.group("General");
    QStringList l = cg.readEntry("recentFiles", QStringList());
//     kDebug() << l.toStringList();
    
    return l;
}

