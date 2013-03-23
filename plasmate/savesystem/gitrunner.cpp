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
    m_isRunning = false;
}

GitRunner::~GitRunner()
{
    /*if( m_job )
        delete m_job;*/
    delete m_lastRepoRoot;
}

void GitRunner::initJob(DvcsJob &job)
{
    job.setCommunicationMode(m_commMode);
    job.setDirectory(QDir(m_lastRepoRoot->pathOrUrl()));
    job << "git";
}

QString GitRunner::startJob(DvcsJob &job, DvcsJob::JobStatus *status)
{
    m_isRunning = true;
    job.start();
    QString output = job.output(); //Return the result
    m_isRunning = false;

    if (status) {
        *status = job.status();    // Save job status
    }

    job.cancel();                  // Kill the job
    delete &job;

    return output;
}

void GitRunner::setCommunicationMode(KProcess::OutputChannelMode comm)
{
    m_commMode = comm;
}

void GitRunner::setDirectory(const KUrl &dir)
{
    m_lastRepoRoot->setDirectory(dir.pathOrUrl());
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
    const QString result = startJob(*job);

    QFileInfo finfo(initialPath);
    QDir dir;
    if (finfo.isFile()) {
        dir = finfo.absoluteDir();
    } else {
        dir = QDir(initialPath);
        dir.makeAbsolute();
    }

    return (dir.exists(gitDir) && result.compare("true")) ? true : false;
}

bool GitRunner::hasNewChangesToCommit()
{
    DvcsJob::JobStatus stat;
    QString result = status(&stat);
    if (stat != DvcsJob::JobSucceeded) {
        return false;
    }

    if (result.contains("nothing to commit (working directory clean)",
    Qt::CaseSensitive)) {
        return false;
    }

    return true;
}

DvcsJob::JobStatus GitRunner::init(const KUrl &directory)
{
    // We need to tell the runner to change dir!
    m_lastRepoRoot->setDirectory(directory.pathOrUrl());
    DvcsJob *job = new DvcsJob();
    initJob(*job);

    DvcsJob::JobStatus status;
    *job << "init";
    startJob(*job, &status);
    return status;
}

void GitRunner::addIgnoredFileExtension(const QString ignoredFileExtension)
{
    QFile gitIgnoreFile(this->m_lastRepoRoot->path() + "/.gitignore");
    if(!gitIgnoreFile.exists()) {
        gitIgnoreFile.open(QIODevice::ReadWrite);
        gitIgnoreFile.write(ignoredFileExtension.toUtf8() + '\n');

    } else {
        gitIgnoreFile.open(QIODevice::Append);
        gitIgnoreFile.write(ignoredFileExtension.toUtf8() + '\n');
    }
    gitIgnoreFile.close();
}

DvcsJob::JobStatus GitRunner::createWorkingCopy(const KUrl &repoOrigin,
                                                const KUrl &repoDestination)
{
    // TODO: now supports only cloning a local repo(not very useful, I know =P),
    // so extend the method to be used over the Internet.
    m_lastRepoRoot->setDirectory(repoDestination.pathOrUrl());
    DvcsJob *job = new DvcsJob();
    initJob(*job);

    *job << "clone";
    *job << repoOrigin.pathOrUrl();

    DvcsJob::JobStatus status;
    startJob(*job, &status);
    return status;
}

DvcsJob::JobStatus GitRunner::add(const KUrl::List &localLocations)
{
    if (localLocations.empty()) {
        return DvcsJob::JobCancelled;
    }

    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "add";

    // Adding files to the runner.
    QStringList stringFiles = localLocations.toStringList();
    while (!stringFiles.isEmpty()) {
        *job <<  m_lastRepoRoot->pathOrUrl() + '/' + stringFiles.takeAt(0);
    }

    DvcsJob::JobStatus status;
    startJob(*job, &status);

    return status;
}

QString GitRunner::status(DvcsJob::JobStatus *status)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "status";

    QString result;
    if (status) {
        result = startJob(*job, status);
    } else {
        result = startJob(*job);
    }

    return result;
}


DvcsJob::JobStatus GitRunner::commit(const QString &message)
{
    // NOTE: git doesn't allow empty commit !
    if (message.isEmpty()) {
        return DvcsJob::JobCancelled;
    }

    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "commit";
    *job << "-m";
    //Note: the message is quoted somewhere else
    *job << message;

    DvcsJob::JobStatus status;
    startJob(*job, &status);
    return status;
}

DvcsJob::JobStatus GitRunner::moveToCommit(const QString &sha1hash,
                                           const QString &newBranch)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "checkout";
    *job << sha1hash;

    DvcsJob::JobStatus status;

    startJob(*job, &status);
    if (status != DvcsJob::JobSucceeded) {
        return status;
    }

    job = new DvcsJob();
    initJob(*job);
    *job << "checkout";
    *job << "-b";
    *job << newBranch;

    startJob(*job, &status);
    return status;
}

DvcsJob::JobStatus GitRunner::deleteCommit(const QString &sha1hash)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "reset";
    *job << "--hard";
    *job << sha1hash;

    DvcsJob::JobStatus status;
    startJob(*job, &status);
    return status;
}

DvcsJob::JobStatus GitRunner::remove(const KUrl::List &files)
{
    if (files.empty()) {
        return DvcsJob::JobCancelled;
    }

    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "rm";
    QStringList stringFiles = files.toStringList();
    while (!stringFiles.isEmpty()) {
        *job <<  m_lastRepoRoot->pathOrUrl() + '/' + stringFiles.takeAt(0);
    }

    DvcsJob::JobStatus status;
    startJob(*job, &status);
    return status;
}


QList<QHash<QString, QString> > GitRunner::log(DvcsJob::JobStatus *status)
{
    QList<QHash<QString, QString> > data;
    QHash<QString, QString> temp;

    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "log";
    //we will use $ in the format in order to
    //split the string author, date, subject
    //and we will use ^^ in order to identify every commit
    *job << "--pretty=format:%an$%s$%ad$%H^^";

    QString result;
    if (status) {
        result = startJob(*job, status);
    } else {
        result = startJob(*job);
    }

    QStringList commitList = result.split("^^", QString::SkipEmptyParts);
    foreach(QString commit, commitList) {
        QStringList l = commit.split('$', QString::SkipEmptyParts);
        temp["author"] = removeNewLines(l.at(0));
        temp["subject"] = removeNewLines(l.at(1));
        temp["date"] = removeNewLines(l.at(2));
        temp["sha1hash"] = removeNewLines(l.at(3));
        data.append(temp);
    }

    return data;
}

QString GitRunner::removeNewLines(const QString &string)
{
    QString tmp = string;
    return tmp.replace("\n", "");
}

DvcsJob::JobStatus GitRunner::switchBranch(const QString &newBranch)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "checkout";
    *job << newBranch;

    DvcsJob::JobStatus status;
    startJob(*job, &status);
    return status;
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

    DvcsJob::JobStatus status;
    startJob(*job, &status);
    return status;
}

DvcsJob::JobStatus GitRunner::deleteBranch(const QString &branch)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "branch";
    *job << "-D";
    *job << branch;

    DvcsJob::JobStatus status;
    startJob(*job, &status);
    return status;
}

QString GitRunner::currentBranch(DvcsJob::JobStatus *status)
{
    if(status) {
        branches(status);

        if (*status != DvcsJob::JobSucceeded) {
            return QString();
        }
    } else {
        branches();
    }

    QString tmp = m_branchesWithAsterisk.takeFirst();
    while (!tmp.contains('*', Qt::CaseInsensitive)) {
        tmp = m_branchesWithAsterisk.takeFirst();
    }

    tmp.remove(0, 2);
    return tmp;
}

QStringList GitRunner::branches(DvcsJob::JobStatus *status)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "branch";

    QString result = startJob(*job, status);

    // Every branch is listed in one line. so first split by lines,
    // then look for the branch marked with a "*".
    m_branchesWithAsterisk.clear();
    m_branchesWithAsterisk = result.split('\n', QString::SkipEmptyParts);

    QStringList list;

    foreach (QString branch, m_branchesWithAsterisk) {
        if (branch.startsWith('*', Qt::CaseInsensitive)) {
            branch.remove(0, 2);
        }
        branch.replace(" ", "");
        list << branch;
    }

    return list;
}

DvcsJob::JobStatus GitRunner::newBranch(const QString &newBranch)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "branch";
    *job << newBranch;

    DvcsJob::JobStatus status;
    startJob(*job, &status);
    return status;
}

DvcsJob::JobStatus GitRunner::renameBranch(const QString &newBranch)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "branch";
    *job << "-m";
    *job << newBranch;

    DvcsJob::JobStatus status;
    startJob(*job, &status);
    return status;
}

DvcsJob::JobStatus GitRunner::setAuthor(const QString &username)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "config";
    *job << "user.name";
    *job << username;

    DvcsJob::JobStatus status;
    startJob(*job, &status);
    return status;
}

DvcsJob::JobStatus GitRunner::setEmail(const QString &email)
{
    DvcsJob *job = new DvcsJob();
    initJob(*job);
    *job << "config";
    *job << "user.email";
    *job << email;

    DvcsJob::JobStatus status;
    startJob(*job, &status);
    return status;
}

