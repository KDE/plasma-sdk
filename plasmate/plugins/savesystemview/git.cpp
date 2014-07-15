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

#include "git.h"
#include "commitsmodel.h"

#include <interfaces/icore.h>
#include <interfaces/iproject.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/contextmenuextension.h>
#include <interfaces/context.h>

#include <project/projectmodel.h>

#include <util/path.h>

#include <vcs/interfaces/ibasicversioncontrol.h>
#include <vcs/interfaces/ibranchingversioncontrol.h>
#include <vcs/interfaces/idistributedversioncontrol.h>
#include <vcs/vcsjob.h>
#include <vcs/dvcs/dvcsjob.h>

#include <KLocalizedString>
#include <KMessageBox>

#include <QAction>
#include <QMenu>
#include <QDirIterator>
#include <QDebug>

Git::Git(QObject *parent)
    : QObject(parent),
      m_project(nullptr),
      m_dvcs(nullptr),
      m_branching(nullptr)
{
    m_commitsModel = new CommitsModel(this);
    connect(this, &Git::commitsModelChanged, m_commitsModel, &CommitsModel::resetModel);
}

Git::~Git()
{
}

void Git::setProject(KDevelop::IProject *project)
{
    m_project = project;
    m_repositoryPath = m_project->path().toUrl();
}

 KDevelop::IProject *Git::project() const
{
    return m_project;
}

bool Git::initGit()
{
    if (!m_project) {
        return false;
    }

    KDevelop::IPlugin* plugin = nullptr;

    if (!m_project->versionControlPlugin()) {
        plugin = KDevelop::ICore::self()->pluginController()->loadPlugin(QStringLiteral("kdevgit"));
    } else {
        plugin = m_project->versionControlPlugin();
    }

    if (!plugin) {
        // the git plugin doesn't exist
        // there is nothing more for us to do then
        return false;
    }

    m_dvcs = plugin->extension<KDevelop::IDistributedVersionControl>();
    m_branching = plugin->extension<KDevelop::IBranchingVersionControl>();

    if (!m_dvcs || !m_branching) {
        // something went wrong
         return false;
    }

    return true;
}

bool Git::isRepository()
{
    bool ok = m_dvcs->isVersionControlled(m_repositoryPath);
    if (ok) {
        // reset the model
        emit commitsModelChanged();
    }

    return ok;
}

bool Git::initializeRepository()
{
    if (!m_dvcs) {
        return false;
    }

    KDevelop::VcsJob *job = m_dvcs->init(m_repositoryPath);
    if (!handleJob(job)) {
        return false;
    }

    return newSavePoint(QStringLiteral("Initial Commit"));
}

QStringList Git::branches()
{
    QStringList l;

    KDevelop::VcsJob *job = m_branching->branches(m_repositoryPath);
    if (!handleJob(job)) {
        return l;
    }

    l = job->fetchResults().toStringList();

    return l;
}

QString Git::currentBranch()
{
    QString branch;

    KDevelop::VcsJob *job = m_branching->currentBranch(m_repositoryPath);
    if (!handleJob(job)) {
        return branch;
    }

    branch = job->fetchResults().toString();

    return branch;
}

bool Git::createBranch(const QString &branchName)
{
    KDevelop::VcsJob *job = m_branching->branch(m_repositoryPath, KDevelop::VcsRevision(), branchName);
    if (!handleJob(job)) {
        return false;
    }

    emit commitsModelChanged();
    return true;
}

bool Git::switchBranch(const QString &branchName)
{
    KDevelop::VcsJob *job = m_branching->switchBranch(m_repositoryPath, branchName);
    if (!handleJob(job)) {
        return false;
    }

    emit commitsModelChanged();
    return true;
}

bool Git::deleteBranch(const QString &branchName)
{
    KDevelop::VcsJob *job = m_branching->deleteBranch(m_repositoryPath, branchName);
    if (!handleJob(job)) {
        return false;
    }

    emit commitsModelChanged();
    return true;
}

bool Git::renameBranch(const QString &oldName, const QString &newName)
{
    KDevelop::VcsJob *job = m_branching->renameBranch(m_repositoryPath, oldName, newName);
    if (!handleJob(job)) {
        return false;
    }

    emit commitsModelChanged();
    return true;
}

QList<KDevelop::VcsEvent> Git::commits()
{
    QList<KDevelop::VcsEvent> vcsEventList;

    KDevelop::VcsJob *job = m_dvcs->log(m_repositoryPath);
    if (!handleJob(job)) {
        return vcsEventList;
    }

    QList<QVariant> l = job->fetchResults().toList();

    for (auto it : l) {
        KDevelop::VcsEvent vcsEvent = it.value<KDevelop::VcsEvent>();
        vcsEventList << vcsEvent;
    }

    return vcsEventList;
}

bool Git::handleJob(KDevelop::VcsJob *job)
{
    if (!job) {
        return false;
    }

    bool success = true;

    if (!job->exec()) {
        if (job->status() != KDevelop::VcsJob::JobSucceeded) {
            KMessageBox::error(0, i18n(qPrintable(job->errorString())));
            qDebug() << "Job output" << qobject_cast<KDevelop::DVcsJob*>(job)->directory() <<  qobject_cast<KDevelop::DVcsJob*>(job)->output() << job;
            qDebug() << "Job status" << job->status();

            success = false;
        }
    }

    job->deleteLater();
    return success;
}

QAbstractItemModel* Git::commitsModel() const
{
    return m_commitsModel;
}


bool Git::newSavePoint(const QString &commitMessage, bool saveDocuments)
{
    if (saveDocuments && !KDevelop::ICore::self()->documentController()->saveAllDocuments()) {
        return false;
    }

    QList<QUrl> filesAndDirs;
    QDirIterator it(m_repositoryPath.toLocalFile(), QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString entry = it.next();
        const QString excludedFile = m_project->name() + ".plasmate";
        if (it.fileInfo().isFile() && it.fileName() != excludedFile) {
            filesAndDirs << QUrl::fromLocalFile(it.filePath());
        }
    }

    KDevelop::VcsJob *job = m_dvcs->add(filesAndDirs, KDevelop::IBasicVersionControl::Recursive);

    if (!handleJob(job)) {
        return false;
    }

    job = m_dvcs->commit(commitMessage, filesAndDirs, KDevelop::IBasicVersionControl::Recursive);

    if (!handleJob(job)) {
        return false;
    }

    emit commitsModelChanged();
    return true;
}


#include "git.moc"

