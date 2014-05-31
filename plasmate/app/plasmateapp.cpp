/*
    Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the
    Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "plasmateapp.h"
#include "plasmateextention.h"
#include "startpage/startpage.h"


#include <shell/core.h>
#include <shell/documentcontroller.h>
#include <shell/projectcontroller.h>
#include <shell/uicontroller.h>


#include <sublime/mainwindow.h>


#include <interfaces/idocumentcontroller.h>
#include <interfaces/iprojectcontroller.h>

#include <QMenuBar>
#include <QDockWidget>


#include <KLocalizedString>
#include <KMessageBox>
#include <KConfigGroup>
#include <KSplashScreen>
#include <KToolBar>

#include <QDebug>

PlasmateApp::PlasmateApp(QObject *parent)
    : QObject(parent)
{
    init();
    setupStartPage(true);
}

PlasmateApp::~PlasmateApp()
{
}

void PlasmateApp::init()
{
    KSplashScreen* splash = 0;

    QString session;
    PlasmateExtension::init();

    KDevelop::Core::initialize(splash, KDevelop::Core::Default, session);

    m_core = KDevelop::Core::self();
    m_uiControllerInternal = m_core->uiControllerInternal();
    m_projectController = m_core->projectController();
    m_documentController = m_core->documentController();

    connect(m_documentController, SIGNAL(documentClosed(KDevelop::IDocument*)),
            this, SLOT(checkStartPage()));

    connect(m_projectController, SIGNAL(projectClosed(KDevelop::IProject*)),
            this, SLOT(checkStartPage()));
}

void PlasmateApp::checkStartPage()
{
    if (m_documentController->openDocuments().isEmpty() &&
        m_projectController->projects().isEmpty()) {
        // we don't have any project and documents so we don't
        // have to close anything :)
        setupStartPage(false);
    }
}

void PlasmateApp::setupStartPage(bool closeProjectsAndDocuments)
{
    if (!m_uiControllerInternal->activeSublimeWindow()) {
        return;
    }

    if (closeProjectsAndDocuments) {
        // when plasmate is about to go to the StartPage  we *don't* want
        // to have any opened documents and projects so lets close them if there are any

        // close all documents
        m_documentController->closeAllDocuments();

        //close all projects
        for (auto project : m_projectController->projects()) {
            m_projectController->closeProject(project);
        }
    }

    StartPage *startPage = new StartPage();

    connect(startPage, &StartPage::projectSelected, [=](const QUrl &projectFile) {
        loadMainWindow(projectFile);
    });

    m_uiControllerInternal->activeSublimeWindow()->setBackgroundCentralWidget(startPage);

    QList<QToolBar*> toolBars = m_uiControllerInternal->activeSublimeWindow()->findChildren<QToolBar*>();

    if (!m_toolbarActions.isEmpty()) {
        m_toolbarActions.clear();
    }

    for (auto toolBar : toolBars) {
        m_toolbarActions.append(toolBar->actions());
    }

    showKDevUi(false);
}

void PlasmateApp::loadMainWindow(const QUrl &projectPath)
{
    const QString metadataFilePath = projectPath.path() + QStringLiteral("/metadata.desktop");

    if (!QFile(metadataFilePath).exists()) {
        const char *error = "Your package is invalid";
        KMessageBox::error(m_uiControllerInternal->activeMainWindow(), i18n(error));
        return;
    }

    const QString projectName = QDir(projectPath.path()).dirName();
    const QString projectPlasmateFile = projectPath.path() + QLatin1Char('/') + projectName + QLatin1Char('.') +
                                        PlasmateExtension::getInstance()->projectFileExtension();

    if (!QFile(projectPlasmateFile).exists()) {
        KConfigGroup configPlasmate(KSharedConfig::openConfig(projectPlasmateFile), QStringLiteral("Project"));
        configPlasmate.writeEntry("Name", projectName);
        configPlasmate.writeEntry("Manager", "KDevGenericManager");
        configPlasmate.sync();
    }

    showKDevUi(true);

    m_documentController->openDocument(metadataFilePath);
    m_projectController->openProject(QUrl::fromLocalFile(projectPlasmateFile));
}

void PlasmateApp::showKDevUi(bool visible)
{
    m_uiControllerInternal->activeMainWindow()->menuBar()->setVisible(visible);

    for (auto action : m_toolbarActions) {
        action->setVisible(visible);
        if (action->isChecked()) {
            action->setChecked(false);
        }
    }

    for (auto obj : m_uiControllerInternal->activeSublimeWindow()->children()) {
        QDockWidget *dockWidget = qobject_cast<QDockWidget *>(obj);
        if (dockWidget && dockWidget->isVisible()) {
            dockWidget->close();
        }
    }
}

