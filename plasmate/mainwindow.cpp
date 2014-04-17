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
#include <KSharedConfig>
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
#include "packagehandler/packagehandler.h"
#include "startpage.h"
#include "publisher/publisher.h"
#include "editors/metadata/metadatahandler.h"

MainWindow::MainWindow(QWidget *parent)
      : KParts::MainWindow(parent, Qt::Widget),
        m_dockWidgetsHandler(new DockWidgetsHandler(this)),
        m_packageHandler(nullptr)
{
    setupActions();

    setXMLFile(QStringLiteral("plasmateui.rc"));
    createShellGUI(true);
    setAutoSaveSettings();

    menuBar()->addMenu(helpMenu());

    setupStartPage();
}

MainWindow::~MainWindow()
{
}

QAction *MainWindow::addAction(const QString &text, const char *icon, const  char *slot, const char *name)
{
    QAction *action = createAction(text, icon, name);
    connect(action, SIGNAL(triggered(bool)), m_dockWidgetsHandler, slot);
    return action;
}

QAction *MainWindow::createAction(QString text, const char * icon, const char *name)
{
    QAction *action = new QAction(this);
    action->setText(text);
    action->setIcon(QIcon::fromTheme(icon));
    actionCollection()->addAction(name, action);
    return action;
}

void MainWindow::setupActions()
{
    QAction *close = KStandardAction::close(this, SLOT(closeProject()), actionCollection());
    close->setText(i18n("Close Project"));

    QAction *quitAction = KStandardAction::quit(this, SLOT(close()), actionCollection());
    QWidget::addAction(quitAction);

    QAction *refresh = KStandardAction::redisplay(this, SLOT(saveAndRefresh()), actionCollection());
    refresh->setShortcut(Qt::CTRL + Qt::Key_F5);
    refresh->setText(i18n("Refresh Preview"));

    addAction(i18n("Console"), "utilities-terminal", SLOT(toggleKonsolePreviewer()), "konsole")->setCheckable(true);
    QAction *installProjectAction = addAction(i18n("Install Project"), "plasmagik", SLOT(installPackage()), "installproject");
    installProjectAction->setShortcut(Qt::META + Qt::Key_F5);

    QAction *publisherAction = createAction(i18n("Publish"), "krfb", "publish");
    connect(publisherAction, SIGNAL(triggered(bool)), this, SLOT(togglePublisher()));

    addAction(i18n("Create Save Point"), "document-save", SLOT(selectSavePoint()), "savepoint");
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

void MainWindow::closeProject()
{
    if (centralWidget()) {
        QWidget *centralWidget = takeCentralWidget();
        KXMLGUIClient *client = dynamic_cast<KXMLGUIClient*>(centralWidget);

        if (client) {
            factory()->removeClient(client);
        }

        centralWidget->deleteLater();
    }

    // we don't need the dockwidgets anymore
    m_dockWidgetsHandler->removeAllDockWidgets();

    setupStartPage();
}

void MainWindow::setupStartPage()
{
    if (!m_startPage) {
        m_startPage = new StartPage(this);
    }

    toolBar()->hide();
    menuBar()->hide();

    if (m_startPage) {
        setCentralWidget(m_startPage.data());
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveProjectState();

    KParts::MainWindow::closeEvent(event);
    qApp->exit();
}

void MainWindow::saveProjectState()
{
    m_dockWidgetsHandler->saveDockWidgetsState();
}

void MainWindow::loadProject()
{

    QWidget *startPage = takeCentralWidget();
    startPage->deleteLater();

    toolBar()->show();
    menuBar()->show();

    m_dockWidgetsHandler->toggleFileList();
}

void MainWindow::togglePublisher()
{
    QScopedPointer<Publisher> publisherWidget;
    MetadataHandler metadataHandler;
    const QString projectName = m_packageHandler->packagePath().split(QLatin1Char('/'), QString::SkipEmptyParts).last();
    metadataHandler.setFilePath(m_packageHandler->packagePath() + QStringLiteral("metadata.desktop"));
    publisherWidget.reset(new Publisher(this, QUrl(m_packageHandler->packagePath()), metadataHandler.serviceTypes().at(0)));
    publisherWidget->setProjectName(projectName);

    //open the dialog
    publisherWidget->exec();
}

void MainWindow::setPackageHandler(PackageHandler *packageHandler)
{
    m_packageHandler = packageHandler;
    m_dockWidgetsHandler->setPackageHandler(m_packageHandler);

    QString mainScriptPath(m_packageHandler->packagePath());

    MetadataHandler metadataHandler;
    metadataHandler.setFilePath(mainScriptPath + QStringLiteral("metadata.desktop"));
    mainScriptPath += m_packageHandler->contentsPrefix() +  metadataHandler.mainScript();
    loadProject();
}

