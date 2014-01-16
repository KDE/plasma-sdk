/*
 *   Copyright 2009 Lim Yuen Hoe <yuenhoe@hotmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDir>

#include <KDebug>
#include <KConfig>
#include <KIO/DeleteJob>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMenu>
#include <KMessageBox>
#include <KPushButton>
#include <KStandardDirs>
#include <KUrl>
#include <KZip>

#include "projectmanager.h"
#include "startpage.h"

ProjectManager::ProjectManager(QWidget* parent)
    : KDialog(parent)
{
    setButtons(KDialog::None);
    m_projectList = new QListWidget(this);
    m_projectList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(m_projectList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(emitProjectSelected()));
    connect(m_projectList,SIGNAL(itemSelectionChanged()), this, SLOT(checkButtonState()));

    m_loadButton = new KPushButton(i18n("Load Project"), this);
    connect(m_loadButton, SIGNAL(clicked()), this, SLOT(emitProjectSelected()));

    m_removeMenuButton = new KPushButton(i18n("Remove Project"), this);

    m_removeMenu = new KMenu(i18n("Remove Project"), this);

    m_removeMenuButton->setMenu(m_removeMenu);
    m_removeMenu->addAction(i18n("From List"), this, SLOT(confirmRemoveFromList()));
    m_removeMenu->addAction(i18n("From Disk"), this, SLOT(confirmRemoveFromDisk()));
    m_removeMenu->addAction(i18n("Cache"), this, SLOT(confirmRemoveProjectCache()));

    QVBoxLayout *l = new QVBoxLayout();

    m_loadButton->setDisabled(true);

    m_removeMenuButton->setDisabled(true);
    l->addWidget(m_projectList);
    l->addWidget(m_loadButton);
    l->addWidget(m_removeMenuButton);

    QWidget *tmpWidget = new QWidget();
    tmpWidget->setLayout(l);
    setMainWidget(tmpWidget);
}


void ProjectManager::confirmRemoveFromList()
{
    const int count = m_projectList->selectedItems().count();
    QString dialogText = i18np("Are you sure you want to remove the selected project from the project list?",
                               "Are you sure you want to remove the selected projects from the project list?",
                               count);
    const int code = KMessageBox::warningContinueCancel(this, dialogText);
    if (code == KMessageBox::Continue) {
        removeSelectedProjects(false);
    }
}

void ProjectManager::confirmRemoveFromDisk()
{
    const int count = m_projectList->selectedItems().count();
    QString dialogText = i18np("Are you sure you want to remove the selected project from disk? This cannot be undone",
                               "Are you sure you want to remove the selected projects from disk? This cannot be undone",
                               count);
    const int code = KMessageBox::warningContinueCancel(this, dialogText);
    if (code == KMessageBox::Continue) {
        removeSelectedProjects(true);
    }
}

void ProjectManager::confirmRemoveProjectCache()
{
    const QList<QListWidgetItem *> items = m_projectList->selectedItems();
    foreach (const QListWidgetItem *item, items) {
        QString path = item->data(StartPage::FullPathRole).value<QString>();
        if (!path.endsWith(QLatin1Char('/'))) {
            path += QLatin1Char('/');
        }
        path += PROJECTRC;
        if (!QFile::remove(path)) {
            KMessageBox::error(this, QLatin1String("Plasmate was unable to remove the project's cache"));
        }
    }
}

void ProjectManager::removeSelectedProjects(bool deleteFromDisk)
{
    const QList<QListWidgetItem *> items = m_projectList->selectedItems();
    bool checkSuccess = false;
    QStringList recentList = m_mainWindow->recentProjects();

    foreach (const QListWidgetItem *item, items) {
        QString recentProject = item->data(StartPage::FullPathRole).value<QString>();
        recentProject.append("/");
        if (recentProject.contains(KStandardDirs::locateLocal("appdata",""))
            && !recentProject.isEmpty() && deleteFromDisk) {
            deleteProject(recentProject);
        } else if (recentProject.endsWith("package/")) {
            deleteProject(recentProject);
        }

        if (recentList.removeAll(item->data(StartPage::FullPathRole).value<QString>()) > 0) {
            checkSuccess = true;
        }
    }
    if (checkSuccess) {
        setRecentProjects(recentList);
    }
}

void ProjectManager::checkButtonState()
{
    QList<QListWidgetItem *> l = m_projectList->selectedItems();
    m_loadButton->setEnabled(l.count() == 1);
    m_removeMenuButton->setEnabled(l.count() > 0);
}

void ProjectManager::addProject(QListWidgetItem *item)
{
    m_projectList->addItem(item);
}

void ProjectManager::clearProjects()
{
    m_projectList->clear();
}

void ProjectManager::emitProjectSelected()
{
    QList<QListWidgetItem *> l = m_projectList->selectedItems();
    if (l.isEmpty()) {
        return;
    }

    QString url = l[0]->data(StartPage::FullPathRole).value<QString>();

    emit projectSelected(url);
    done(KDialog::Accepted);
}

bool ProjectManager::exportPackage(const KUrl &toExport, const KUrl &targetFile)
{
    // Think ONE minute before committing nonsense: if you want to zip a folder,
    // and you create the *.zip file INSIDE that folder WHILE copying the files,
    // guess what happens??
    // This also means: always try at least once, before committing changes.
    if (targetFile.pathOrUrl().contains(toExport.pathOrUrl())) {
        // Sounds like we are attempting to create the package from inside the package folder, noooooo :)
        return false;
    }

    if (QFile::exists(targetFile.path())) {
        //TODO: Make sure this succeeds
        QFile::remove(targetFile.path()); // overwrite!
    }

    // Create an empty zip file
    KZip zip(targetFile.pathOrUrl());
    zip.open(QIODevice::ReadWrite);
    zip.close();

    // Reopen for writing
    if (zip.open(QIODevice::ReadWrite)) {
        kDebug() << "zip file opened successfully";
        zip.addLocalDirectory(toExport.pathOrUrl(), ".");
        zip.close();
        return true;
    }

    kDebug() << "Cant open zip file" ;
    return false;
}

bool ProjectManager::importPackage(const KUrl &toImport, const KUrl &targetLocation)
{
    bool ret = true;
    KZip plasmoid(toImport.path());
    if (!plasmoid.open(QIODevice::ReadOnly)) {
        kDebug() << "ProjectManager::importPackage can't open the plasmoid archive";
        return false;
    }
    plasmoid.directory()->copyTo(targetLocation.path());
    plasmoid.close();
    return ret;
}

void ProjectManager::addRecentProject(const QString &path)
{
    QStringList recent = m_mainWindow->recentProjects();
    recent.removeAll(path);
    recent.prepend(path);
    //kDebug() << "Writing the following recent files to the config:" << recent;

    KConfigGroup cg(KGlobal::config(), "General");
    cg.writeEntry("recentProjects", recent);
    KGlobal::config()->sync();
    emit requestRefresh();
}

void ProjectManager::setRecentProjects(const QStringList &paths)
{
    KConfigGroup cg(KGlobal::config(), "General");
    cg.writeEntry("recentProjects", paths);
    KGlobal::config()->sync();
    emit requestRefresh();
}

void ProjectManager::deleteProject(const KUrl &projectLocation)
{
    KUrl project = projectLocation;
    KIO::del(project);
}
