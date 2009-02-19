/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QDir>
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
#include <KActionCollection>
#include <KParts/Part>
#include <KStandardDirs>

#include <Plasma/PackageMetadata>

#include "editors/editpage.h"
#include "mainwindow.h"
#include "packagemodel.h"
#include "sidebar.h"
#include "startpage.h"
#include "ui_mainwindow.h"
#include "previewer/previewer.h"

MainWindow::MainWindow(QWidget *parent)
    : KParts::MainWindow(parent, 0),
      m_workflow(0),
      m_sidebar(0),
      m_previewer(0),
      m_factory(0),
      m_part(0),
      m_model(0),
      m_oldTab(0) // we start from startPage
{
    setXMLFile("plasmateui.rc");
    createMenus();

    m_startPage = new StartPage(this);
    connect(m_startPage, SIGNAL(projectSelected(QString,QString)), this, SLOT(loadProject(QString,QString)));
    setCentralWidget(m_startPage);
}

MainWindow::~MainWindow()
{
    delete m_startPage;
    delete m_factory;

    if (m_previewer) {
        delete m_previewer;
        delete m_previewerWidget;
    }
}

void MainWindow::createMenus()
{
    KStandardAction::quit(this, SLOT(quit()), actionCollection());
    menuBar()->addMenu(helpMenu());
    setupGUI();
}

void MainWindow::createDockWidgets()
{
    m_workflow = new QDockWidget(i18n("Workflow"), this);
    m_sidebar = new Sidebar(m_workflow);

    m_sidebar->addItem(KIcon("go-home"), i18n("Start page"));
    m_sidebar->addItem(KIcon("accessories-text-editor"), i18n("Edit"));
    m_sidebar->addItem(KIcon("krfb"), i18n("Publish"));
    m_sidebar->addItem(KIcon("help-contents"), i18n("Documentation"));

    m_workflow->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    connect(m_sidebar, SIGNAL(currentIndexChanged(int)),
            this, SLOT(changeTab(int)));

    m_workflow->setWidget(m_sidebar);
    addDockWidget(Qt::LeftDockWidgetArea, m_workflow);

    m_previewerWidget = new QDockWidget(i18n("Previewer"), this);
    m_previewer = new Previewer();
    m_previewerWidget->setWidget(m_previewer);
    addDockWidget(Qt::BottomDockWidgetArea, m_previewerWidget);
}

void MainWindow::quit()
{
    qApp->closeAllWindows();
//     deleteLater();
}

void MainWindow::showEditor()
{
    if (!m_factory) {
        m_factory = KLibLoader::self()->factory("katepart");

        m_part = 0;
        if (m_factory) {
            m_part = static_cast<KParts::ReadWritePart *>(m_factory->create(this, "KatePart"));
        }
    }

    if (m_part) {
        m_part = static_cast<KParts::ReadWritePart *>(m_factory->create(this, "KatePart"));
        setCentralWidget(m_part->widget());
        createGUI(m_part);
    }
}

void MainWindow::hideEditor()
{
    m_part->closeUrl();
    m_part = 0;
    createGUI(0);
}

void MainWindow::changeTab(int tab)
{
//     kDebug() << "Clicked m_sidebar item number" << tab;

    if (tab == m_oldTab) { // user clicked on the current tab 
        if (tab == StartPageTab) {
            m_startPage->resetStatus();
        }
        return;
    }

    centralWidget()->deleteLater();
    m_startPage = 0;

    switch (tab) {
        case StartPageTab: {
            m_startPage = new StartPage(this);
            setCentralWidget(m_startPage);
        }
        break;
        case EditTab: {
            m_editPage = new EditPage(this);
            m_editPage->setModel(m_model);
            setCentralWidget(m_editPage);
        }
        break;
        case PublishTab: {
            QLabel *l = new QLabel(i18n("Publish widget will go here!"), this);
            setCentralWidget(l);
        }
        break;
        case DocsTab: {
            QLabel *l = new QLabel(i18n("Documentation widget will go here!"), this);
            setCentralWidget(l);
        }
        break;
    }

   m_oldTab = tab;
}

void MainWindow::loadProject(const QString &name, const QString &type)
{
    kDebug() << "Loading project named" << name << "...";
    delete m_model;

    QString packagePath = KStandardDirs::locateLocal("appdata", name + '/');
    QString actualType = type;

    if (actualType.isEmpty()) {
        QDir dir(packagePath);
        if (dir.exists("metadata.desktop")) {
            Plasma::PackageMetadata metadata(packagePath + "metadata.desktop");
            actualType = metadata.serviceType();
        }
    }

    // Add it to the recent files first.
    m_model = new PackageModel(this);
    m_model->setPackageType(actualType);
    m_model->setPackage(packagePath);

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

    kDebug() << "Writing the following m_sidebar of recent files to the config:" << recentFiles;

    cg.writeEntry("recentFiles", recentFiles);
    c.sync();

    // Load the needed widgets, switch to page 1 (edit)...
    createDockWidgets();
    m_sidebar->setCurrentIndex(1);
}

QStringList MainWindow::recentProjects() // TODO Limit to 5?
{
    KConfig c;
    KConfigGroup cg = c.group("General");
    QStringList l = cg.readEntry("recentFiles", QStringList());
//     kDebug() << l.toStringList();

    return l;
}

