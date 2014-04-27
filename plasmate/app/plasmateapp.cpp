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
#include "../startpage.h"


#include <shell/core.h>
#include <shell/documentcontroller.h>
#include <shell/projectcontroller.h>
#include <shell/uicontroller.h>


#include <sublime/mainwindow.h>


#include <interfaces/idocumentcontroller.h>
#include <interfaces/iprojectcontroller.h>

#include <QMenuBar>
#include <QDockWidget>


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


    connect(KDevelop::Core::self()->documentController(),
            SIGNAL(documentClosed(KDevelop::IDocument*)),
            this, SLOT(checkStartPage()));

    connect(KDevelop::Core::self()->projectController(),
            SIGNAL(projectClosed(KDevelop::IProject*)),
            this, SLOT(checkStartPage()));
}

void PlasmateApp::checkStartPage()
{
    KDevelop::Core *core = KDevelop::Core::self();
    if (core->documentController()->openDocuments().isEmpty() &&
        core->projectController()->projects().isEmpty()) {
        // we don't have any project and documents so we don't
        // have to close anything :)
        setupStartPage(false);
    }
}

void PlasmateApp::setupStartPage(bool closeProjectsAndDocuments)
{
    KDevelop::UiController *uiControllerInternal = KDevelop::Core::self()->uiControllerInternal();
    if (!uiControllerInternal->activeSublimeWindow()) {
        return;
    }

    if (closeProjectsAndDocuments) {
        // when plasmate is about to go to the StartPage  we *don't* want
        // to have any opened documents and projects so lets close them if there are any

        // close all documents
        KDevelop::Core::self()->documentController()->closeAllDocuments();

        //close all projects
        for (auto project : KDevelop::Core::self()->projectController()->projects()) {
            KDevelop::Core::self()->projectController()->closeProject(project);
        }
    }

    StartPage *startPage = new StartPage();

    connect(startPage, &StartPage::projectSelected, [=](const QUrl &projectFile) {
        loadMainWindow(projectFile);
    });

    uiControllerInternal->activeSublimeWindow()->setBackgroundCentralWidget(startPage);

    QList<QToolBar *> toolBars = uiControllerInternal->activeSublimeWindow()->findChildren<QToolBar *>();
    for (auto toolBar : toolBars) {
        m_toolbarActions.append(toolBar->actions());
    }

    showKDevUi(false);
}

void PlasmateApp::loadMainWindow(const QUrl &projectFile)
{
    showKDevUi(true);

    KDevelop::ICore::self()->documentController()->openDocument(projectFile);
}

void PlasmateApp::showKDevUi(bool visible)
{
    KDevelop::UiController *uiControllerInternal = KDevelop::Core::self()->uiControllerInternal();

    uiControllerInternal->activeMainWindow()->menuBar()->setVisible(visible);

    for (auto action : m_toolbarActions) {
        action->setVisible(visible);
        if (action->isChecked()) {
            action->setChecked(false);
        }
    }

    for (auto obj : uiControllerInternal->activeSublimeWindow()->children()) {
        QDockWidget *dockWidget = qobject_cast<QDockWidget *>(obj);
        if (dockWidget && dockWidget->isVisible()) {
            dockWidget->close();
        }
    }
}

