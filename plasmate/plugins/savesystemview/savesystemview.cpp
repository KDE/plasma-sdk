/*
    Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "savesystemview.h"
#include "savesystemviewplugin.h"
#include "git.h"

#include <interfaces/icore.h>
#include <interfaces/iproject.h>
#include <interfaces/iprojectcontroller.h>

#include <project/projectmodel.h>

#include <KActionCollection>
#include <KLocalizedString>

SaveSystemView::SaveSystemView(SaveSystemViewPlugin *plugin, QWidget *parent)
    : QWidget(parent),
      m_git(nullptr)
{
    connect(KDevelop::ICore::self()->projectController(), SIGNAL(projectOpened(KDevelop::IProject*)),
            this, SLOT(projectOpened(KDevelop::IProject*)));
}

SaveSystemView::~SaveSystemView()
{
}

void SaveSystemView::projectOpened(KDevelop::IProject *project)
{
    if (m_git) {
        delete m_git;
        m_git = nullptr;
    }

    m_git = new Git(project, this);

    if (!m_git->initGit()) {
        return;
    }

    m_git->initializeRepository();
}

#include "savesystemview.moc"

