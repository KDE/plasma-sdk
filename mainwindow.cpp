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
#include <KTextEditor/Editor>
#include <KTextEditor/CommandInterface>
#include <KStandardAction>
#include <KUrl>
#include <KListWidget>
#include <KActionCollection>
#include <KParts/Part>
#include <KStandardDirs>
#include <kmimetypetrader.h>
#include <KMessageBox>

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
      m_curMode(Preserve),
      m_curWidget(0)
{
    m_layout = new QVBoxLayout();
    setLayout(m_layout);
}

void MainWindow::CentralContainer::switchTo(QWidget* newWidget, SwitchMode mode)
{
    if (m_curWidget) {
        m_curWidget->hide();
        m_layout->removeWidget(m_curWidget);
        if (m_curMode == DeleteAfter)
            delete m_curWidget;
    }
    m_curMode = mode;
    m_curWidget = newWidget;
    m_layout->addWidget(m_curWidget);
    m_curWidget->show();
}

MainWindow::MainWindow(QWidget *parent)
    : KParts::MainWindow(parent, Qt::Widget),
      m_sidebar(0),
      m_timeLine(0),
      m_previewer(0),
      m_metaEditor(0),
      m_publisher(0),
      m_browser(0),
      m_editWidget(0),
      m_editPage(0),
      m_model(0),
      m_oldTab(0), // we start from startPage
      m_docksCreated(false),
      m_part(0),
      m_notesPart(0)
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

    if (m_sidebar) {
        //delete m_startPage;
        configDock.writeEntry("WorkflowLocation", QVariant(m_sidebar->location()));
        delete m_sidebar;
    }

    if (m_previewer) {
        configDock.writeEntry("PreviewerHeight", m_previewerWidget->height());
        configDock.writeEntry("PreviewerWidth", m_previewerWidget->width());
        c.sync();
        delete m_previewer;
        delete m_previewerWidget;
    }

    if (m_browser) {
        // save current page for restoration next time
        // TODO: Maybe it makes more sense to save this per-project?
        KConfigGroup cg = KGlobal::config()->group("General");
        cg.writeEntry("lastBrowserPage", m_browser->currentPage().toEncoded());
        KGlobal::config()->sync();
        delete m_browser;
    }

    if (m_publisher) {
        delete m_publisher;
    }

    if (m_timeLine) {
        configDock.writeEntry("TimeLineLocation", QVariant(m_timeLine->location()));
        delete m_timeLine;
    }

    if (m_editPage) {
        delete m_editPage;
        delete m_editWidget;
    }
    c.sync();
}

void MainWindow::createMenus()
{
    KStandardAction::quit(this, SLOT(quit()), actionCollection());
    KAction *refresh = KStandardAction::redisplay(this, SLOT(saveAndRefresh()), actionCollection());
    refresh->setShortcut(tr("Ctrl+F5"));
    refresh->setText(i18n("Refresh Previewer"));
    menuBar()->addMenu(helpMenu());
    setupGUI();
}

void MainWindow::createDockWidgets()
{
    KConfig c;
    KConfigGroup configDock = c.group("DocksPosition");
    /////////////////////////////////////////////////////////////////////////
    Qt::DockWidgetArea location = (Qt::DockWidgetArea) configDock.readEntry("WorkflowLocation",
                                                                            QVariant(Qt::TopDockWidgetArea)).toInt();
    m_sidebar = new Sidebar(this,
                            location);
    m_sidebar->setObjectName("workflow");
    addDockWidget(location, m_sidebar);

    m_sidebar->addItem(KIcon("go-home"), i18n("Start page"));
    m_sidebar->addItem(KIcon("accessories-text-editor"), i18n("Edit"));
    m_sidebar->addItem(KIcon("document-save"),i18n("New SavePoint"));
    m_sidebar->addItem(KIcon("krfb"), i18n("Publish"));
    m_sidebar->addItem(KIcon("help-contents"), i18n("Documentation"));
    m_sidebar->addItem(KIcon("user-desktop"), i18n("Preview"));

    connect(m_sidebar, SIGNAL(currentIndexClicked(const QModelIndex &)),
            this, SLOT(changeTab(const QModelIndex &)));

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
    location = (Qt::DockWidgetArea) configDock.readEntry("TimeLineLocation",
                                                         QVariant(Qt::BottomDockWidgetArea)).toInt();
    m_timeLine = new TimeLine(this,
                              m_model->package(),
                              location);

    m_timeLine->setObjectName("timeline");
    connect(m_timeLine, SIGNAL(sourceDirectoryChanged()),
            this, SLOT(editorDestructiveRefresh()));
    connect(m_timeLine, SIGNAL(savePointClicked()),
            this, SLOT(saveEditorData()));
    addDockWidget(location, m_timeLine);
    /////////////////////////////////////////////////////////////////////////
    m_previewerWidget = new QDockWidget(i18n("Previewer"), this);
    m_previewerWidget->setObjectName("preview");
    m_previewer = new Previewer(this);
    m_previewerWidget->setWidget(m_previewer);
    addDockWidget(Qt::LeftDockWidgetArea, m_previewerWidget);

    m_previewerWidget->updateGeometry();
    m_previewer->updateGeometry();

    m_previewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_previewerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // TODO: Should probably make project notes an editor-tree item instead of a
    //       dockwidget, since we're somewhat squeezed on dockwidget space now..
    QDockWidget *m_projectNotesWidget = new QDockWidget(i18n("Project notes"), this);
    m_projectNotesWidget->setObjectName("projectNotes");
    loadNotesEditor(m_projectNotesWidget);
    addDockWidget(Qt::LeftDockWidgetArea, m_projectNotesWidget);

    //splitDockWidget(m_workflow, m_editWidget, Qt::Horizontal);
    splitDockWidget(m_editWidget, m_previewerWidget, Qt::Vertical);

    // Restoring the previous layout
    restoreState(configDock.readEntry("MainWindowLayout",QByteArray()), 0);

    connect(this, SIGNAL(newSavePointClicked()),
            m_timeLine, SLOT(newSavePoint()));

    // Restore browser widget if there is something to restore
    KConfigGroup cg = KGlobal::config()->group("General");
    QString lastPage = cg.readEntry("lastBrowserPage");
    if (lastPage != QString::null) { // restore!
        if (!m_browser) {
            m_browser = new DocBrowser(m_model, this);
        }

        m_browser->load(lastPage);
    }

    m_docksCreated = true;

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

    int tab = (m_sidebar->isVertical()) ? item.row(): item.column();

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
        if (m_metaEditor) {
            m_central->switchTo(m_metaEditor);
        } else if (m_part) {
            m_central->switchTo(m_part->widget());
            m_part->widget()->setFocus(Qt::OtherFocusReason);
        } else {
            QLabel *l = new QLabel(i18n("Select a file to edit."), this);
            m_central->switchTo(l, CentralContainer::DeleteAfter);
        }
    }
    break;
    case PublishTab: {
        if (!m_publisher)
            m_publisher = new Publisher(this, m_model->package(), m_model->packageType());
        m_publisher->setProjectName(m_currentProject);
        m_central->switchTo(m_publisher);
    }
    break;
    case DocsTab: {
        if (!m_browser) {
            m_browser = new DocBrowser(m_model, this);
        }
        m_central->switchTo(m_browser);
        m_browser->focusSearchField();
    }
    break;
    case PreviewTab: {
        Previewer *tabPreviewer = new Previewer(this);
        tabPreviewer->addApplet(m_model->package());
        m_central->switchTo(tabPreviewer, CentralContainer::DeleteAfter);
    }
    }

    m_oldTab = tab;
}

void MainWindow::saveEditorData()
{
    if (qobject_cast<KParts::ReadWritePart*>(m_part)) {
        static_cast<KParts::ReadWritePart*>(m_part)->save();
    }
    if (qobject_cast<KParts::ReadWritePart*>(m_notesPart)) {
        static_cast<KParts::ReadWritePart*>(m_notesPart)->save();
    }
    if (m_metaEditor) {
        m_metaEditor->writeFile();
    }
}

void MainWindow::saveAndRefresh()
{
    saveEditorData();
    emit refreshRequested();
}

void MainWindow::editorDestructiveRefresh()
{
    if (qobject_cast<KParts::ReadOnlyPart*>(m_part)) {
        static_cast<KParts::ReadOnlyPart*>(m_part)->openUrl(
              static_cast<KParts::ReadOnlyPart*>(m_part)->url());
    }
    if (m_metaEditor) {
        m_metaEditor->readFile();
    }
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
    } else {
        delete part;
        //mainWidget = m_part->widget();
    }

    // open the target for editting/viewing
    if (!target.equals(m_part->url())) {
        m_part->openUrl(target);
        KTextEditor::Document *editorPart = qobject_cast<KTextEditor::Document *>(m_part);
        if (editorPart) { // resetup editor if opening new/different file
            KTextEditor::View *view = qobject_cast<KTextEditor::View *>(editorPart->widget());
            setupTextEditor(editorPart, view);
            mainWidget = view;
        } else {
            mainWidget = m_part->widget();
        }
    } else {
      mainWidget = m_part->widget();
    }

    if (!m_part) {
        kDebug() << "Failed to load editor:" << error;
    }

    m_central->switchTo(mainWidget);

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

        // set nice defaults for katepart
        KTextEditor::CommandInterface *command = qobject_cast<KTextEditor::CommandInterface *>(editorPart->editor());
        if (command) {
            QString ret;
            command->queryCommand("set-indent-mode")->exec(view, "set-indent-mode normal", ret); // more friendly
            command->queryCommand("set-replace-tabs")->exec(view, "set-replace-tabs 1", ret); // important for python
            if (m_model->implementationApi() == "python") { // 4 spaces recommended for python
                command->queryCommand("set-indent-width")->exec(view, "set-indent-width 4", ret);
            } else { // 2 spaces recommended for ruby, JS is agnostic
                command->queryCommand("set-indent-width")->exec(view, "set-indent-width 2", ret);
            }
        }
    }

    KTextEditor::ConfigInterface *config = dynamic_cast<KTextEditor::ConfigInterface*>(editorPart);
    if (config) {
        config->setConfigValue("backup-on-save-prefix", ".");
    }
}

void MainWindow::loadNotesEditor(QDockWidget *container)
{
    KService::List offers = KMimeTypeTrader::self()->query("text/plain", "KParts/ReadWritePart");
    if (offers.isEmpty()) {
        offers = KMimeTypeTrader::self()->query("text/plain", "KParts/ReadOnlyPart");
    }
    if (!offers.isEmpty()) {
        QVariantList args;
        QString error;
        m_notesPart = dynamic_cast<KParts::ReadOnlyPart*>(
                      offers.at(0)->createInstance<KParts::Part>(
                      this, args, &error));

        if (!m_notesPart) {
            kDebug() << "Failed to load notes editor:" << error;
        }

        refreshNotes();
        container->setWidget(m_notesPart->widget());
    }
}

void MainWindow::refreshNotes()
{
    if (!m_notesPart) {
      return;
    }
    KParts::ReadWritePart* part = qobject_cast<KParts::ReadWritePart*>(m_notesPart);
    if (part && part->isModified()) {
        part->save(); // save notes if we previously had one open.
    }
    QString notesFile = m_model->package() + "NOTES";
    QFile notes(notesFile);
    if (!notes.exists())
        notes.open(QIODevice::WriteOnly);
    m_notesPart->openUrl(KUrl("file://" + notesFile));
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

    // Converting projects which use ServiceTypes instead of X-KDE-ServiceTypes
    QFile metadataFile(packagePath + "/metadata.desktop");
    metadataFile.open(QIODevice::ReadWrite);
    QString contents = metadataFile.readAll();
    contents.replace(QRegExp("\nServiceTypes"), "\nX-KDE-ServiceTypes");
    metadataFile.resize(0);
    QTextStream stream(&metadataFile);
    stream << contents;
    metadataFile.close();

    if (actualType.isEmpty()) {
        QDir dir(packagePath);
        if (dir.exists("metadata.desktop")) {
            Plasma::PackageMetadata metadata(packagePath + "metadata.desktop");
            actualType = metadata.serviceType();
        }
    }

    // Add it to the recent files first.
    m_model = new PackageModel(this);
    kDebug() << "Setting project type to:" << actualType;
    m_model->setPackageType(actualType);
    kDebug() << "Setting model package to:" << packagePath;

    if (!m_model->setPackage(packagePath))
    {
        KMessageBox::error(this, i18n("Invalid plasmagick package."));
        return;
    }

    QStringList recentFiles;
    KConfigGroup cg = KGlobal::config()->group("General");
    recentFiles = recentProjects();

    if (recentFiles.contains(name)) {
        recentFiles.removeAt(recentFiles.indexOf(name));
    }

    if (!name.isEmpty()) {
        recentFiles.prepend(name);
    } else {
        return;
    }

    kDebug() << "Writing the following m_sidebar of recent files to the config:" << recentFiles;

    cg.writeEntry("recentFiles", recentFiles);
    KGlobal::config()->sync();

    // Load the needed widgets, switch to page 1 (edit)...
    if (!m_docksCreated) {
        createDockWidgets();
    } else { // loading a new project!
        // workaround to completely clear previewer
        delete m_previewer;
        m_previewer = new Previewer(this);
        m_previewerWidget->setWidget(m_previewer);
        // point editor tree to new model
        m_editPage->setModel(m_model);
        // delete old publisher
        delete m_publisher;
        m_publisher = 0;

        if (m_browser) {
            m_browser->setPackage(m_model);
        }

        refreshNotes();
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

    m_currentProject = metadata.name();
}

QStringList MainWindow::recentProjects()
{
    KConfigGroup cg = KGlobal::config()->group("General");
    QStringList l = cg.readEntry("recentFiles", QStringList());
//     kDebug() << l.toStringList();

    return l;
}

