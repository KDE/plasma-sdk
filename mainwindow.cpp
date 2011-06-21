/*
Copyright 2009 Riccardo Iaconelli <riccardo@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDir>
#include <QDockWidget>
#include <QListWidgetItem>
#include <QModelIndex>
#include <QLabel>
#include <QGridLayout>

#include <KTextEdit>

#include <KAction>
#include <KActionCollection>
#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <KMenu>
#include <KMenuBar>
#include <KMimeTypeTrader>
#include <KTextEditor/ConfigInterface>
#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KTextEditor/Editor>
#include <KTextEditor/CommandInterface>
#include <KToolBar>
#include <KStandardAction>
#include <KUrl>
#include <KListWidget>
#include <KActionCollection>
#include <KParts/Part>
#include <KStandardDirs>
#include <KMessageBox>

#include <Plasma/PackageMetadata>

#include "editors/editpage.h"
#include "editors/metadata/metadataeditor.h"
#include "savesystem/timeline.h"
#include "mainwindow.h"
#include "packagemodel.h"
#include "sidebar.h"
#include "startpage.h"
#include "previewer/plasmoid/plasmoidpreviewer.h"
#include "previewer/runner/runnerpreviewer.h"
#include "publisher/publisher.h"
#include "docbrowser/docbrowser.h"

#include "modeltest/modeltest.h"

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
        if (m_curMode == DeleteAfter) {
            delete m_curWidget;
        }
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
      m_previewerWidget(0),
      m_metaEditor(0),
      m_publisher(0),
      m_browser(0),
      m_editWidget(0),
      m_editPage(0),
      m_model(0),
      m_oldTab(0), // we start from startPage
      m_docksCreated(false),
      m_part(0),
      m_notesPart(0),
      m_notesWidget(0)
{
    setXMLFile("plasmateui.rc");
    setupActions();
    createMenus();
    toolBar()->hide();
    menuBar()->hide();
    m_startPage = new StartPage(this);
    connect(m_startPage, SIGNAL(projectSelected(QString, QString)),
            this, SLOT(loadProject(QString, QString)));
    m_central = new CentralContainer(this);
    setCentralWidget(m_central);
    m_central->switchTo(m_startPage);
    setDockOptions(QMainWindow::AllowNestedDocks); // why not?

    if (autoSaveConfigGroup().entryMap().isEmpty()) {
        setWindowState(Qt::WindowMaximized);
    }
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
    delete m_publisher;
    m_publisher = 0;
    delete m_editPage;
    m_editPage = 0;
    delete m_editWidget;
    m_editWidget = 0;

    if (m_previewerWidget) {
        configDock.writeEntry("PreviewerHeight", m_previewerWidget->height());
        configDock.writeEntry("PreviewerWidth", m_previewerWidget->width());
        c.sync();
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

    if (m_timeLine) {
        configDock.writeEntry("TimeLineLocation", QVariant(m_timeLine->location()));
        delete m_timeLine;
    }

    c.sync();
}

void MainWindow::openDocumentation()
{
    if (!m_browser) {
        m_browser = new DocBrowser(m_model, 0);
        connect(m_browser, SIGNAL(visibilityChanged(bool)), this, SLOT(updateActions()));
    }

    m_browser->setVisible(!m_browser->isVisible());
    m_browser->setObjectName("Documentation");
    m_browser->focusSearchField();
    addDockWidget(Qt::LeftDockWidgetArea, m_browser);
}

void MainWindow::createMenus()
{
    KStandardAction::quit(this, SLOT(quit()), actionCollection());
    KAction *refresh = KStandardAction::redisplay(this, SLOT(saveAndRefresh()), actionCollection());
    refresh->setShortcut(Qt::CTRL + Qt::Key_F5);
    refresh->setText(i18n("Refresh Preview"));
    menuBar()->addMenu(helpMenu());
    setupGUI();
}

void MainWindow::quit()
{
    qApp->closeAllWindows();
//     deleteLater();
}

KAction *MainWindow::addAction(QString text, const char * icon, const  char *slot, const char *name, const KShortcut &shortcut)
{
    KAction *action = new KAction(this);
    action->setText(text);
    action->setIcon(KIcon(icon));
    action->setShortcut(shortcut);
    connect(action, SIGNAL(triggered(bool)), this, slot);
    actionCollection()->addAction(name, action);
    return action;
}

void MainWindow::setupActions()
{
    addAction(i18n("Create Save Point"), "document-save", SLOT(selectSavePoint()), "savepoint", KStandardShortcut::save());
    addAction(i18n("Publish"), "krfb", SLOT(selectPublish()),   "publish");

    addAction(i18n("Close Project"), "plasmagik", SLOT(showStartPage()),   "file_startpage");
    addAction(i18n("Preview"), "user-desktop", SLOT(togglePreview()), "preview")->setCheckable(true);
    addAction(i18n("Notes"), "accessories-text-editor", SLOT(toggleNotes()), "notes")->setCheckable(true);
    addAction(i18n("Files"), "system-file-manager", SLOT(toggleFileList()), "file_list")->setCheckable(true);
    addAction(i18n("Timeline"), "process-working",  SLOT(toggleTimeLine()), "timeline")->setCheckable(true);
    addAction(i18n("Documentation"), "help-contents", SLOT(openDocumentation()), "documentation")->setCheckable(true);
}

void MainWindow::updateActions()
{
    actionCollection()->action("preview")->setChecked(m_previewerWidget && m_previewerWidget->isVisible());
    actionCollection()->action("notes")->setChecked(m_notesWidget && m_notesWidget->isVisible());
    actionCollection()->action("file_list")->setChecked(m_editWidget && m_editWidget->isVisible());
    actionCollection()->action("timeline")->setChecked(m_timeLine && m_timeLine->isVisible());
    actionCollection()->action("documentation")->setChecked(m_browser && m_browser->isVisible());
}

void MainWindow::showStartPage()
{
    toolBar()->hide();
    menuBar()->hide();
    setCentralWidget(m_central);
    m_central->switchTo(m_startPage);
    setDockOptions(QMainWindow::AllowNestedDocks);
}

void MainWindow::toggleTimeLine()
{
    if (!m_timeLine) {
        initTimeLine();
    }

    m_timeLine->setVisible(!m_timeLine->isVisible());
}

void MainWindow::initTimeLine()
{
    KConfig c;
    KConfigGroup configDock = c.group("DocksPosition");
    Qt::DockWidgetArea location = (Qt::DockWidgetArea) configDock.readEntry("WorkflowLocation",
                          QVariant(Qt::TopDockWidgetArea)).toInt();
    location = (Qt::DockWidgetArea) configDock.readEntry("TimeLineLocation",
                                                         QVariant(Qt::BottomDockWidgetArea)).toInt();

    if (!m_timeLine) {
        m_timeLine = new TimeLine(this, m_model->package(), location);
        m_timeLine->setObjectName("timeline");
        connect(m_timeLine, SIGNAL(sourceDirectoryChanged()), this, SLOT(editorDestructiveRefresh()));
        connect(m_timeLine, SIGNAL(savePointClicked()), this, SLOT(saveEditorData()));
        connect(m_timeLine, SIGNAL(visibilityChanged(bool)), this, SLOT(updateActions()));
        connect(this, SIGNAL(newSavePointClicked()), m_timeLine, SLOT(newSavePoint()));
        addDockWidget(location, m_timeLine);
        m_timeLine->loadTimeLine(m_model->package());
        m_timeLine->setVisible(false);
    } else {
        // The TimeLine is already created, but we changed project, so we need to update the working directory
        m_timeLine->setWorkingDir(m_model->package());
    }
}

void MainWindow::toggleFileList()
{
    setFileListVisible(!m_editWidget || !m_editWidget->isVisible());
}

void MainWindow::setFileListVisible(const bool visible)
{
    if (visible && !m_editWidget) {
        m_editWidget = new QDockWidget(i18n("Files"), this);
        m_editWidget->setObjectName("edit tree");
        m_editWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        m_editWidget->setWidget(m_editPage);

        if (m_previewerWidget) {
            splitDockWidget(m_previewerWidget, m_editWidget, Qt::Vertical);
        } else {
            addDockWidget(Qt::LeftDockWidgetArea, m_editWidget);
        }

        connect(m_editWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(updateActions()));
    }

    if (m_editWidget) {
        m_editWidget->setVisible(visible);
    }
}

void MainWindow::toggleNotes()
{
    setNotesVisible(!m_notesWidget || !m_notesWidget->isVisible());
}

void MainWindow::setNotesVisible(const bool visible)
{
    if (visible && !m_notesWidget) {
        m_notesWidget = new QDockWidget(i18n("Notes"), this);
        m_notesWidget->setObjectName("projectNotes");
        loadNotesEditor(m_notesWidget);
        addDockWidget(Qt::BottomDockWidgetArea, m_notesWidget);
        connect(m_notesWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(updateActions()));
    }

    if (m_notesWidget) {
        m_notesWidget->setVisible(visible);
    }
}

void MainWindow::selectSavePoint()
{
   if (!m_timeLine) {
     initTimeLine();
  }

  saveEditorData();
  emit newSavePointClicked();
}

void MainWindow::selectPublish()
{
  if (!m_publisher) {
    m_publisher = new Publisher(this, m_model->package(), m_model->packageType());
  }
  m_publisher->setProjectName(m_currentProject);
  m_publisher->exec();
}

void MainWindow::togglePreview()
{
    m_previewerWidget->setVisible(!m_previewerWidget->isVisible());
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
    if (m_previewerWidget) {
        m_previewerWidget->refreshPreview();
    }
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
    if (qobject_cast<KParts::ReadOnlyPart*>(m_notesPart)) {
        static_cast<KParts::ReadOnlyPart*>(m_notesPart)->openUrl(
              static_cast<KParts::ReadOnlyPart*>(m_notesPart)->url());
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
    m_oldTab = EditTab;
}

void MainWindow::setupTextEditor(KTextEditor::Document *editorPart, KTextEditor::View *view)
{
    //FIXME: we should be setting the highlight based on the type of document
    //editorPart->setHighlightingMode("JavaScript");
    //FIXME: (probably related to the above) If I open a python file then immediately a
    // js file, editor tries to add a python-style encoding comment at the top of the js file.
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

        // use same backup file format as above so that it is gitignored
        KTextEditor::ConfigInterface *config = dynamic_cast<KTextEditor::ConfigInterface*>(m_notesPart);
        if (config) {
            config->setConfigValue("backup-on-save-prefix", ".");
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
    QDir notesDir(m_model->package());
    notesDir.cdUp();
    QString notesFile = notesDir.absolutePath() + "/NOTES";
    QFile notes(notesFile);
    if (!notes.exists())
        notes.open(QIODevice::WriteOnly);
    m_notesPart->openUrl(KUrl("file://" + notesFile));
}

void MainWindow::loadMetaDataEditor(KUrl target)
{
    // save any previous editor content
    saveEditorData();

    if (!m_metaEditor) {
        m_metaEditor = new MetaDataEditor(this);
    }

    m_metaEditor->setFilename(target.path());
    m_metaEditor->readFile();
    m_central->switchTo(m_metaEditor);

    if (m_sidebar) {
        m_sidebar->setCurrentIndex(EditTab);
    }
    m_oldTab = EditTab;
}

void MainWindow::loadProject(const QString &name, const QString &type)
{
    m_currentProject = name;
    m_currentProjectType = type;
    kDebug() << "Loading project named" << name << "...";
    delete m_model;
    toolBar()->show();

    // Saving NewProject preferences
    KConfigGroup preferences = KGlobal::config()->group("NewProjectDefaultPreferences");

    preferences.writeEntry("Username", m_startPage->userName());
    preferences.writeEntry("Email", m_startPage->userEmail());

    preferences.writeEntry("radioButtonJsChecked", m_startPage->selectedJsRadioButton());
    preferences.writeEntry("radioButtonPyChecked", m_startPage->selectedPyRadioButton());
    preferences.writeEntry("radioButtonRbChecked", m_startPage->selectedRbRadioButton());
    preferences.writeEntry("radioButtonDeChecked", m_startPage->selectedDeRadioButton());
    preferences.sync();

    QString packagePath;
    QDir pDir(name);
    if (pDir.isRelative()) {
        packagePath = KStandardDirs::locateLocal("appdata", name + '/');
    } else {
        packagePath = name;
    }

    if (!packagePath.endsWith('/')) {
        packagePath.append('/');
    }

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

    //Workaround for Plasma::PackageStructure not recognizing Plasma/PopupApplet as a valid type
    //FIXME:
   QString fixedType;
    if (actualType == "Plasma/Applet,Plasma/PopupApplet") {
        fixedType = "Plasma/Applet";
    } else {
        fixedType = actualType;
    }

    // Add it to the recent files first.
    m_model = new PackageModel(this);
#ifdef DEBUG_MODEL
    new ModelTest(m_model, this);
#endif
    kDebug() << "Setting project type to:" << fixedType;
    m_model->setPackageType(fixedType);
    kDebug() << "Setting model package to:" << packagePath;

    if (!m_model->setPackage(packagePath))
    {
        KMessageBox::error(this, i18n("Invalid plasmagick package."));
        return;
    }
    m_editPage = new EditPage();
    m_editPage->setModel(m_model);

    connect(m_editPage, SIGNAL(loadEditor(KService::List, KUrl)), this, SLOT(loadRequiredEditor(const KService::List, KUrl)));
    connect(m_editPage, SIGNAL(loadMetaDataEditor(KUrl)), this, SLOT(loadMetaDataEditor(KUrl)));

    m_editPage->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

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
        //createDockWidgets();
    } else { // loading a new project!
        // prevent accidental loading of previous active project's file
        // plus temporary workaround for editor issue with handling different languages
        delete m_part;
        m_part = 0;
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

//    QDir projectPath(packagePath);
//     if (projectPath.cdUp()) {
//         m_timeLine->setWorkingDir(KUrl(projectPath.absolutePath()));
//         m_timeLine->loadTimeLine(KUrl(projectPath.absolutePath()));
//     }
    QByteArray state = saveState();

    // initialize previewer
    delete m_previewerWidget;
    m_previewerWidget = createPreviewerFor(actualType);
    if (m_previewerWidget) {
        addDockWidget(Qt::LeftDockWidgetArea, m_previewerWidget);
        m_previewerWidget->showPreview(packagePath);
        m_previewerWidget->setVisible(true);
    }

    setFileListVisible(true);
    restoreState(state);

    // Now, setup some useful properties such as the project name in the title bar
    // and setting the current working directory.
    Plasma::PackageMetadata metadata(packagePath + "metadata.desktop");
    setCaption("[Project:" + metadata.name() + ']');
    kDebug() << "Content prefix: " << m_model->contentsPrefix() ;
    QDir::setCurrent(m_model->package() + m_model->contentsPrefix());

    m_currentProject = metadata.name();

    // load mainscript
    kDebug() << "loading metadata:" << packagePath + "metadata.desktop";
    KConfig *metafile = new KConfig(packagePath + "metadata.desktop");
    KConfigGroup meta = metafile->group("Desktop Entry");
    QString mainScript = meta.readEntry("X-Plasma-MainScript", QString());
    kDebug() << "read mainScript" << mainScript;
    if (!mainScript.isEmpty()) {
        KUrl url = KUrl(packagePath + "contents/" + mainScript);
        m_editPage->loadFile(url);
    }
    // After we loaded the project, init the TimeLine and Previewer component
    menuBar()->show();
    updateActions();
}

QStringList MainWindow::recentProjects()
{
    KConfigGroup cg = KGlobal::config()->group("General");
    QStringList l = cg.readEntry("recentFiles", QStringList());
//     kDebug() << l.toStringList();

    return l;
}

Previewer* MainWindow::createPreviewerFor(const QString& projectType)
{
    Previewer* ret = 0;
    if (projectType == "Plasma/Applet" ||
        projectType == "Plasma/Applet,Plasma/PopupApplet") {
        ret = new PlasmoidPreviewer(i18n("Preview"), this);
    } else if (projectType == "Plasma/Runner") {
        ret = new RunnerPreviewer(i18n("Previewer"), this);
    }

    if (ret) {
        ret->setObjectName("preview");
        connect(ret, SIGNAL(refreshRequested()), this, SLOT(saveAndRefresh()));
        connect(ret, SIGNAL(visibilityChanged(bool)), this, SLOT(updateActions()));
    }

    return ret;
}
