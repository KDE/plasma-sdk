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
#ifndef COMMIT_H
#define COMMIT_H

#include "../Vng.h"

#include <QString>
#include <QDateTime>

class CommitPrivate;
class ChangeSet;
class File;

class Commit
{
public:
    /// default constructor creates an invalid commit object.
    Commit();
    Commit(const QString &treeism, const Commit &nextCommit = Commit());
    Commit(const Commit &other);
    ~Commit();

    /// return an older commit
    QList<Commit> previous();
    /// return a commit made at a later time that has this one as its parent.
    Commit next();

    int previousCommitsCount() const;

    QString author() const;
    QString committer() const;
    QByteArray logMessage() const;
    /// The tree structure sha1, which is part of the commit.
    QString tree() const;
    /// the param passed to the constructor. This can be something like "HEAD"
    QString commitTreeIsm() const;

    /// similar to the previos but this will always return a sha1 whereas
    // the commitTreeIsm can return a ref.
    QString commitTreeIsmSha1() const;


    QDateTime commitTime() const;
    QDateTime authorTime() const;

    /// available only when the diff provided it.
    ChangeSet changeSet() const;

    /// Clear references to the previousCommits, if this commit is the last user of the parent commits it will clean up some memory.
    void clearParents();

    Commit &operator=(const Commit &other);
    bool operator==(const Commit &other);

    Vng::Acceptance acceptance() const;
    void setAcceptance(Vng::Acceptance accepted);

    bool isValid() const;

    /**
     * Return the first commit on this branch.
     * If this is head commit on a branch (not master), this will search for
     * the first commit that both this branch and another branch share.
     */
    Commit firstCommitInBranch();

    static Commit createFromStream(QIODevice *device);
    /// lists all files in the whole repo at the given commit time.
    static QList<File> allFiles(const QString &commitTreeIsm = "HEAD");

protected:
    Commit(CommitPrivate *priv);
    void fillFromTreeIsm(const QString &treeIsm);
    CommitPrivate * d;

private:
    static Commit createFromStream(QIODevice *device, CommitPrivate *priv);
};

#endif
