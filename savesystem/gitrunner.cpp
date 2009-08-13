/******************************************************************************
 * Copyright (C) 2009 by Diego '[Po]lentino' Casella <polentino911@gmail.com> *
 *                                                                            *
 *    This program is free software; you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation; either version 2 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful, but     *
 *    WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 *    General Public License for more details.                                *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program; if not, write to the Free Software Foundation  *
 *    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA        *
 *                                                                            *
 ******************************************************************************/

#include "gitrunner.h"
#include "dvcsjob.h"

#include <QFileInfo>
#include <QDir>
#include <QProcess>

#include <KDebug>
#include <KProcess>


GitRunner::GitRunner()
{
    //m_job = new DvcsJob();
    m_commMode = KProcess::SeparateChannels;
    m_lastRepoRoot = new KUrl();
    m_result = QString();
    m_isRunning = false;
    m_jobStatus = DvcsJob::JobNotStarted;

}

GitRunner::~GitRunner()
{
    /*if( m_job )
        delete m_job;*/
    if (m_lastRepoRoot)
        delete m_lastRepoRoot;
}

void GitRunner::initJob(DvcsJob &job)
{
    job.setCommunicationMode(m_commMode);
    job.setDirectory(QDir(m_lastRepoRoot->pathOrUrl()));
    job << "git";
}

void GitRunner::startJob(DvcsJob &job)
{
    m_result.clear();
    m_isRunning = true;
    job.start();
    m_result.append(job.output());      // Save the result
    m_isRunning = false;
    m_jobStatus = job.status();         // Save job status
    job.cancel();                       // Kill the job
    delete &job;
}

void GitRunner::setCommunicationMode(KProcess::OutputChannelMode comm)
{
    m_commMode = comm;
}

void GitRunner::setDirectory(const KUrl &dir)
{
    m_lastRepoRoot = new KUrl(dir);
}

bool GitRunner::isRunning()
{
    return m_isRunning;
}

bool GitRunner::isValidDirectory()
{
    const QString initialPath(m_lastRepoRoot->toLocalFile(KUrl::RemoveTrailingSlash));
    setDirectory(*m_lastRepoRoot);

    // A possible git repo has a .git subdicerctory
    const QString gitDir(".git");

    // Also, git rev-parse --is-inside-work-tree returns "true" if we are
    // inside any subdirectory of the git tree.
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "rev-parse";
    *job << "--is-inside-work-tree";
    startJob(*job);

    QFileInfo finfo(initialPath);
    QDir dir;
    if (finfo.isFile()) {
        dir = finfo.absoluteDir();
    } else {
        dir = QDir(initialPath);
        dir.makeAbsolute();
    }

    return (dir.exists(gitDir) && m_result.compare("true")) ? true : false;
}

bool GitRunner::hasNewChangesToCommit()
{
    if(status() != DvcsJob::JobSucceeded)
        return false;

    if(m_result.contains("nothing to commit (working directory clean)",
                         Qt::CaseSensitive))
        return false;

    return true;
}

QString& GitRunner::getResult()
{
    return  m_result;
}


DvcsJob::JobStatus GitRunner::init(const KUrl &directory)
{
    // We need to tell the runner to change dir!
    m_lastRepoRoot = new KUrl(directory);
    DvcsJob *job = new DvcsJob();
    initJob(*job);

    *job << "init";
    startJob(*job);
    return m_jobStatus;
}


DvcsJob::JobStatus GitRunner::createWorkingCopy(const KUrl &repoOrigin,
                                                const KUrl &repoDestination)
{
    // TODO: now supports only cloning a local repo(not very useful, I know =P),
    // so extend the method to be used over the Internet.
    m_lastRepoRoot = new KUrl(repoDestination);
    DvcsJob *job = new DvcsJob();
    initJob(*job);

    *job << "clone";
    *job << repoOrigin.pathOrUrl();
    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::add(const KUrl::List &localLocations)
{
    if (localLocations.empty())
        return m_jobStatus = DvcsJob::JobCancelled;

    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "add";

    // Adding files to the runner.
    QStringList stringFiles = localLocations.toStringList();
    while (!stringFiles.isEmpty()) {
        *job <<  m_lastRepoRoot->pathOrUrl() + '/' + stringFiles.takeAt(0);
    }

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::status()
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "status";

    startJob(*job);
    return m_jobStatus;
}


DvcsJob::JobStatus GitRunner::commit(const QString &message)
{
    // NOTE: git doesn't allow empty commit !
    if (message.isEmpty())
        return m_jobStatus = DvcsJob::JobCancelled;

    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "commit";
    *job << "-m";
    //Note: the message is quoted somewhere else
    *job << message;

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::moveToCommit(const QString &sha1hash,
                                           const QString &newBranch)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "checkout";
    *job << sha1hash;

    startJob(*job);
    if (m_jobStatus != DvcsJob::JobSucceeded)
        return m_jobStatus;

    job = new DvcsJob();
    initJob(*job);
    *job << "checkout";
    *job << "-b";
    *job << newBranch;

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::deleteCommit(const QString &sha1hash)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "reset";
    *job << "--hard";
    *job << sha1hash;

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::remove(const KUrl::List &files)
{
    if (files.empty())
        return m_jobStatus = DvcsJob::JobCancelled;

    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "rm";
    QStringList stringFiles = files.toStringList();
    while (!stringFiles.isEmpty()) {
        *job <<  m_lastRepoRoot->pathOrUrl() + '/' + stringFiles.takeAt(0);
    }

    startJob(*job);
    return m_jobStatus;
}


DvcsJob::JobStatus GitRunner::log()
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "log";

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::switchBranch(const QString &newBranch)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "checkout";
    *job << newBranch;

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::mergeBranch(const QString &branchName,
                                          const QString &message)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "merge";
    *job << "--no-ff";
    *job << "-m";
    *job << message;
    *job << branchName;

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::deleteBranch(const QString &branch)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "branch";
    *job << "-D";
    *job << branch;

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::currentBranch()
{
    DvcsJob::JobStatus status = branches();
    if (status != DvcsJob::JobSucceeded)
        return status;
    // Every branch is listed in one line. so first split by lines,
    // then look for the branch marked with a "*".
    QStringList list = m_result.split('\n');
    QString tmp = list.takeFirst();
    while (!tmp.contains('*', Qt::CaseInsensitive))
        tmp = list.takeFirst();

    tmp.remove(0, 2);
    m_result = tmp;
    return status;
}

DvcsJob::JobStatus GitRunner::branches()
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "branch";

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::newBranch(const QString &newBranch)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "branch";
    *job << newBranch;

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::renameBranch(const QString &newBranch)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "branch";
    *job << "-m";
    *job << newBranch;

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::setAuthor(const QString &username)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "config";
    *job << "user.name";
    *job << username;

    startJob(*job);
    return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::setEmail(const QString &email)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "config";
    *job << "user.email";
    *job << email;

    startJob(*job);
    return m_jobStatus;
}
