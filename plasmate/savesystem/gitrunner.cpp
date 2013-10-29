/******************************************************************************
 * Copyright (C) 2009 by Diego '[Po]lentino' Casella <polentino911@gmail.com> *
 * Copyright (C) 2013 by Giorgos Tsiapaliokas <terietor@gmail.com>            *
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


GitRunner::GitRunner(QObject *parent)
        :QObject(parent)
{

    m_lastRepoRoot = new KUrl();

    //signals
    connect(this, SIGNAL(isValidDirectory()), this, SLOT(handleIsValidDirectory()));
}

GitRunner::~GitRunner()
{
    delete m_lastRepoRoot;
}

KJob *GitRunner::initJob(const QStringList& command) const

{
    QStringList comm;
    comm << "git";
    foreach(const QString&c, command) {
        comm << c;
    }
    KJob *job = new DvcsJob(comm, m_lastRepoRoot->pathOrUrl());
    kDebug() << m_lastRepoRoot->pathOrUrl();
    return job;
}

QString GitRunner::execSynchronously(const QStringList& command)
{
    KJob *job = initJob(command);

    QString result;

    if (!job->exec()) {
        handleError(job);
        return QString();
    } else {
        DvcsJob *j = qobject_cast<DvcsJob*>(job);
        if (!j) {
            return QString();
        }
        result = j->output();
    }
    return result;
}

void GitRunner::setDirectory(const KUrl &dir)
{
    m_lastRepoRoot->setDirectory(dir.pathOrUrl());
}

bool GitRunner::isValidDirectory()
{
    const QString initialPath(m_lastRepoRoot->toLocalFile(KUrl::RemoveTrailingSlash));
    setDirectory(*m_lastRepoRoot);

    // A possible git repo has a .git subdicerctory
    const QString gitDir(".git");

    // Also, git rev-parse --is-inside-work-tree returns "true" if we are
    // inside any subdirectory of the git tree.
    QStringList command;
    command << "rev-parse";
    command << "--is-inside-work-tree";
    const QString result= execSynchronously(command);

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
    QStringList command;
    command << "diff";
    const QString result = execSynchronously(command);

    return !result.simplified().isEmpty();
}

void GitRunner::init()
{
    QStringList command;
    command << "init";
    KJob *job = initJob(command);

    connect(job, SIGNAL(result(KJob*)), this, SLOT(handleInit(KJob*)));
    job->start();
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

void GitRunner::createWorkingCopy(const KUrl &repoOrigin,
                                                const KUrl &repoDestination)
{
    // TODO: now supports only cloning a local repo(not very useful, I know =P),
    // so extend the method to be used over the Internet.
    m_lastRepoRoot->setDirectory(repoDestination.pathOrUrl());

    QStringList command;
    command << "clone " + repoOrigin.pathOrUrl();
    KJob *job = initJob(command);

    connect(job, SIGNAL(result(KJob*)), this, SLOT(handleCreateWorkingCopy(KJob*)));

    job->start();
}

void GitRunner::add(const KUrl::List &localLocations)
{
    if (localLocations.empty()) {
        return;
    }

    QStringList command;
    command << "add";

    // Adding files to the runner.
    QStringList stringFiles = localLocations.toStringList();
    while (!stringFiles.isEmpty()) {
        command.append(m_lastRepoRoot->pathOrUrl() + '/' + stringFiles.takeAt(0));
    }

    execSynchronously(command);
}

QString GitRunner::status()
{
    QStringList command;
    command << "status";
    return execSynchronously(command);
}


void GitRunner::commit(const QString &message)
{
    // NOTE: git doesn't allow empty commit !
    if (message.isEmpty()) {
        return;
    }

    QStringList command;
    command << "commit";
    command << "-m";
    //Note: the message is quoted somewhere else
    command << message;

    KJob *job = initJob(command);

    connect(job, SIGNAL(result(KJob*)), this, SLOT(handleCommit(KJob*)));

    job->start();
}

void GitRunner::moveToCommit(const QString &sha1hash,
                                           const QString &newBranch)
{
    QStringList com;
    com << "checkout " + sha1hash;
    execSynchronously(com);
    QStringList command;
    command << "checkout -b " + newBranch;
    KJob *job = initJob(command);

    connect(job, SIGNAL(result(KJob*)), this, SLOT(handleMoveToCommit(KJob*)));

    job->start();
}

void GitRunner::deleteCommit(const QString &sha1hash)
{
    QStringList command;
    command << "reset --hard " + sha1hash;
    KJob *job = initJob(command);

    connect(job, SIGNAL(result(KJob*)), this, SLOT(handleDeleteCommit(KJob*)));

    job->start();
}

void GitRunner::remove(const KUrl::List &files)
{
    if (files.empty()) {
        return;
    }

    QStringList command;
    command << "rm ";
    QStringList stringFiles = files.toStringList();
    while (!stringFiles.isEmpty()) {
        command.append(m_lastRepoRoot->pathOrUrl() + '/' + stringFiles.takeAt(0));
    }

    KJob *job = initJob(command);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(handleRemove(KJob*)));

    job->start();
}


QList<QHash<QString, QString> > GitRunner::log()
{
    QList<QHash<QString, QString> > data;
    QHash<QString, QString> temp;

    QStringList command;
    command << "log";
    //we will use $ in the format in order to
    //split the string author, date, subject
    //and we will use ^^ in order to identify every commit
    command << "--pretty=format:%an$%s$%ad$%H^^";

    QString result = execSynchronously(command);

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

void GitRunner::switchBranch(const QString &newBranch)
{
    QStringList command;
    command << "checkout";
    command << newBranch;
    KJob *job = initJob(command);

    connect(job, SIGNAL(result(KJob*)), this, SLOT(handleSwitchBranch(KJob*)));

    job->start();
}

void GitRunner::mergeBranch(const QString &branchName,
                                          const QString &message)
{
    QStringList command;
    command << "merge" << "--no-ff" << "-m" << message << branchName;
    execSynchronously(command);
}

void GitRunner::deleteBranch(const QString &branch)
{
    QStringList command;
    command << "branch -D" + branch;
    KJob *job = initJob(command);

    connect(job, SIGNAL(result(Kjob*)), this, SLOT(handleDeleteBranch(Kjob*)));
    job->start();
}

QString GitRunner::currentBranch()
{
    branches();

    if (m_branchesWithAsterisk.isEmpty()) {
        return QString();
    }

    QString tmp = m_branchesWithAsterisk.takeFirst();
    while (!tmp.contains('*', Qt::CaseInsensitive)) {
        tmp = m_branchesWithAsterisk.takeFirst();
    }

    tmp.remove(0, 2);
    return tmp;
}

QStringList GitRunner::branches()
{
    QStringList command;
    command << "branch";
    const QString result = execSynchronously(command);
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

void GitRunner::newBranch(const QString &newBranch)
{
    QStringList command;
    command << "branch" <<  newBranch;
    KJob *job = initJob(command);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(handleNewBranch(KJob*)));

    job->start();
}

void GitRunner::renameBranch(const QString &newBranch)
{
    QStringList command;
    command << "branch -m " + newBranch;
    KJob *job = initJob(command);

    connect(job, SIGNAL(result(KJob*)), this, SLOT(handleRenameBranch(KJob*)));
    job->start();
}

void GitRunner::setAuthor(const QString &username)
{
    QStringList command;
    command << "config user.name " + username;
    execSynchronously(command);
}

void GitRunner::setEmail(const QString &email)
{
    QStringList command;
    command << "config user.email " + email;
    execSynchronously(command);
}

void GitRunner::handleRenameBranch(KJob *job)
{
    if (!job) {
        return;
    }

    if (job->error()) {
        handleError(job);
    }

    emit renameBranchFinished();
}

void GitRunner::handleDeleteBranch(KJob *job)
{
    if (!job) {
        return;
    }

    if (job->error()) {
        handleError(job);
    }

    emit deleteBranchFinished();
}

void GitRunner::handleMoveToCommit(KJob *job)
{
    if (!job) {
        return;
    }

    if (job->error()) {
        handleError(job);
    }

    emit moveToCommitFinished();
}

void GitRunner::handleNewBranch(KJob *job)
{
    if (!job) {
        return;
    }

    if (job->error()) {
        handleError(job);
    }

    emit newBranchFinished();
}

void GitRunner::handleRemove(KJob *job)
{
    if (!job) {
        return;
    }

    if (job->error()) {
        handleError(job);
    }

    emit removeFinished();
}

void GitRunner::handleSwitchBranch(KJob *job)
{
    if (!job) {
        return;
    }

    if (job->error()) {
        handleError(job);
    }

    emit switchBranchFinished();
}

void GitRunner::handleInit(KJob *job)
{
    if (!job) {
        return;
    }

    if (job->error()) {
        handleError(job);
    } else {
        QDir workingDir(m_lastRepoRoot->pathOrUrl());
        add(workingDir.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs));
        commit(QLatin1String("Initial commit from Plasmate. Add the default files"));
    }
    emit initFinished();
}

void GitRunner::handleCreateWorkingCopy(KJob *job)
{
    if (!job) {
        return;
    }

    if (job->error()) {
        handleError(job);
    }

    emit createWorkingCopyFinished();
}

void GitRunner::handleDeleteCommit(KJob *job)
{
    if (!job) {
        return;
    }

    if (job->error()) {
        handleError(job);
    }

    emit deleteCommitFinished();
}

void GitRunner::handleCommit(KJob *job)
{
    if (!job) {
        return;
    }

    if (job->error()) {
        handleError(job);
    }

    emit commitFinished();
}

void GitRunner::handleError(KJob *job)
{
    DvcsJob *j = qobject_cast<DvcsJob*>(job);
    if (!j) {
        return;
    }

    kDebug() << "oops, found an error while running" << j->dvcsCommand() << ":" << j->error()
    << j->errorText();
    kDebug() << "output of " << j->dvcsCommand() << "is:" << j->output();
}

