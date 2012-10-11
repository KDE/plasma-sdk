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

#include <iostream>

#include <QDir>
#include <QDockWidget>
#include <QListWidgetItem>
#include <QModelIndex>
#include <QLabel>
#include <QGridLayout>
#include <QTextStream>

#include <KTextEdit>

#include <KAction>
#include <KActionCollection>
#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <KMenu>
#include <KMenuBar>
#include <KMimeTypeTrader>
#include <KStandardAction>
#include <KTextEditor/ConfigInterface>
#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KTextEditor/Editor>
#include <KTextEditor/CommandInterface>
#include <KToolBar>
#include <KStandardAction>
#include <KUrl>
#include <KActionCollection>
#include <KParts/Part>
#include <KStandardDirs>
#include <KMessageBox>
#include <KXMLGUIFactory>

#include "editors/editpage.h"
#include "editors/metadata/metadataeditor.h"
#include "editors/imageviewer/imageviewer.h"
#include "editors/text/texteditor.h"
#include "editors/kconfigxt/kconfigxteditor.h"
#include "savesystem/timeline.h"
#include "mainwindow.h"
#include "packagemodel.h"
#include "sidebar.h"
#include "startpage.h"
#include "konsole/konsolepreviewer.h"
#include "previewer/plasmoid/plasmoidpreviewer.h"
#include "previewer/runner/runnerpreviewer.h"
#include "previewer/windowswitcher/tabboxpreviewer.h"
#include "publisher/publisher.h"
#include "docbrowser/docbrowser.h"

#include "modeltest/modeltest.h"

static const int STATE_VERSION = 0;

MainWindowWrapper::MainWindowWrapper(QObject *parent)
    : QObject(parent),
    m_mainWindow(0)
{
    m_mainWindow = new MainWindow();
    m_mainWindow->show();
}

MainWindowWrapper::~MainWindowWrapper()
{
    delete m_mainWindow;
}

MainWindow* MainWindowWrapper::mainWindow()
{
    return m_mainWindow;
}

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
    if (m_curWidget == newWidget) {
        return;
    }

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
        m_textEditor(0),
        m_filelist(0),
        m_editPage(0),
        m_imageViewer(0),
        m_model(0),
        m_oldTab(0), // we start from startPage
        m_docksCreated(false),
        m_isPlasmateCreatedPackage(true),
        m_part(0),
        m_notesWidget(0),
        m_kconfigXtEditor(0),
        m_konsoleWidget(0),
        m_notesPart(0)

{
    setXMLFile("plasmateui.rc");
    setupActions();
    createMenus();
    toolBar()->hide();
    menuBar()->hide();
    m_startPage = new StartPage(this);
    connect(m_startPage, SIGNAL(projectSelected(QString)), this, SLOT(loadProject(QString)));
    m_central = new CentralContainer(this);
    setCentralWidget(m_central);
    m_central->switchTo(m_startPage);
    setDockOptions(QMainWindow::AllowNestedDocks); // why not?
    if (autoSaveConfigGroup().entryMap().isEmpty()) {
        setWindowState(Qt::WindowMaximized);
    }
    connect(this, SIGNAL(sendMessage(QtMsgType, const QString)), this, SLOT(customMessageHandler(QtMsgType,QString)));
}

void MainWindow::emitSendMessage(QtMsgType type, const QString& msg)
{
    emit sendMessage(type, msg);
}

MainWindow::~MainWindow()
{
    // Saving layout position
    KConfigGroup configDock(KGlobal::config(), "DocksPosition");
    configDock.writeEntry("MainWindowLayout", saveState(STATE_VERSION));

    // if the user closes the application with an editor open, should
    // save its contents
    saveEditorData();

    factory()->removeClient(m_part);
    delete m_part;
    delete m_metaEditor;
    delete m_publisher;
    delete m_editPage;
    delete m_filelist;

    if (m_previewerWidget) {
        configDock.writeEntry("PreviewerHeight", m_previewerWidget->height());
        configDock.writeEntry("PreviewerWidth", m_previewerWidget->width());
        delete m_previewerWidget;
    }

    if (m_timeLine) {
        configDock.writeEntry("TimeLineLocation", QVariant(m_timeLine->location()));
        delete m_timeLine;
    }

    KGlobal::config()->sync();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveProjectState();
    KParts::MainWindow::closeEvent(event);
}

void MainWindow::toggleDocumentation()
{
    showDocumentation(!m_browser || m_browser->isHidden());
}

void MainWindow::showDocumentation(bool show)
{
    if (!m_browser) {
        if (!show) {
            return;
        }

        m_browser = new DocBrowser(m_model, this);
        connect(m_browser, SIGNAL(visibilityChanged(bool)), this, SLOT(updateActions()));
        m_browser->setObjectName("Documentation");
        // FIXME: should open the same place it closed at
        addDockWidget(Qt::LeftDockWidgetArea, m_browser);
    }

    m_browser->setVisible(show);
    if (show) {
        m_browser->focusSearchField();
    }
}

void MainWindow::createMenus()
{
    menuBar()->addMenu(helpMenu());
    setupGUI(ToolBar | Keys | StatusBar | Save);
}

void MainWindow::quit()
{
    qApp->closeAllWindows();
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
    KAction *close = KStandardAction::close(this, SLOT(closeProject()), actionCollection());
    close->setText(i18n("Close Project"));

    KAction *quitAction = KStandardAction::quit(this, SLOT(quit()), actionCollection());
    QWidget::addAction(quitAction);

    KAction *refresh = KStandardAction::redisplay(this, SLOT(saveAndRefresh()), actionCollection());
    refresh->setShortcut(Qt::CTRL + Qt::Key_F5);
    refresh->setText(i18n("Refresh Preview"));

    addAction(i18n("Install Project"), "plasmagik", SLOT(installPackage()), "installproject", KShortcut(Qt::META + Qt::Key_I));
    addAction(i18n("Create Save Point"), "document-save", SLOT(selectSavePoint()), "savepoint", KStandardShortcut::save());
    addAction(i18n("Publish"), "krfb", SLOT(selectPublish()),   "publish");
    addAction(i18n("Preview"), "user-desktop", SLOT(togglePreview()), "preview")->setCheckable(true);
    addAction(i18n("Notes"), "accessories-text-editor", SLOT(toggleNotes()), "notes")->setCheckable(true);
    addAction(i18n("Files"), "system-file-manager", SLOT(toggleFileList()), "file_list")->setCheckable(true);
    addAction(i18n("Timeline"), "process-working",  SLOT(toggleTimeLine()), "timeline")->setCheckable(true);
    addAction(i18n("Documentation"), "help-contents", SLOT(toggleDocumentation()), "documentation")->setCheckable(true);
}

void MainWindow::updateActions()
{
    actionCollection()->action("preview")->setChecked(m_previewerWidget && m_previewerWidget->isVisible());
    actionCollection()->action("notes")->setChecked(m_notesWidget && m_notesWidget->isVisible());
    actionCollection()->action("file_list")->setChecked(m_filelist && m_filelist->isVisible());
    actionCollection()->action("timeline")->setChecked(m_timeLine && m_timeLine->isVisible());
    actionCollection()->action("documentation")->setChecked(m_browser && m_browser->isVisible());
}

void MainWindow::installPackage()
{
    if (!m_publisher) {
        m_publisher = new Publisher(this, m_model->package(), m_model->packageType());
    }

    saveEditorData();

    m_publisher->setProjectName(m_currentProject);
    m_publisher->doPlasmaPkg();
}

void MainWindow::closeProject()
{
    saveEditorData();
    saveProjectState();
    toolBar()->hide();
    menuBar()->hide();
    if (m_timeLine) {
        m_timeLine->hide();
    }

    if (m_previewerWidget) {
        m_previewerWidget->hide();
    }

    if (m_konsoleWidget) {
        m_konsoleWidget->hide();
    }

    if (m_notesWidget) {
        m_notesWidget->hide();
    }

    if (m_filelist) {
        m_filelist->hide();
    }

     if (m_browser) {
        m_browser->hide();
    }

    setCentralWidget(m_central);

    m_central->switchTo(m_startPage);
    setDockOptions(QMainWindow::AllowNestedDocks);
    m_startPage->cancelNewProject();
}

void MainWindow::toggleTimeLine()
{
    if (!m_timeLine) {
        initTimeLine();
    } else {
        m_timeLine->setVisible(!m_timeLine->isVisible());
    }
}

void MainWindow::initTimeLine()
{
    if (!m_timeLine) {
        //FIXME: should come from project specific save data if it exists
        KConfigGroup configDock(KGlobal::config(), "DocksPosition");
        Qt::DockWidgetArea location = (Qt::DockWidgetArea)configDock.readEntry("TimeLineLocation", (int)Qt::BottomDockWidgetArea);
        m_timeLine = new TimeLine(this, m_model->package(), location);
        m_timeLine->setObjectName("timeline");
        connect(m_timeLine, SIGNAL(sourceDirectoryChanged()), this, SLOT(editorDestructiveRefresh()));
        connect(m_timeLine, SIGNAL(savePointClicked()), this, SLOT(saveEditorData()));
        connect(m_timeLine, SIGNAL(visibilityChanged(bool)), this, SLOT(updateActions()));
        connect(this, SIGNAL(newSavePointClicked()), m_timeLine, SLOT(newSavePoint()));
        addDockWidget(location, m_timeLine);
    }

    KUrl directory = m_model->package();
    if (QDir(directory.path() + "/contents").exists()) {
            m_timeLine->loadTimeLine(directory);
    }
}

void MainWindow::toggleFileList()
{
    setFileListVisible(!m_filelist || !m_filelist->isVisible());
}

void MainWindow::setFileListVisible(const bool visible)
{
    if (visible && !m_filelist) {
        m_filelist = new QDockWidget(i18n("Files"), this);
        m_filelist->setObjectName("edit tree");
        m_filelist->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        m_filelist->setWidget(m_editPage);

        if (m_previewerWidget) {
            splitDockWidget(m_previewerWidget, m_filelist, Qt::Vertical);
        } else {
            addDockWidget(Qt::LeftDockWidgetArea, m_filelist);
        }

        connect(m_filelist, SIGNAL(visibilityChanged(bool)), this, SLOT(updateActions()));
    }

    if (m_filelist) {
        m_filelist->setVisible(visible);
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

    saveEditorData();

    m_publisher->setProjectName(m_currentProject);
    m_publisher->exec();
}

void MainWindow::togglePreview()
{
    if (m_previewerWidget) {
        m_previewerWidget->setVisible(!m_previewerWidget->isVisible());
        if (m_previewerWidget->isVisible()) {
            m_previewerWidget->refreshPreview();
        }
    }
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
        connect(m_metaEditor, SIGNAL(apiChanged()), SLOT(checkProjectrc()));

    }
}

void MainWindow::saveAndRefresh()
{
    //in every new save clear the konsole.
    m_konsoleWidget->clearTmpFile();

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

    KService::Ptr service = offers.at(0);
    if (!m_partService || m_partService->storageId() != service->storageId() || !m_part) {
        m_partService = service;
        QString error; // we should show this via debug if we fail
        KParts::ReadOnlyPart *part = dynamic_cast<KParts::ReadOnlyPart*>(
                offers.at(0)->createInstance<KParts::Part>(
                    this, QVariantList(), &error));

        //the widget which will be loaded to the plasmate
        if (!part) {
            //this means failure. A KMessageBox will appear(see the code above)
            delete m_textEditor;
            m_textEditor = 0;

            delete m_part;
            m_part = 0;
        } else if (!m_part || !part->inherits(m_part->metaObject()->className())) {
            delete m_textEditor;
            m_textEditor = 0;

            delete m_part;
            m_part = part;
            KTextEditor::Document *editorPart = qobject_cast<KTextEditor::Document *>(m_part);
            if (editorPart) {
                m_textEditor = new TextEditor(editorPart, m_model, this);
            }
            createGUI(m_part);
        } else {
            // reuse m_part if we can
            delete part;
        }

        if (!m_part) {
            KMessageBox::error(this, i18n("Failed to load editor for %1:\n\n%2", target.prettyUrl(), error), i18n("Loading Failure"));
            return;
        }
    }

    if (!m_part) {
        KMessageBox::error(this, i18n("Failed to load editor for %1", target.prettyUrl()), i18n("Loading Failure"));
        return;
    }

    // open the target for editting/viewing if it isn't already viewing that file
    if (!target.equals(m_part->url())) {
        m_part->openUrl(target);
    }

    QWidget *mainWidget = m_textEditor ? m_textEditor : m_part->widget();
    m_central->switchTo(mainWidget);
    mainWidget->setMinimumWidth(300);
    //Add the part's GUI
    if (m_textEditor) {
        m_textEditor->modifyToolBar(toolBar());
    }

    // We keep only one editor object alive at a time -
    // so we know who to activate when the edit tab is reselected
    delete m_metaEditor;
    m_metaEditor = 0;
    m_oldTab = EditTab;
}

void MainWindow::loadNotesEditor(QDockWidget *container)
{
    delete m_notesPart;
    m_notesPart = 0;

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

    const QString notesFile = projectFilePath(".NOTES");
    QFile notes(notesFile);
    if (!notes.exists()) {
        notes.open(QIODevice::WriteOnly);
        notes.close();
    }
    m_notesPart->openUrl(KUrl("file://" + notesFile));
}

QString MainWindow::projectFilePath(const QString &filename)
{
    if (!m_model) {
        return QString();
    }

    QDir packageDir(m_model->package());
    if (m_isPlasmateCreatedPackage) {
        packageDir.cdUp();
    }

    return packageDir.absolutePath() + "/" + filename;
}

void MainWindow::saveProjectState()
{
    kDebug() << m_model << saveState(STATE_VERSION);
    if (!m_model) {
        return;
    }

    const QString projectrc = projectFilePath(PROJECTRC);
    KConfig c(projectrc);
    KConfigGroup configDocks(&c, "DocksPosition");
    configDocks.writeEntry("MainWindowLayout", saveState(STATE_VERSION));
    configDocks.writeEntry("Timeline", m_timeLine && m_timeLine->isVisible());
    configDocks.writeEntry("Documentation", m_browser && m_browser->isVisible());
    configDocks.writeEntry("FileList", m_filelist && m_filelist->isVisible());
    configDocks.writeEntry("Notes", m_notesWidget && m_notesWidget->isVisible());
    configDocks.writeEntry("Previewer", m_previewerWidget && m_previewerWidget->isVisible());
    c.sync();

    /* TODO: implement browser state loading
    if (m_browser) {
        KConfigGroup cg(KGlobal::config(), "General");
        cg.writeEntry("lastBrowserPage", m_browser->currentPage().toEncoded());
    */
}

void MainWindow::updateSideBar()
{
    if (m_sidebar) {
        m_sidebar->setCurrentIndex(EditTab);
    }
    m_oldTab = EditTab;
}

void MainWindow::loadImageViewer(const KUrl& target)
{
    saveEditorData();

    if (!m_imageViewer) {
        m_imageViewer = new ImageViewer(this);
    }

    m_imageViewer->loadImage(target);
    m_central->switchTo(m_imageViewer);
    updateSideBar();
}

void MainWindow::loadKConfigXtEditor(const KUrl& target)
{
    saveEditorData();
    if (!m_kconfigXtEditor) {
        m_kconfigXtEditor = new KConfigXtEditor(this);
    }

    m_kconfigXtEditor->setFilename(target);
    m_kconfigXtEditor->readFile();
    m_central->switchTo(m_kconfigXtEditor);

    updateSideBar();
}

void MainWindow::showKonsolePreviewer()
{
    if(m_konsoleWidget->isVisible()) {
        m_konsoleWidget->setVisible(false);
    } else {
        m_konsoleWidget->setVisible(true);
    }
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

    updateSideBar();
}

void MainWindow::loadProject(const QString &path)
{
    if (path.isEmpty()) {
        kDebug() << "path is empty?!";
        return;
    }

    QString packagePath;
    QDir pDir(path);
    if (pDir.isRelative()) {
        packagePath = KStandardDirs::locateLocal("appdata", path + '/');
    } else {
        packagePath = path;
    }

    if (!packagePath.endsWith('/')) {
        packagePath.append('/');
    }

    QDir dir(packagePath);
    if (!dir.exists("metadata.desktop")) {
        kDebug() << "no metadata.desktop?!";
        return;
    }

    // if the project rc file is IN the package, then this was loaded from an existing local project
    // otherwise, we assume it was created by plasmate and the project files are up one dir
    m_isPlasmateCreatedPackage = !QFile::exists(packagePath + PROJECTRC);
    m_currentProject = path;
    kDebug() << "Loading project from" << packagePath;
    KService service(packagePath + "metadata.desktop");
    QStringList types = service.serviceTypes();

    // Workaround for Plasma::PackageStructure not recognizing Plasma/PopupApplet as a valid type
    QString actualType;
    if (types.contains("KWin/WindowSwitcher")) {
        actualType = "Plasma/Applet";
    } else if (types.contains("KWin/Script")) {
        actualType = "Plasma/Applet";
    } else if (types.contains("Plasma/Applet")) {
        actualType = "Plasma/Applet";
    } else if (types.contains("KWin/Effect")) {
        actualType = "Plasma/Applet";
    } else {
        actualType = types.first();
    }

    QString previewerType;
    if (types.contains("KWin/WindowSwitcher")) {
        previewerType = "KWin/WindowSwitcher";
    } else if (types.contains("KWin/Script")) {
        //KWin Scripts doesn't have a previewer but we want the previewer to store their type
        //because we will not use it on MainWindow::createPreviewerFor, so the previewer will be disable
        previewerType = "KWin/Script";
    } else if (types.contains("KWin/Effect")) {
        previewerType = "KWin/Effect";
    } else {
        previewerType = actualType;
    }

    delete m_model;
    m_model = new PackageModel(this);
#ifdef DEBUG_MODEL
    new ModelTest(m_model, this);
#endif
    kDebug() << "Setting project type to:" << actualType;
    m_model->setPackageType(actualType);
    kDebug() << "Setting model package to:" << packagePath;

    if (!m_model->setPackage(packagePath)) {
        KMessageBox::error(this, i18n("Invalid Plasma package."));
        return;
    }

    if (!m_editPage) {
        m_editPage = new EditPage();
        connect(m_editPage, SIGNAL(loadEditor(KService::List, KUrl)), this, SLOT(loadRequiredEditor(const KService::List, KUrl)));
        connect(m_editPage, SIGNAL(loadMetaDataEditor(KUrl)), this, SLOT(loadMetaDataEditor(KUrl)));
        connect(m_editPage, SIGNAL(loadImageViewer(KUrl)), this, SLOT(loadImageViewer(KUrl)));
        connect(m_editPage, SIGNAL(loadKConfigXtEditor(KUrl)), this, SLOT(loadKConfigXtEditor(KUrl)));
        m_editPage->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    }

    m_editPage->setModel(m_model);
    m_editPage->expandAll();

    // prevent accidental loading of previous active project's file
    // plus temporary workaround for editor issue with handling different languages
    delete m_part;
    m_part = 0;

    // delete old publisher
    delete m_publisher;
    m_publisher = 0;

    QLabel *l = new QLabel(i18n("Select a file to edit."), this);
    m_central->switchTo(l);

    m_oldTab = EditTab;

    QByteArray state = saveState(STATE_VERSION);
    const QString projectrc = projectFilePath(PROJECTRC);
    bool showPreview = true;
    kDebug() << "******** checking for" << projectrc;
    if (QFile::exists(projectrc)) {
        KConfig c(projectrc);
        KConfigGroup configDocks(&c, "DocksPosition");
        state = configDocks.readEntry("MainWindowLayout", state);

        if (configDocks.readEntry("Timeline", false)) {
            initTimeLine();
        } else {
            delete m_timeLine;
            m_timeLine = 0;
        }

        if (configDocks.readEntry("Documentation", false)) {
            showDocumentation(true);
        } else {
            delete m_browser;
            m_browser = 0;
        }

        setFileListVisible(configDocks.readEntry("FileList", true));
        setNotesVisible(configDocks.readEntry("Notes", false));
        showPreview = configDocks.readEntry("Previewer", showPreview);
    } else {
        setFileListVisible(true);
    }

    if (m_browser) {
        m_browser->setPackage(m_model);
    }

    if (m_notesPart) {
        refreshNotes();
    }


    //initialize the konsole previewer
    m_konsoleWidget = new KonsolePreviewer(i18n("Previewer Output"), this);

    //after the init, cleat the tmp file
    m_konsoleWidget->clearTmpFile();

    // initialize previewer
    delete m_previewerWidget;
    m_previewerWidget = createPreviewerFor(previewerType);
    actionCollection()->action("preview")->setEnabled(m_previewerWidget);
    if (m_previewerWidget) {
        addDockWidget(Qt::LeftDockWidgetArea, m_previewerWidget);
        m_previewerWidget->showPreview(packagePath);
        m_previewerWidget->setVisible(showPreview);

        //now do the relative stuff for the konsole
        m_konsoleWidget->populateKonsole();
        m_konsoleWidget->setObjectName("Previewer Output");
        connect(m_previewerWidget, SIGNAL(showKonsole()), this, SLOT(showKonsolePreviewer()));
        addDockWidget(Qt::BottomDockWidgetArea, m_konsoleWidget);
    }

    restoreState(state, STATE_VERSION);
    toolBar()->show();

    // Now, setup some useful properties such as the project name in the title bar
    // and setting the current working directory.

    //connect(m_metaEditor, SIGNAL(apiChanged()), SLOT(checkProjectrc()));
    kDebug() << "loading metadata:" << packagePath + "metadata.desktop";
    checkMetafile(packagePath);
    KConfig metafile(packagePath + "metadata.desktop");
    KConfigGroup meta(&metafile, "Desktop Entry");
    m_currentProject = meta.readEntry("Name", path);
    setCaption(m_currentProject);
    kDebug() << "Content prefix: " << m_model->contentsPrefix() ;
    QDir::setCurrent(m_model->package() + m_model->contentsPrefix());

    // load mainscript
    QString mainScript = meta.readEntry("X-Plasma-MainScript", QString());
    kDebug() << "read mainScript" << mainScript;
    if (!mainScript.isEmpty()) {
        KUrl url = KUrl(packagePath + "contents/" + mainScript);
        m_editPage->loadFile(url);
    }
    // After we loaded the project, init the TimeLine and Previewer component
    menuBar()->show();
    if (m_timeLine) {
        m_timeLine->loadTimeLine(m_model->package());
    }

    if (m_filelist) {
        m_filelist->show();
    }

    updateActions();
}

void MainWindow::checkMetafile(const QString &path)
{
    KUrl projectPath(path);
    QDir dir(projectPath.path());

    if (!dir.exists(PROJECTRC)) {
        kDebug() << dir.filePath(PROJECTRC)+ " file doesn't exist, metadata.desktop cannot be checked";
        return;
    }
    KConfig preferencesPath(dir.path() +'/'+ PROJECTRC);
    KConfigGroup preferences(&preferencesPath, "ProjectDefaultPreferences");
    QString api;
    if (preferences.readEntry("radioButtonJsChecked", false)) {
        api.append("javascript");
    } else if (preferences.readEntry("radioButtonPyChecked", false)) {
        api.append("python");
    } else if (preferences.readEntry("radioButtonRbChecked", false)) {
        api.append("ruby-script");
    } else if (preferences.readEntry("radioButtonDeChecked", true)) {
        api.append("declarativeappletscript");
    }

    KConfig metafile(path + "metadata.desktop");
    KConfigGroup meta(&metafile, "Desktop Entry");
    meta.writeEntry("X-Plasma-API", api);
    meta.sync();
}

void MainWindow::checkProjectrc()
{
    KUrl path(m_metaEditor->filename());
    path.cd("../..");
    QDir dir(path.path());
    qDebug() << path.path();
    if(!dir.exists(PROJECTRC)) {
        kDebug() << dir.filePath(PROJECTRC)+ " file doesn't exist," << PROJECTRC <<  "cannot be checked";
        return;
    }
    KConfig preferencesPath(dir.path() +'/'+ PROJECTRC);
    KConfigGroup preferences(&preferencesPath, "ProjectDefaultPreferences");
    QString api;
    KConfig metafile(m_metaEditor->filename());
    KConfigGroup meta(&metafile, "Desktop Entry");
    api = meta.readEntry("X-Plasma-API");
    if (api == QString("javascript")) {
        preferences.writeEntry("radioButtonJsChecked", true);
    } else if (api == QString("python")) {
        preferences.writeEntry("radioButtonPyChecked", true);
    } else if (api == QString("ruby-script")) {
          preferences.writeEntry("radioButtonRbChecked", true);
    } else if (api == QString("declarativeappletscript")) {
          preferences.writeEntry("radioButtonDeChecked", true);
    }
    preferences.sync();
}


QStringList MainWindow::recentProjects()
{
    KConfigGroup cg(KGlobal::config(), "General");
//     kDebug() << l.toStringList();

    return cg.readEntry("recentProjects", QStringList());
}

Previewer* MainWindow::createPreviewerFor(const QString& projectType)
{
    Previewer* ret = 0;
    bool showPreviewAction = true;

    if (projectType.contains("KWin/WindowSwitcher")) {
        ret = new TabBoxPreviewer(i18nc("Window Title", "Window Switcher Previewer"), this);
    } else if (projectType.contains("Plasma/Applet")) {
        ret = new PlasmoidPreviewer(i18n("Preview"), this);
    } else if (projectType == "Plasma/Runner") {
        ret = new RunnerPreviewer(i18n("Previewer"), this);
    } else {
        showPreviewAction = false;

    }

    if (showPreviewAction) {
        //we have a previewer( ret is valid) so
        //show the action in the toolbar
        actionCollection()->action("preview")->setVisible(true);
    }
    else {
        //we don't have a previewer( ret == 0 ) so
        //hide the action in the toolbar
        actionCollection()->action("preview")->setVisible(false);
    }

    if (ret) {
        ret->setObjectName("preview");
        connect(ret, SIGNAL(refreshRequested()), this, SLOT(saveAndRefresh()));
        connect(ret, SIGNAL(visibilityChanged(bool)), this, SLOT(updateActions()));
    }

    return ret;
}

void MainWindow::customMessageHandler(QtMsgType type, const QString& msg)
{
    if (QString(msg).startsWith("plasmate") || //don't include the plasmate specific output
        QString(msg).startsWith("Object::") || // don't include QObject warnings
        QString(msg).startsWith("QGraphicsScene::") || //don't include QGraphicsScene warnings
        QString(msg).startsWith(" X Error")) //don't include silly X errors
    {
       std::cout << msg.toLocal8Bit().data() << std::endl;
    } else {
        QString txt;
        switch (type) {
            case QtDebugMsg:
                txt = QString("Debug: %1").arg(msg);
                break;
            case QtWarningMsg:
                txt = QString("Warning: %1").arg(msg);
                break;
            case QtCriticalMsg:
                txt = QString("Critical: %1").arg(msg);
                break;
            case QtFatalMsg:
                txt = QString("Fatal: %1").arg(msg);
                abort();
        }

        QFile outFile("/var/tmp/plasmatepreviewerlog.txt");

        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << endl;
        outFile.close();

        //populate the konsole
        if (m_konsoleWidget) {
            m_konsoleWidget->populateKonsole();
        }
    }
}
