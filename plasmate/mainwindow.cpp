/*
Copyright 2009 Riccardo Iaconelli <riccardo@kde.org>
Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

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

#include <KActionCollection>
#include <KParts/ReadWritePart>
#include <KConfigGroup>
#include <KEditToolBar>
#include <KLocalizedString>
#include <KStandardAction>
#include <KServiceTypeTrader>
#include <KToolBar>
#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/View>
#include <KXMLGUIFactory>
#include <KMessageBox>
#include <QAction>
#include <QMenuBar>

#include "mainwindow.h"
#include "dockwidgetshandler.h"
#include "packagehandler.h"
#include "startpage.h"

static const int STATE_VERSION = 0;

MainWindow::MainWindow(QWidget *parent)
      : KParts::MainWindow(parent, Qt::Widget),
        m_part(nullptr),
        m_doc(nullptr),
        m_view(nullptr),
        m_dockWidgetsHandler(nullptr),
        m_packageHandler(new PackageHandler(this)),
        m_startPage(nullptr)

{
    m_dockWidgetsHandler = new DockWidgetsHandler(m_packageHandler, this);

    KTextEditor::Editor::instance()->readConfig();

    m_doc = KTextEditor::Editor::instance()->createDocument(nullptr);

    m_view = m_doc->createView(this);
    setupActions();
    m_view->setStatusBarEnabled(true);

    setXMLFile(QStringLiteral("plasmateui.rc"));
    createShellGUI(true);
    guiFactory()->addClient(m_view);
    setAutoSaveSettings();

    menuBar()->addMenu(helpMenu());

    setupStartPage();
}

MainWindow::~MainWindow()
{
    guiFactory()->removeClient(m_view);
}

QAction *MainWindow::addAction(QString text, const char * icon, const  char *slot, const char *name)
{
    QAction *action = new QAction(this);
    action->setText(text);
    action->setIcon(QIcon::fromTheme(icon));
    connect(action, SIGNAL(triggered(bool)), m_dockWidgetsHandler, slot);
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
    QAction *installProjectAction = addAction(i18n("Install Project"), "plasmagik", SLOT(installPackage()), "installproject");
    installProjectAction->setShortcut(Qt::META + Qt::Key_F5);

    addAction(i18n("Create Save Point"), "document-save", SLOT(selectSavePoint()), "savepoint");
    addAction(i18n("Publish"), "krfb", SLOT(selectPublish()),   "publish");
    addAction(i18n("Preview"), "user-desktop", SLOT(togglePreviewer()), "preview")->setCheckable(true);
    addAction(i18n("Notes"), "accessories-text-editor", SLOT(toggleNotes()), "notes")->setCheckable(true);
    addAction(i18n("Files"), "system-file-manager", SLOT(toggleFileList()), "file_list")->setCheckable(true);
    addAction(i18n("Timeline"), "process-working",  SLOT(toggleTimeLine()), "timeline")->setCheckable(true);
    addAction(i18n("Documentation"), "help-contents", SLOT(toggleDocumentation()), "documentation")->setCheckable(true);

    KStandardAction::configureToolbars(this, SLOT(editToolbars()), actionCollection());
}

void MainWindow::editToolbars()
{
    KConfigGroup cfg = KSharedConfig::openConfig()->group("MainWindow");
    saveMainWindowSettings(cfg);
    KEditToolBar dlg(guiFactory(), this);

    connect(&dlg, SIGNAL(newToolBarConfig()), this, SLOT(slotNewToolbarConfig()));
    dlg.exec();
}

void MainWindow::slotNewToolbarConfig()
{
    applyMainWindowSettings(KSharedConfig::openConfig()->group("MainWindow"));
}

void MainWindow::setupStartPage()
{
    m_startPage = new StartPage(m_packageHandler, this);
    toolBar()->hide();
    menuBar()->hide();
    m_view->hide();
    setCentralWidget(m_startPage);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveProjectState();

    KParts::MainWindow::closeEvent(event);
    qApp->exit();
}

void MainWindow::saveProjectState()
{
    const QString projectrc = m_packageHandler->packagePath() + QStringLiteral(".plasmateprojectrc");
    KConfigGroup configDocks(KSharedConfig::openConfig(projectrc), "DocksPosition");
    configDocks.writeEntry("MainWindowLayout", saveState(STATE_VERSION));
    configDocks.sync();

    m_dockWidgetsHandler->saveDockWidgetsState();
    m_view->document()->documentSave();
}

void MainWindow::loadProject(const QString &projectPath)
{
    toolBar()->show();
    menuBar()->show();
    m_view->show();

    const QString projectrc = m_packageHandler->packagePath() + QStringLiteral(".plasmateprojectrc");
    KConfigGroup configDocks(KSharedConfig::openConfig(projectrc), "DocksPosition");
    const QString state = configDocks.readEntry("MainWindowLayout", QString());
    restoreState(state.toLocal8Bit(), STATE_VERSION);

    m_view->document()->openUrl(QUrl::fromLocalFile(projectPath));
    setCentralWidget(m_view);
}

