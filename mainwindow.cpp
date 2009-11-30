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
#include <KTextEditor/ConfigInterface>
#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KStandardAction>
#include <KUrl>
#include <KListWidget>
#include <KActionCollection>
#include <KParts/Part>
#include <KStandardDirs>

#include <Plasma/PackageMetadata>

#include "editors/editpage.h"
#include "editors/metadata/metadataeditor.h"
#include "savesystem/timeline.h"
#include "mainwindow.h"
#include "packagemodel.h"
#include "sidebar.h"
#include "startpage.h"
#include "ui_mainwindow.h"
#include "previewer/previewer.h"
#include "publisher/publisher.h"
#include "docbrowser/docbrowser.h"


MainWindow::CentralContainer::CentralContainer(QWidget* parent)
    : QWidget(parent),
      m_curWidget(0)
{
    m_layout = new QVBoxLayout();
    setLayout(m_layout);
}

void MainWindow::CentralContainer::switchTo(QWidget* newWidget)
{
    if (m_curWidget) {
        m_curWidget->hide();
        m_layout->removeWidget(m_curWidget);
    }
    m_curWidget = newWidget;
    m_layout->addWidget(m_curWidget);
    m_curWidget->show();
}

MainWindow::MainWindow(QWidget *parent)
    : KParts::MainWindow(parent, 0),
      m_workflow(0),
      m_sidebar(0),
      m_dockTimeLine(0),
      m_timeLine(0),
      m_previewer(0),
      m_publisher(0),
      m_browser(0),
      m_metaEditor(0),
      m_editWidget(0),
      m_editPage(0),
      m_model(0),
      m_oldTab(0), // we start from startPage
      docksCreated(false),
      m_part(0)
{
    setXMLFile("plasmateui.rc");
    createMenus();
    m_startPage = new StartPage(this);
    connect(m_startPage, SIGNAL(projectSelected(QString, QString)),
            this, SLOT(loadProject(QString, QString)));
    m_central = new CentralContainer(this);
    setCentralWidget(m_central);
    m_central->switchTo(m_startPage);
    setDockOptions(QMainWindow::AllowNestedDocks); // why not?
}

MainWindow::~MainWindow()
{
    // Saving layout position
    KConfig c;
    KConfigGroup configDock = c.group("DocksPosition");
    configDock.writeEntry("MainWindowLayout", saveState(0));
    c.sync();

    // if the user closes the application with an editor open, should
    // save its contents
    saveEditorData();
    delete m_part;
    m_part = 0;
    delete m_metaEditor;
    m_metaEditor = 0;

    if (m_workflow) {
        //delete m_startPage;
        delete m_workflow;
    }

    if (m_previewer) {
        configDock.writeEntry("PreviewerHeight", m_previewerWidget->height());
        configDock.writeEntry("PreviewerWidth", m_previewerWidget->width());
        c.sync();
        delete m_previewer;
        delete m_previewerWidget;
    }
    if (m_dockTimeLine) {
        delete m_timeLine;
        delete m_dockTimeLine;
    }

    if (m_editPage) {
        delete m_editPage;
        delete m_editWidget;
    }
}

void MainWindow::createMenus()
{
    KStandardAction::quit(this, SLOT(quit()), actionCollection());
    KAction *refresh = KStandardAction::redisplay(this, SLOT(saveAndRefresh()), actionCollection());
    refresh->setShortcut(tr("Ctrl+F5"));
    refresh->setText("Refresh Previewer");
    menuBar()->addMenu(helpMenu());
    setupGUI();
}

void MainWindow::createDockWidgets()
{
    KConfig c;
    KConfigGroup configDock = c.group("DocksPosition");
    /////////////////////////////////////////////////////////////////////////
    m_workflow = new QDockWidget(i18n("Workflow"), this);
    m_workflow->setObjectName("workflow");
    m_sidebar = new Sidebar(m_workflow);

    m_sidebar->addItem(KIcon("go-home"), i18n("Start page"));
    m_sidebar->addItem(KIcon("accessories-text-editor"), i18n("Edit"));
    m_sidebar->addItem(KIcon("document-save"),i18n("New SavePoint"));
    m_sidebar->addItem(KIcon("krfb"), i18n("Publish"));
    m_sidebar->addItem(KIcon("help-contents"), i18n("Documentation"));
    m_sidebar->addItem(KIcon("user-desktop"), i18n("Preview"));

    connect(m_sidebar, SIGNAL(currentIndexClicked(const QModelIndex &)),
            this, SLOT(changeTab(const QModelIndex &)));

    m_workflow->setWidget(m_sidebar);
    addDockWidget(Qt::LeftDockWidgetArea, m_workflow);

    m_workflow->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_sidebar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    /////////////////////////////////////////////////////////////////////////
    m_editPage = new EditPage();
    m_editPage->setModel(m_model);

    m_editWidget = new QDockWidget(i18n("Files"), this);
    m_editWidget->setObjectName("edit tree");
    m_editWidget->setWidget(m_editPage);
    addDockWidget(Qt::RightDockWidgetArea, m_editWidget);

    connect(m_editPage, SIGNAL(loadEditor(KService::List, KUrl)), this, SLOT(loadRequiredEditor(const KService::List, KUrl)));
    connect(m_editPage, SIGNAL(loadMetaDataEditor(KUrl)), this, SLOT(loadMetaDataEditor(KUrl)));

    m_editPage->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    m_editWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    /////////////////////////////////////////////////////////////////////////
    m_dockTimeLine = new QDockWidget(i18n("TimeLine"), this);
    m_dockTimeLine->setObjectName("timeline");
    m_timeLine = new TimeLine(m_dockTimeLine, m_model->package());

    m_dockTimeLine->setWidget(m_timeLine);

    addDockWidget(Qt::RightDockWidgetArea, m_dockTimeLine);

    m_dockTimeLine->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_timeLine->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    /////////////////////////////////////////////////////////////////////////
    m_previewerWidget = new QDockWidget(i18n("Previewer"), this);
    m_previewerWidget->setObjectName("workflow");
    m_previewer = new Previewer(this);
    m_previewerWidget->setWidget(m_previewer);
    addDockWidget(Qt::LeftDockWidgetArea, m_previewerWidget);

    m_previewerWidget->updateGeometry();
    m_previewer->updateGeometry();

    m_previewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_previewerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    splitDockWidget(m_workflow, m_editWidget, Qt::Horizontal);
    splitDockWidget(m_editWidget, m_previewerWidget, Qt::Vertical);

    // Restoring the previous layout
    restoreState(configDock.readEntry("MainWindowLayout",QByteArray()), 0);

    connect(this, SIGNAL(newSavePointClicked()),
            m_timeLine, SLOT(newSavePoint()));

    docksCreated = true;

    int w = size().width() < sizeHint().width() ? sizeHint().width() : size().width();
    int h = size().height() < sizeHint().height() ? sizeHint().height() : size().height();
    resize(w, h);
}

void MainWindow::quit()
{
    qApp->closeAllWindows();
//     deleteLater();
}

void MainWindow::changeTab(const QModelIndex &item)
{
    // should save data in any open editors when changing tabs
    saveEditorData();

    int tab = item.row();

    m_startPage->resetStatus();

    if(tab == SavePoint) {
        emit newSavePointClicked();
        m_sidebar->setCurrentIndex(m_oldTab);
        tab = m_oldTab;
    }

    switch (tab) {
    case StartPageTab: {
        m_central->switchTo(m_startPage);
    }
    break;
    case EditTab: {
        // see if there is a previously active editor to restore
        // TODO: immediately shift focus to the editor (instead of the tabbar)
        if (m_metaEditor) {
            m_central->switchTo(m_metaEditor);
        } else if (m_part) {
            m_central->switchTo(m_part->widget());
        } else {
            //FIXME: LEAK!
            QLabel *l = new QLabel(i18n("Select a file to edit."), this);
            m_central->switchTo(l);
        }
    }
    break;
    case PublishTab: {
        if (!m_publisher)
            m_publisher = new Publisher(this, m_model->package());
        m_central->switchTo(m_publisher);
    }
    break;
    case DocsTab: {
        if (!m_browser)
            m_browser = new DocBrowser(this);
        m_central->switchTo(m_browser);
    }
    break;
    case PreviewTab: {
        // FIXME: LEAK!
        Previewer *tabPreviewer = new Previewer(this);
        tabPreviewer->addApplet(m_model->package());
        m_central->switchTo(tabPreviewer);
    }
    }

    m_oldTab = tab;
}

void MainWindow::saveEditorData() {
    if (qobject_cast<KParts::ReadWritePart*>(m_part)) {
        static_cast<KParts::ReadWritePart*>(m_part)->save();
    }
    if (m_metaEditor) {
        m_metaEditor->writeFile();
    }
}

void MainWindow::saveAndRefresh() {
    saveEditorData();
    emit refreshRequested();
}

void MainWindow::loadRequiredEditor(const KService::List offers, KUrl target)
{
    // save any previous editor content
    saveEditorData();

    if (offers.isEmpty()) {
        kDebug() << "No offers for editor, can not load.";
        return;
    }

    QVariantList args;
    QString error; // we should show this via debug if we fail
    KParts::ReadOnlyPart *part = dynamic_cast<KParts::ReadOnlyPart*>(
              offers.at(0)->createInstance<KParts::Part>(
                this, args, &error));

    QWidget *mainWidget = 0;
    if (!m_part || !part->inherits(m_part->metaObject()->className())) {
        delete m_part; // reuse if we can
        m_part = part;
        KTextEditor::Document *editorPart = qobject_cast<KTextEditor::Document *>(m_part);
        if (editorPart) {
            KTextEditor::View *view = qobject_cast<KTextEditor::View *>(editorPart->widget());
            setupTextEditor(editorPart, view);
            mainWidget = view;
        } else {
            mainWidget = m_part->widget();
        }
    } else {
        delete part;
        mainWidget = m_part->widget();
    }

    if (!m_part) {
        kDebug() << "Failed to load editor:" << error;
    }

    m_central->switchTo(mainWidget);

    // open the target for editting/viewing
    if (!target.equals(m_part->url()))
        m_part->openUrl(target);
    mainWidget->setMinimumWidth(300);
    //Add the part's GUI
    //createGUI(m_part);

    // We keep only one editor object alive at a time -
    // so we know who to activate when the edit tab is reselected
    delete m_metaEditor;
    m_metaEditor = 0;

    m_sidebar->setCurrentIndex(EditTab);
    m_oldTab = EditTab;
}

void MainWindow::setupTextEditor(KTextEditor::Document *editorPart, KTextEditor::View *view)
{
    //FIXME: we should be setting the highlight based on the type of document
    //editorPart->setHighlightingMode("JavaScript");

    if (view) {
        view->setContextMenu(view->defaultContextMenu());

        KTextEditor::ConfigInterface *config = qobject_cast<KTextEditor::ConfigInterface*>(view);
        if (config) {
            kDebug() << "setting various config values...";
            config->setConfigValue("line-numbers", true);
            config->setConfigValue("dynamic-word-wrap", true);
        }
    }

    KTextEditor::ConfigInterface *config = dynamic_cast<KTextEditor::ConfigInterface*>(editorPart);
    if (config) {
        config->setConfigValue("backup-on-save-prefix", ".");
    }
}

void MainWindow::loadMetaDataEditor(KUrl target) {
    // save any previous editor content
    saveEditorData();

    if (!m_metaEditor)
        m_metaEditor = new MetaDataEditor(this);

    m_metaEditor->setFilename(target.path());
    m_metaEditor->readFile();
    m_central->switchTo(m_metaEditor);

    m_sidebar->setCurrentIndex(EditTab);
    m_oldTab = EditTab;
}

void MainWindow::loadProject(const QString &name, const QString &type)
{
    kDebug() << "Loading project named" << name << "...";
    delete m_model;

    // Saving NewProject preferences
    KConfigGroup preferences = KGlobal::config()->group("NewProjectDefaultPreferences");

    preferences.writeEntry("Username", m_startPage->userName());
    preferences.writeEntry("Email", m_startPage->userEmail());

    preferences.writeEntry("radioButtonJsChecked", m_startPage->selectedJsRadioButton());
    preferences.writeEntry("radioButtonPyChecked", m_startPage->selectedPyRadioButton());
    preferences.writeEntry("radioButtonRbChecked", m_startPage->selectedRbRadioButton());
    preferences.sync();

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
    KConfigGroup cg = KGlobal::config()->group("General");
    recentFiles = recentProjects();

    if (recentFiles.contains(name)) {
        recentFiles.removeAt(recentFiles.indexOf(name));
    }

    if (!name.isEmpty()) {
        recentFiles.prepend(name);
    } else
        return;

    kDebug() << "Writing the following m_sidebar of recent files to the config:" << recentFiles;

    cg.writeEntry("recentFiles", recentFiles);
    KGlobal::config()->sync();

    // Load the needed widgets, switch to page 1 (edit)...
    if(!docksCreated)
        createDockWidgets();
    else { // loading a new project!
        // workaround to completely clear previewer
        delete m_previewer;
        m_previewer = new Previewer(this);
        m_previewerWidget->setWidget(m_previewer);
        // point editor tree to new model
        m_editPage->setModel(m_model);
        // delete old publisher
        delete m_publisher;
        m_publisher = 0;
    }

    QLabel *l = new QLabel(i18n("Select a file to edit."), this);
    m_central->switchTo(l);

    m_oldTab = EditTab;
    m_sidebar->setCurrentIndex(m_oldTab);

    m_timeLine->setWorkingDir(KUrl(packagePath));
    m_timeLine->loadTimeLine(KUrl(packagePath));

    // load project in previewer
    m_previewer->addApplet(packagePath);

    // Now, setup some useful properties such as the project name in the title bar
    // and setting the current working directory.
    Plasma::PackageMetadata metadata(packagePath + "metadata.desktop");
    setCaption("[Project:" + metadata.name() + ']');
    kDebug() << "Content prefix: " << m_model->contentsPrefix() ;
    QDir::setCurrent(m_model->package() + m_model->contentsPrefix());
}

QStringList MainWindow::recentProjects()
        // Q: TODO Limit to 5?
        // A: Before limiting, we need to provide an "Export" feature so
        // the developer can save his projects and import it later for review.
{
    KConfigGroup cg = KGlobal::config()->group("General");
    QStringList l = cg.readEntry("recentFiles", QStringList());
//     kDebug() << l.toStringList();

    return l;
}

