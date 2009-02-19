/*
 * This file is part of the vng project
 * Copyright (C) 2008 Thomas Zander <tzander@trolltech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Commit.h"
#include "Commit_p.h"
#include "../GitRunner.h"
#include "../Logger.h"

#include <QProcess>
#include <QDebug>

static QDateTime timeFromSecondsSinceEpoch(const QString &string) {
    const int split = string.lastIndexOf('>');
    const int tz = string.lastIndexOf(' ');
    if (split > 0 && tz > split)
        return QDateTime::fromTime_t(string.mid(split + 1, tz - split).toLong());
    return QDateTime();
}

Commit::Commit(CommitPrivate *priv)
    : d(priv)
{
}

Commit::Commit()
    : d(0)
{
}

Commit::Commit(const QString &treeIsm, const Commit &nextCommit)
    : d(0)
{
    fillFromTreeIsm(treeIsm);
    if (d)
        d->child = nextCommit;
}

Commit::Commit(const Commit &other)
    : d(other.d)
{
    if (d)
        d->ref++;
}

Commit::~Commit()
{
    if (d && --d->ref == 0)
        delete d;
}

QList<Commit> Commit::previous()
{
    if (d->previousCommits.count() != d->parentTreeisms.count()) {
        foreach(QString id, d->parentTreeisms)
            d->previousCommits.append(Commit(id, *this));
    }
    return d->previousCommits;
}

int Commit::previousCommitsCount() const
{
    return d->parentTreeisms.count();
}

QString Commit::author() const
{
    int split = d->author.lastIndexOf('>');
    if (split > 0)
        return d->author.left(split + 1);
    return d->author;
}

QString Commit::committer() const
{
    int split = d->committer.lastIndexOf('>');
    if (split > 0)
        return d->committer.left(split + 1);
    return d->committer;
}

QDateTime Commit::commitTime() const
{
    return timeFromSecondsSinceEpoch(d->committer);
}

QDateTime Commit::authorTime() const
{
    return timeFromSecondsSinceEpoch(d->author);
}

QByteArray Commit::logMessage() const
{
    return d->logMessage;
}

QString Commit::tree() const
{
    return d->tree;
}

void Commit::clearParents()
{
    d->previousCommits.clear();
}

Commit &Commit::operator=(const Commit &other)
{
    if (other.d)
        other.d->ref++;
    if (d && --d->ref == 0)
        delete d;
    d = other.d;
    return *this;
}

bool Commit::operator==(const Commit &other)
{
    return other.d == d || (other.d && d && other.d->tree == d->tree);
}

Vng::Acceptance Commit::acceptance() const
{
    return d->acceptance;
}

void Commit::setAcceptance(Vng::Acceptance accepted)
{
    d->acceptance = accepted;
}

bool Commit::isValid() const
{
    return d && !d->tree.isEmpty();
}

QString Commit::commitTreeIsm() const
{
    return d->treeIsm;
}

QString Commit::commitTreeIsmSha1() const
{
    if (! d->resolvedTreeIsm.isEmpty())
        return d->resolvedTreeIsm;

    QProcess git;
    QStringList arguments;
    if (d->treeIsm == "HEAD") // grmbl; git requires us to special case this...
        arguments << "show-ref" << "--hash" << "--head";
    else
        arguments << "show-ref" << "--hash" << d->treeIsm;
    GitRunner runner(git, arguments);
    AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc)
        return d->treeIsm;

    char buf[50];
    while(true) {
        qint64 lineLength = Vng::readLine(&git, buf, sizeof(buf));
        if (lineLength == -1)
            break;
        d->resolvedTreeIsm += QString::fromLatin1(buf, lineLength);
        if (d->resolvedTreeIsm.length() > 40)
            break;
    }
    d->resolvedTreeIsm = d->resolvedTreeIsm.trimmed();
    if (d->resolvedTreeIsm.length() != 40) {
        Logger::warn() << "Commit::commitTreeIsmSha1: show-ref gave unexpected; '"  << d->resolvedTreeIsm << "`\n";
        d->resolvedTreeIsm.clear();
    }
    return d->resolvedTreeIsm;
}

Commit Commit::next()
{
    return d->child;
}

Commit Commit::firstCommitInBranch()
{
    Q_ASSERT(isValid());
    int max = 50;
    Q_ASSERT(isValid());
    if (previousCommitsCount() == 0) {
        Logger::debug() << "Commit::firstCommitInBranch: No parent commit\n";
        return Commit(); // this is the first commit in the repo
    }

    Commit currentBranch;

    QList<Commit> otherBranches;

    QDir heads(".git/refs/heads");
    foreach (QString head, heads.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
        Commit branch(head);
        if (operator==(branch)) { // this only works if this commit is the curent HEAD...
            currentBranch = (*this);
            continue;
        }
        otherBranches.append(branch);
    }
    if (!currentBranch.isValid()) { // we are not on a branch...
        Logger::debug() << "Commit::firstCommitInBranch: we are not on a branch\n";
        return Commit();
    }

    currentBranch = currentBranch.previous()[0];
    while(true) {
        QDateTime time = currentBranch.commitTime();
        QList<Commit> list;
        foreach (Commit c, otherBranches) {
            Q_ASSERT(c.previousCommitsCount());
            while(c.commitTime() > time)
                c = c.previous()[0];
            if (c == currentBranch && currentBranch.previousCommitsCount() == 1)
                return currentBranch;
            list.append(c);
        }
        otherBranches = list;
        if (currentBranch.previousCommitsCount() == 0)
            return currentBranch;
        currentBranch = currentBranch.previous()[0];
        if (--max <= 0)
            return Commit();
    }
}

ChangeSet Commit::changeSet() const
{
    return d->changeSet;
}

void Commit::fillFromTreeIsm(const QString &treeIsm)
{
    QProcess git;
    QStringList arguments;
    arguments << "cat-file" << "commit" << treeIsm;
    GitRunner runner(git, arguments);
    AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc) {
        Logger::info() << "Invalid treeIsm passed " << treeIsm << endl;
        delete d;
        d = 0;
        return;
    }
    Commit c = createFromStream(&git, d);
    d = c.d;
    d->treeIsm = treeIsm;
    if (d)
        d->ref++;

    if (d->treeIsm.length() == 40) // more checks?
        d->resolvedTreeIsm = d->treeIsm;
}

// static
Commit Commit::createFromStream(QIODevice *device)
{
    return Commit::createFromStream(device, 0);
}

// static
Commit Commit::createFromStream(QIODevice *device, CommitPrivate *priv)
{
    if (priv == 0)
        priv = new CommitPrivate();
    char buf[1024];

    enum State { Init, Header, Comment, Files, Done };
    State state = Init;
    while (state != Done) {
        qint64 lineLength = Vng::readLine(device, buf, sizeof(buf));
        if (lineLength == -1)
            break;
        QString line = QString::fromLocal8Bit(buf, lineLength);
        if (state == Init || state == Header) {
            state = Header;
            if (line.length() == 1) {
                state = Comment;
                continue;
            }
            else if (line.startsWith("commit "))
                priv->treeIsm = line.mid(7).trimmed();
            else if (line.startsWith("parent "))
                priv->parentTreeisms.append(line.mid(7).trimmed());
            else if (line.startsWith("author "))
                priv->author = line.mid(7).trimmed();
            else if (line.startsWith("committer "))
                priv->committer = line.mid(10).trimmed();
            else if (line.startsWith("tree "))
                priv->tree = line.mid(5).trimmed();
            else
                state = Init;
        }
        if ((state == Init || state == Files) && line.length() > 0) {
            if (line[0] == ':')
                state = Files;
            if (line.length() > 100) {
                if (priv->parentTreeisms.count() <= 1) { // skip file listings for merges
                    File file;
                    file.setOldProtection(line.mid(1, 6));
                    file.setProtection(line.mid(8, 6));
                    file.setOldSha1(line.mid(15, 40));
                    file.setSha1(line.mid(56, 40));
                    file.setFileName(QByteArray(buf + 99, lineLength - 100)); // immediately cut off the linefeed
                    switch (buf[97]) {
                        case 'M': file.setOldFileName(file.fileName()); break;
                        case 'D': file.setOldFileName(file.fileName()); file.setFileName(QByteArray()); break;
                        default:
                                  break;
                    }
                    priv->changeSet.addFile(file);
                }
            }
            else {
                device->waitForReadyRead(-1);
                lineLength = device->peek(buf, 2);
                if (lineLength == -1 || buf[0] == 'c') { // exit, the next line is not part of this commit no more.
                    state = Done;
                    break;
                }
            }
        }
        if (state == Comment) {
            if (line.length() == 1)
                state = Init; // can be part of the comment, or we will start the Files section soon.
            else {
                if (state == Init && ! priv->logMessage.isEmpty())
                    priv->logMessage.append('\n');
                priv->logMessage += line;
            }
            continue;
        }
    }

    Commit answer;
    Q_ASSERT(answer.d == 0); // make sure there will be no mem-leak
    answer.d = priv;
    return answer;
}

// static
QList<File> Commit::allFiles(const QString &commitTreeIsm)
{
    QList<File> files;

    QProcess git;
    QStringList arguments;
    arguments << "ls-tree" << "-r" << commitTreeIsm;
    GitRunner runner(git, arguments);
    AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc != AbstractCommand::Ok)
        return files;
    char buf[4000];
    while(true) {
        qint64 lineLength = Vng::readLine(&git, buf, sizeof(buf));
        if (lineLength == -1)
            break;
        Q_ASSERT(lineLength > 53);
        buf[lineLength - 1] = 0; // remove the linefeed
        File file;
        file.setFileName(QByteArray(buf + 53, lineLength - 54));
        file.setProtection(QString::fromLatin1(buf, 6));
        file.setSha1(QString::fromLatin1(buf + 12, 40));
        files << file;
    }
    git.waitForFinished();
    return files;
}
