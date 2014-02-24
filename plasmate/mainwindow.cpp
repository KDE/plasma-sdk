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

#include <QAction>
#include <QDir>
#include <QDockWidget>
#include <QListWidgetItem>
#include <QModelIndex>
#include <QLabel>
#include <QGridLayout>
#include <QTextStream>

#include <KTextEdit>

#include <KActionCollection>
#include <KConfig>
#include <KConfigGroup>
#include <QDebug>
#include <KDesktopFile>
#include <QMenu>
#include <QMenuBar>
#include <KMimeTypeTrader>
#include <KStandardAction>
#include <KTextEditor/ConfigInterface>
#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KTextEditor/Editor>
#include <KTextEditor/CommandInterface>
#include <KToolBar>
#include <KStandardAction>
#include <KLocalizedString>
#include <QUrl>
#include <KParts/Part>
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

#pragma message("TODO: enable once they get ported")
#if 0
    #include "startpage.h"
    #include "konsole/konsolepreviewer.h"
    #include "previewer/plasmoid/plasmoidpreviewer.h"
    #include "previewer/runner/runnerpreviewer.h"
    #include "previewer/windowswitcher/tabboxpreviewer.h"
    #include "publisher/publisher.h"
    #include "docbrowser/docbrowser.h"
    #include "modeltest/modeltest.h"
#endif

#if 1
static const QString PROJECTRC(".plasmateprojectrc");
#endif
#pragma message("TODO: remove it once startpage gets ported")
static const int STATE_VERSION = 0;

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

#pragma message("TODO: enable once its gets ported")
MainWindow::MainWindow(QWidget *parent)
      : KParts::MainWindow(parent, Qt::Widget),
        m_sidebar(0),
        #if 0
        m_timeLine(0),
        m_previewerWidget(0),
        m_metaEditor(0),
        m_publisher(0),
        m_browser(0),
        #endif
        m_notesWidget(0),
        m_textEditor(0),
        m_kconfigXtEditor(0),
        m_konsoleWidget(0),
        m_filelist(0),
        m_editPage(0),
        m_imageViewer(0),
        m_model(0),
        m_oldTab(0), // we start from startPage
        m_docksCreated(false),
        m_isPlasmateCreatedPackage(true),
        m_part(0),
        m_notesPart(0)

{
    setXMLFile("plasmateui.rc");
    setupActions();
    createMenus();
    toolBar()->hide();
    menuBar()->hide();
    #if 0
    m_startPage = new StartPage(this);
    connect(m_startPage, SIGNAL(projectSelected(QString)), this, SLOT(loadProject(QString)));
    #endif
    m_central = new CentralContainer(this);
    setCentralWidget(m_central);
    #if 0
    m_central->switchTo(m_startPage);
    #endif
    #pragma message("TODO: enable once it gets ported")
    #pragma message("TODO: we must start from the startpage")
    #if 0
    m_central->switchTo(m_startPage);
    #endif
    setDockOptions(QMainWindow::AllowNestedDocks); // why not?
    if (autoSaveConfigGroup().entryMap().isEmpty()) {
        setWindowState(Qt::WindowMaximized);
    }
}

MainWindow::~MainWindow()
{
    // Saving layout position
    KConfigGroup configDock(KSharedConfig::openConfig(), "DocksPosition");
    configDock.writeEntry("MainWindowLayout", saveState(STATE_VERSION));

    // if the user closes the application with an editor open, should
    // save its contents
    saveEditorData();

    factory()->removeClient(m_part);
    delete m_part;
    #pragma message("FIXME")
    #if 0
    delete m_metaEditor;
    delete m_publisher;
    delete m_editPage;
    delete m_filelist;
    #endif
    #if 0
    if (m_previewerWidget) {
        configDock.writeEntry("PreviewerHeight", m_previewerWidget->height());
        configDock.writeEntry("PreviewerWidth", m_previewerWidget->width());
        delete m_previewerWidget;
    }
    #endif
    #pragma message("Enable once it gets ported")

    #if 0
    if (m_timeLine) {
        configDock.writeEntry("TimeLineLocation", QVariant(m_timeLine->location()));
        delete m_timeLine;
    }
    #endif

    #pragma message("FIXME")
    #if 0
    KGlobal::config()->sync();
    #endif
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveProjectState();
    KParts::MainWindow::closeEvent(event);
    //This is the last widget which will close before plasmate
    //exits. Even when we go back to the startpage, we just change
    //the mainwidget (check the method MainWindow::CentralContaineri::switchTo)
    //so its safe now to exit the application
    qApp->exit();
}

void MainWindow::toggleDocumentation()
{
    #if 0
    showDocumentation(!m_browser || m_browser->isHidden());
    #endif
}

void MainWindow::showDocumentation(bool show)
{
#if 0
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
#endif
#pragma message("TODO: enable once it gets ported")
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

QAction *MainWindow::addAction(QString text, const char * icon, const  char *slot, const char *name, QShortcut *shortcut)
{
    QAction *action = new QAction(this);
    action->setText(text);
    action->setIcon(QIcon::fromTheme(icon));
    if (shortcut) {
        action->setShortcut(shortcut->key());
    }
    connect(action, SIGNAL(triggered(bool)), this, slot);
    actionCollection()->addAction(name, action);
    return action;
}

void MainWindow::setupActions()
{
    QAction *close = KStandardAction::close(this, SLOT(closeProject()), actionCollection());
    close->setText(i18n("Close Project"));

    QAction *quitAction = KStandardAction::quit(this, SLOT(quit()), actionCollection());
    QWidget::addAction(quitAction);

    QAction *refresh = KStandardAction::redisplay(this, SLOT(saveAndRefresh()), actionCollection());
    refresh->setShortcut(Qt::CTRL + Qt::Key_F5);
    refresh->setText(i18n("Refresh Preview"));

    addAction(i18n("Console"), "utilities-terminal", SLOT(toggleKonsolePreviewer()), "konsole")->setCheckable(true);
    #pragma message("FIXME")
    #if 0
    addAction(i18n("Install Project"), "plasmagik", SLOT(installPackage()), "installproject", KShortcut(Qt::META + Qt::Key_I));
    addAction(i18n("Create Save Point"), "document-save", SLOT(selectSavePoint()), "savepoint", KStandardShortcut::save());
    #endif
    addAction(i18n("Publish"), "krfb", SLOT(selectPublish()),   "publish");
    addAction(i18n("Preview"), "user-desktop", SLOT(togglePreview()), "preview")->setCheckable(true);
    addAction(i18n("Notes"), "accessories-text-editor", SLOT(toggleNotes()), "notes")->setCheckable(true);
    addAction(i18n("Files"), "system-file-manager", SLOT(toggleFileList()), "file_list")->setCheckable(true);
    addAction(i18n("Timeline"), "process-working",  SLOT(toggleTimeLine()), "timeline")->setCheckable(true);
    addAction(i18n("Documentation"), "help-contents", SLOT(toggleDocumentation()), "documentation")->setCheckable(true);
}

void MainWindow::updateActions()
{
    #if 0
        actionCollection()->action("preview")->setChecked(m_previewerWidget && m_previewerWidget->isVisible());
    #endif
    #pragma message("TODO: enable once it gets ported")

    actionCollection()->action("notes")->setChecked(m_notesWidget && m_notesWidget->isVisible());
    actionCollection()->action("file_list")->setChecked(m_filelist && m_filelist->isVisible());
    #if 0
    actionCollection()->action("timeline")->setChecked(m_timeLine && m_timeLine->isVisible());
    actionCollection()->action("documentation")->setChecked(m_browser && m_browser->isVisible());
    #endif
    #pragma message("TODO: enable once it gets ported")
}

void MainWindow::toggleActions()
{
    KDesktopFile desktopFile(m_packagePath + "/metadata.desktop");
    QString projectApi = desktopFile.desktopGroup().readEntry("X-Plasma-API", QString());
    if (projectApi == "python" || projectApi == "ruby-script") {
        //Python and ruby bindings doesn't support kDebug,
        //so the konsole previewer cannot retrieve any debug output.
        //Until this issue is being fixed we are hiding the konsole previewer.
        actionCollection()->action("konsole")->setVisible(false);
        //we are hiding the konsole previewer UI.
        #if 0
            m_konsoleWidget->setVisible(false);
        #endif
        #pragma message("TODO: enable once it gets ported")
    }
}

void MainWindow::installPackage()
{
    #if 0
    if (!m_publisher) {
        m_publisher = new Publisher(this, m_model->package(), m_model->packageType());
    }
    #endif
    saveEditorData();

    #if 0
    m_publisher->setProjectName(m_currentProject);
    m_publisher->doPlasmaPkg();
    #endif
    #pragma message("TODO: enable when it gets ported")
}

void MainWindow::closeProject()
{
    saveEditorData();
    saveProjectState();
    toolBar()->hide();
    menuBar()->hide();

    #pragma message("FIXME")
    #if 0
    if (m_timeLine) {
        m_timeLine->hide();
    }
    #endif

    #pragma message("FIXME")
    #if 0
        if (m_previewerWidget) {
            m_previewerWidget->hide();
        }


    if (m_konsoleWidget) {
        m_konsoleWidget->hide();
    }
    #endif

    if (m_notesWidget) {
        m_notesWidget->hide();
    }

    if (m_filelist) {
        m_filelist->hide();
    }

    #if 0
     if (m_browser) {
        m_browser->hide();
    }
    #endif
    #pragma message("TODO: enable once it gets ported")

     setCentralWidget(m_central);

    #pragma message("TODO: enable once it gets ported")
    #if 0
        m_central->switchTo(m_startPage);
    #endif
    setDockOptions(QMainWindow::AllowNestedDocks);
    #if 0
        m_startPage->cancelNewProject();
    #endif
}

void MainWindow::toggleTimeLine()
{
    #pragma message("FIXME")
    #if 0
    if (!m_timeLine) {
        initTimeLine();
    } else {
        m_timeLine->setVisible(!m_timeLine->isVisible());
    }
    #endif
}

void MainWindow::initTimeLine()
{
    #pragma message("FIXME")
    #if 0
    if (!m_timeLine) {
        //FIXME: should come from project specific save data if it exists
        KConfigGroup configDock(KSharedConfig::openConfig(), "DocksPosition");
        Qt::DockWidgetArea location = (Qt::DockWidgetArea)configDock.readEntry("TimeLineLocation", (int)Qt::BottomDockWidgetArea);
        #pragma message("FIXME")
        #if 0
        m_timeLine = new TimeLine(this, m_model->package(), location);
        #endif
        m_timeLine->setObjectName("timeline");
        connect(m_timeLine, SIGNAL(sourceDirectoryChanged()), this, SLOT(editorDestructiveRefresh()));
        connect(m_timeLine, SIGNAL(savePointClicked()), this, SLOT(saveEditorData()));
        connect(m_timeLine, SIGNAL(visibilityChanged(bool)), this, SLOT(updateActions()));
        connect(this, SIGNAL(newSavePointClicked()), m_timeLine, SLOT(newSavePoint()));
        addDockWidget(location, m_timeLine);
    }

    QUrl directory = m_model->package();
    QUrl directory;
    if (QDir(directory.path() + "/contents").exists()) {
            m_timeLine->loadTimeLine(directory);
    }
    #endif
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

        #if 0
        if (m_previewerWidget) {
            splitDockWidget(m_previewerWidget, m_filelist, Qt::Vertical);
        } else {
            addDockWidget(Qt::LeftDockWidgetArea, m_filelist);
        }
        #endif
        #pragma message("TODO: enable once it gets ported")
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
    #pragma message("FIXME")
    #if 0
    if (!m_timeLine) {
        initTimeLine();
    }
    #endif

    saveEditorData();
    emit newSavePointClicked();
}

void MainWindow::selectPublish()
{
    #if 0
    if (!m_publisher) {
        m_publisher = new Publisher(this, m_model->package(), m_model->packageType());
    }
    #endif

    saveEditorData();

    #if 0
    m_publisher->setProjectName(m_currentProject);
    m_publisher->exec();
    #endif
    #pragma message("TODO: enable once it gets ported")
}

void MainWindow::togglePreview()
{
    #if 0
    if (m_previewerWidget) {
        m_previewerWidget->setVisible(!m_previewerWidget->isVisible());
        if (m_previewerWidget->isVisible()) {
            m_previewerWidget->refreshPreview();
        }
    }
    #endif
    #pragma message("TODO: enable once it gets ported")
}

void MainWindow::saveEditorData()
{
    if (qobject_cast<KParts::ReadWritePart*>(m_part)) {
        static_cast<KParts::ReadWritePart*>(m_part)->save();
    }

    if (qobject_cast<KParts::ReadWritePart*>(m_notesPart)) {
        static_cast<KParts::ReadWritePart*>(m_notesPart)->save();
    }

    #pragma message("FIXME")
    #if 0
    if (m_metaEditor) {
        m_metaEditor->writeFile();
        connect(m_metaEditor, SIGNAL(apiChanged()), SLOT(checkProjectrc()));
    }
    #endif
}

void MainWindow::saveAndRefresh()
{
    //in every new save clear the konsole.
    #if 0
    m_konsoleWidget->clearOutput();
    #endif
    #pragma message("TODO: enable once it gets ported")

    saveEditorData();
    #if 0
        if (m_previewerWidget) {
            m_previewerWidget->refreshPreview();
        }
    #endif
    #pragma message("TODO: enable once it gets ported")
}

void MainWindow::editorDestructiveRefresh()
{
    if (qobject_cast<KParts::ReadOnlyPart*>(m_part)) {
        static_cast<KParts::ReadOnlyPart*>(m_part)->openUrl(
            static_cast<KParts::ReadOnlyPart*>(m_part)->url());
    }
    #if 0
    if (m_metaEditor) {
        m_metaEditor->readFile();
    }
    #endif
    #pragma message("TODO: enable once it gets ported")
    if (qobject_cast<KParts::ReadOnlyPart*>(m_notesPart)) {
        static_cast<KParts::ReadOnlyPart*>(m_notesPart)->openUrl(
            static_cast<KParts::ReadOnlyPart*>(m_notesPart)->url());
    }
}

void MainWindow::loadRequiredEditor(const KService::List offers, QUrl target)
{
    // save any previous editor content
    saveEditorData();

    if (offers.isEmpty()) {
        qDebug() << "No offers for editor, can not load.";
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
            //Add the part's GUI
            createGUI(m_part);
        } else {
            // reuse m_part if we can
            delete part;
        }

        if (!m_part) {
            #pragma message("FIXME")
            #if 0
            KMessageBox::error(this, i18n("Failed to load editor for %1:\n\n%2", target.prettyUrl(), error), i18n("Loading Failure"));
            #endif
            return;
        }
    }

    if (!m_part) {
        #if 0
        KMessageBox::error(this, i18n("Failed to load editor for %1", target.prettyUrl()), i18n("Loading Failure"));
        #endif
        return;
    }

    // open the target for editting/viewing if it isn't already viewing that file
    #if 0
    if (!target.equals(m_part->url())) {
        m_part->openUrl(target);
    }
    #endif

    QWidget *mainWidget = m_textEditor ? m_textEditor : m_part->widget();
    m_central->switchTo(mainWidget);
    mainWidget->setMinimumWidth(300);

    // We keep only one editor object alive at a time -
    // so we know who to activate when the edit tab is reselected
    #pragma message("fixme")
    #if 0
    delete m_metaEditor;
    m_metaEditor = 0;
    #endif
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
            qDebug() << "Failed to load notes editor:" << error;
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
    m_notesPart->openUrl(QUrl("file://" + notesFile));
}

QString MainWindow::projectFilePath(const QString &filename)
{
    if (!m_model) {
        return QString();
    }

    #if 0
    QDir packageDir(m_model->package());
    #endif
    #pragma message("TODO: enable once it gets ported")
    QDir packageDir;

    if (m_isPlasmateCreatedPackage) {
        packageDir.cdUp();
    }

    return packageDir.absolutePath() + "/" + filename;
}

void MainWindow::saveProjectState()
{
    qDebug() << m_model << saveState(STATE_VERSION);
    if (!m_model) {
        return;
    }

    const QString projectrc = projectFilePath(PROJECTRC);
    KConfig c(projectrc);
    KConfigGroup configDocks(&c, "DocksPosition");
    configDocks.writeEntry("MainWindowLayout", saveState(STATE_VERSION));
    #if 0
    configDocks.writeEntry("Timeline", m_timeLine && m_timeLine->isVisible());
    configDocks.writeEntry("Documentation", m_browser && m_browser->isVisible());
    #endif
    #pragma message("TODO: enable once it gets ported")
    configDocks.writeEntry("FileList", m_filelist && m_filelist->isVisible());
    configDocks.writeEntry("Notes", m_notesWidget && m_notesWidget->isVisible());
    #if 0
    configDocks.writeEntry("Previewer", m_previewerWidget && m_previewerWidget->isVisible());
    #endif
    #pragma message("TODO: enable once it gets ported")

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

void MainWindow::loadImageViewer(const QUrl& target)
{
    saveEditorData();

    if (!m_imageViewer) {
        m_imageViewer = new ImageViewer(this);
    }

    m_imageViewer->loadImage(target);
    m_central->switchTo(m_imageViewer);
    updateSideBar();
}

void MainWindow::loadKConfigXtEditor(const QUrl& target)
{
    saveEditorData();
    if (!m_kconfigXtEditor) {
        m_kconfigXtEditor = new KConfigXtEditor(this);
    }

    m_kconfigXtEditor->clear();
    m_kconfigXtEditor->setFilename(target);
    m_kconfigXtEditor->readFile();
    m_central->switchTo(m_kconfigXtEditor);

    updateSideBar();
}

void MainWindow::toggleKonsolePreviewer()
{
    #if 0
    if (!m_konsoleWidget) {
        return;
    }

    if(m_konsoleWidget->isVisible()) {
        m_konsoleWidget->setVisible(false);
    } else {
        m_konsoleWidget->setVisible(true);
    }
    #endif
    #pragma message("TODO: enable when konsolepreviewer gets ported")
}

void MainWindow::loadMetaDataEditor(QUrl target)
{
    // save any previous editor content
    saveEditorData();

    #if 0
    if (!m_metaEditor) {
        m_metaEditor = new MetaDataEditor(this);
    }

    m_metaEditor->setFilename(target.path());
    m_metaEditor->readFile();
    m_central->switchTo(m_metaEditor);
    #endif
    #pragma message("TODO: enable once it gets ported")

    updateSideBar();
}

void MainWindow::loadProject(const QString &path)
{
    if (path.isEmpty()) {
        qDebug() << "path is empty?!";
        return;
    }

    QString packagePath;
    QDir pDir(path);
    if (pDir.isRelative()) {
        #pragma message("FIXME")
        #if 0
        packagePath = KStandardDirs::locateLocal("appdata", path + '/');
        #endif
    } else {
        packagePath = path;
    }

    if (!packagePath.endsWith('/')) {
        packagePath.append('/');
    }

    QDir dir(packagePath);
    if (!dir.exists("metadata.desktop")) {
        qDebug() << "no metadata.desktop?!";
        return;
    }

    // if the project rc file is IN the package, then this was loaded from an existing local project
    // otherwise, we assume it was created by plasmate and the project files are up one dir
    m_isPlasmateCreatedPackage = !QFile::exists(packagePath + PROJECTRC);
    m_currentProject = path;
    qDebug() << "Loading project from" << packagePath;
    KService service(packagePath + "metadata.desktop");
    QStringList types = service.serviceTypes();

    if (types.isEmpty()) {
        const QString errorText = i18n("Your metadata.desktop file doesn't contain a X-KDE-ServiceTypes entry,"
                                " your package is invalid");

        KMessageBox::error(this, errorText);
        return;
    }

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

    //take the previewerType and packagePath, we will need it
    //for the actions in the toolbar
    m_packageType = previewerType;
    m_packagePath = packagePath;

    delete m_model;
    #if 0
    m_model = new PackageModel(this);
    m_model = new PackageModel();
    m_model = 0;
    #pragma message("FIXME")
    qDebug() << "Setting project type to:" << actualType;
    m_model->setPackageType(actualType);
    qDebug() << "Setting model package to:" << packagePath;

    if (!m_model->setPackage(packagePath)) {
        KMessageBox::error(this, i18n("Invalid Plasma package."));
        return;
    }
    #endif

    if (!m_editPage) {
        m_editPage = new EditPage();
        connect(m_editPage, SIGNAL(loadEditor(KService::List, QUrl)), this, SLOT(loadRequiredEditor(const KService::List, QUrl)));
        connect(m_editPage, SIGNAL(loadMetaDataEditor(QUrl)), this, SLOT(loadMetaDataEditor(QUrl)));
        connect(m_editPage, SIGNAL(loadImageViewer(QUrl)), this, SLOT(loadImageViewer(QUrl)));
        connect(m_editPage, SIGNAL(loadKConfigXtEditor(QUrl)), this, SLOT(loadKConfigXtEditor(QUrl)));
        m_editPage->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    }

    connect(m_model, SIGNAL(reloadModel()), m_editPage, SLOT(expandAll()));
    m_editPage->setModel(m_model);
    m_editPage->expandAll();

    // prevent accidental loading of previous active project's file
    // plus temporary workaround for editor issue with handling different languages
    delete m_part;
    m_part = 0;

    #if 0
    // delete old publisher
    delete m_publisher;
    m_publisher = 0;
    #endif
    #pragma message("TODO: enable once it gets ported")

    QLabel *l = new QLabel(i18n("Select a file to edit."), this);
    m_central->switchTo(l);

    m_oldTab = EditTab;

    QByteArray state = saveState(STATE_VERSION);
    const QString projectrc = projectFilePath(PROJECTRC);
    bool showPreview = true;
    qDebug() << "******** checking for" << projectrc;
    if (QFile::exists(projectrc)) {
        KConfig c(projectrc);
        KConfigGroup configDocks(&c, "DocksPosition");
        state = configDocks.readEntry("MainWindowLayout", state);

        #if 0
        if (configDocks.readEntry("Timeline", false)) {
            initTimeLine();
        } else {
            delete m_timeLine;
            m_timeLine = 0;
        }
        #endif

        #if 0
        if (configDocks.readEntry("Documentation", false)) {
            showDocumentation(true);
        } else {
            delete m_browser;
            m_browser = 0;
        }
        #endif
        #pragma message("TODO: enable once it gets ported")

        setFileListVisible(configDocks.readEntry("FileList", true));
        setNotesVisible(configDocks.readEntry("Notes", false));
        showPreview = configDocks.readEntry("Previewer", showPreview);
    } else {
        setFileListVisible(true);
    }

    #if 0
    if (m_browser) {
        m_browser->setPackage(m_model);
    }
    #endif
    #pragma message("TODO: enable once it gets ported")

    if (m_notesPart) {
        refreshNotes();
    }

    #if 0
    //initialize the konsole previewer
    m_konsoleWidget = createKonsoleFor(previewerType);

    // initialize previewer
    delete m_previewerWidget;
    m_previewerWidget = createPreviewerFor(previewerType);
    actionCollection()->action("preview")->setEnabled(m_previewerWidget);
    if (m_previewerWidget) {
        addDockWidget(Qt::LeftDockWidgetArea, m_previewerWidget);
        m_previewerWidget->showPreview(packagePath);
        m_previewerWidget->setVisible(showPreview);
        //now do the relative stuff for the konsole
        connect(m_previewerWidget, SIGNAL(showKonsole()), this, SLOT(toggleKonsolePreviewer()));
        addDockWidget(Qt::BottomDockWidgetArea, m_konsoleWidget);
    }
    #endif
    #pragma message("TODO: enable once they get ported")

    restoreState(state, STATE_VERSION);
    toolBar()->show();

    // Now, setup some useful properties such as the project name in the title bar
    // and setting the current working directory.

    //connect(m_metaEditor, SIGNAL(apiChanged()), SLOT(checkProjectrc()));
    qDebug() << "loading metadata:" << packagePath + "metadata.desktop";
    checkMetafile(packagePath);
    KConfig metafile(packagePath + "metadata.desktop");
    KConfigGroup meta(&metafile, "Desktop Entry");
    m_currentProject = meta.readEntry("Name", path);
    setCaption(m_currentProject);

    #if 0
    qDebug() << "Content prefix: " << m_model->contentsPrefix() ;
    QDir::setCurrent(m_model->package() + m_model->contentsPrefix());
    #endif
    #pragma message("TODO: enable once it gets ported")

    // load mainscript
    QString mainScript = meta.readEntry("X-Plasma-MainScript", QString());
    qDebug() << "read mainScript" << mainScript;
    if (!mainScript.isEmpty()) {
        QUrl url = QUrl(packagePath + "contents/" + mainScript);
        m_editPage->loadFile(url);
    }
    // After we loaded the project, init the TimeLine and Previewer component
    menuBar()->show();
    #if 0
    if (m_timeLine) {
        m_timeLine->loadTimeLine(m_model->package());
    }
    #endif

    if (m_filelist) {
        m_filelist->show();
    }

    updateActions();
    toggleActions();
}

void MainWindow::checkMetafile(const QString &path)
{
    QUrl projectPath(path);
    QDir dir(projectPath.path());

    if (!dir.exists(PROJECTRC)) {
        qDebug() << dir.filePath(PROJECTRC)+ " file doesn't exist, metadata.desktop cannot be checked";
        return;
    }
    KConfig preferencesPath(dir.path() +'/'+ PROJECTRC);
    KConfigGroup preferences(&preferencesPath, "ProjectDefaultPreferences");
    QString api;
    const QString radioButtonChecked = preferences.readEntry("radioButtonChecked");
    if (radioButtonChecked.isEmpty()) {
        qDebug() << dir.filePath(PROJECTRC) + "isn't valid, metadata.desktop cannot be checked";
        return;
    }
    if (radioButtonChecked == "Js") {
        api.append("javascript");
    } else if (radioButtonChecked == "Py") {
        api.append("python");
    } else if (radioButtonChecked == "Rb") {
        api.append("ruby-script");
    } else if (radioButtonChecked == "De") {
        api.append("declarativeappletscript");
    }

    KConfig metafile(path + "metadata.desktop");
    KConfigGroup meta(&metafile, "Desktop Entry");
    meta.writeEntry("X-Plasma-API", api);
    meta.sync();
}

void MainWindow::checkProjectrc()
{
    #pragma message("FIXME")
    #if 0
    QUrl path(m_metaEditor->filename());
    #pragma message("FIXME")
    path.cd("../..");
    #endif
    QUrl path;
    QDir dir(path.path());
    qDebug() << path.path();
    if(!dir.exists(PROJECTRC)) {
        qDebug() << dir.filePath(PROJECTRC)+ " file doesn't exist," << PROJECTRC <<  "cannot be checked";
        return;
    }
    KConfig preferencesPath(dir.path() +'/'+ PROJECTRC);
    KConfigGroup preferences(&preferencesPath, "ProjectDefaultPreferences");
    QString api;
    #if 0
    KConfig metafile(m_metaEditor->filename());
    #endif
    #pragma message("TODO: fix it once it gets ported")

    KConfigGroup meta(KSharedConfig::openConfig(QString()), "Desktop Entry");
    api = meta.readEntry("X-Plasma-API");
    if (api == QString("javascript")) {
        preferences.writeEntry("radioButtonChecked", "Js");
    } else if (api == QString("python")) {
        preferences.writeEntry("radioButtonChecked", "Py");
    } else if (api == QString("ruby-script")) {
          preferences.writeEntry("radioButtonChecked", "Rb");
    } else if (api == QString("declarativeappletscript")) {
          preferences.writeEntry("radioButtonChecked", "De");
    }
    preferences.sync();
}


QStringList MainWindow::recentProjects()
{
    KConfigGroup cg(KSharedConfig::openConfig(), "General");
//     qDebug() << l.toStringList();

    return cg.readEntry("recentProjects", QStringList());
}

#if 0
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

KonsolePreviewer* MainWindow::createKonsoleFor(const QString& projectType)
{
    KonsolePreviewer *konsole = 0;
    if (projectType.contains("KWin/WindowSwitcher")) {
        konsole = new KonsolePreviewer(i18nc("Window Title", "Window Switcher Previewer"));
    } else if (projectType.contains("Plasma/Applet")) {
        konsole = new KonsolePreviewer(i18n("Previewer Output"));
    } else if (projectType == "Plasma/Runner") {
        konsole = new KonsolePreviewer(i18n("Previewer Output"));
    }

    if (konsole) {
        konsole->setParent(this);
        konsole->setObjectName("Previewer Output");

        //after the init, cleat the Output
        konsole->clearOutput();
    }

    return konsole;
    return 0;
}
#endif
#pragma message("TODO: enable once konsole gets ported")

