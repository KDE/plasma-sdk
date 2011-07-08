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

ProjectManager::ProjectManager(QWidget* parent) : QDialog(parent)
{
    projectList = new QListWidget();
    projectList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(projectList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(emitProjectSelected()));
    connect(projectList,SIGNAL(itemSelectionChanged()), this, SLOT(checkButtonState()));

    loadButton = new KPushButton(i18n("Load Project"));
    connect(loadButton, SIGNAL(clicked()), this, SLOT(emitProjectSelected()));

    removeMenuButton = new KPushButton(i18n("Remove Project"));

    removeMenu = new KMenu(i18n("Remove Project"));

    removeMenuButton->setMenu(removeMenu);
    removeMenu->addAction(i18n("From List"), this, SLOT(confirmRemoveFromList()));
    removeMenu->addAction(i18n("From Disk"), this, SLOT(confirmRemoveFromDisk()));

    QHBoxLayout *hoz = new QHBoxLayout();

    loadButton->setDisabled(true);
    hoz->addWidget(loadButton);

    removeMenuButton->setDisabled(true);
    hoz->addWidget(removeMenuButton);

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(projectList);
    lay->addLayout(hoz);
    setLayout(lay);
}

void ProjectManager::confirmRemoveFromList()
{
    m_destroyFlag = false;
    QString dialogText = i18n("Are you sure you want to remove the selected projects? This cannot be undone.");
    int code = KMessageBox::warningContinueCancel(this, dialogText);
    if (code != KMessageBox::Continue) {
        return;
    }
    removeProcess();
}

void ProjectManager::confirmRemoveFromDisk()
{
    m_destroyFlag = true;
    QString dialogText = i18n("Are you sure you want to remove the selected projects? This cannot be undone");
    int code = KMessageBox::warningContinueCancel(this, dialogText);
    if (code != KMessageBox::Continue) {
        return;
    }
    removeProcess();
}

void ProjectManager::removeProcess()
{
    QList<QListWidgetItem*> l = projectList->selectedItems();
    bool checkSuccess = true;

    //TODO: should probably centralize config handling code somewhere.
    KConfigGroup cg(KGlobal::config(), "General");
    QStringList recentProjects = cg.readEntry("recentProjects", QStringList());
    for (int i = 0; i < l.size(); i++) {
        QString folder = l[i]->data(StartPage::FullPathRole).value<QString>();
        QString path = KStandardDirs::locateLocal("appdata", folder + '/');
        if(m_destroyFlag) {
            deleteProject(path);
        }
        if (recentProjects.contains(folder)) {
            recentProjects.removeAt(recentProjects.indexOf(folder));
        } else {
            checkSuccess = false;
        }
    }
    if (checkSuccess) {
        cg.writeEntry("recentProjects", recentProjects);
        KGlobal::config()->sync();
        emit requestRefresh();
    }
}

void ProjectManager::checkButtonState()
{
    QList<QListWidgetItem*> l = projectList->selectedItems();
    if (l.size()==0) {
        loadButton->setDisabled(true);
        removeMenuButton->setDisabled(true);
    } else if (l.size()==1) {
        loadButton->setEnabled(true);
        removeMenuButton->setEnabled(true);
    } else {
        loadButton->setDisabled(true);
        removeMenuButton->setEnabled(true);
    }
}


void ProjectManager::addProject(QListWidgetItem *item)
{
    projectList->addItem(item);
}

void ProjectManager::clearProjects()
{
    projectList->clear();
}

void ProjectManager::emitProjectSelected()
{
    QList<QListWidgetItem*> l = projectList->selectedItems();
    if (l.isEmpty()) {
        return;
    }

    QString url = l[0]->data(StartPage::FullPathRole).value<QString>();

    emit projectSelected(url);
    done(QDialog::Accepted);
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

bool ProjectManager::deleteProject(const KUrl &projectLocation)
{
    removeDirectory(projectLocation.path());
    return true;
}

void ProjectManager::removeDirectory(const QString& path)
{
    QDir d(path);
    QFileInfoList list = d.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden);
    for (int i = 0; i < list.size(); i++) {
        if (list[i].isDir() && list[i].filePath() != path) {
            removeDirectory(list[i].filePath());
        }
        d.remove(list[i].fileName());
    }
    d.rmpath(path);
}
