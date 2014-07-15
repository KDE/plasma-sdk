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

#ifndef GIT_H
#define GIT_H

#include <QObject>
#include <QUrl>

#include <vcs/vcsevent.h>

namespace KDevelop {
    class IProject;
    class VcsJob;
    class IDistributedVersionControl;
    class IBranchingVersionControl;
}; // end namespace

class QAbstractItemModel;

class CommitsModel;

class Git: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* commitsModel READ commitsModel NOTIFY commitsModelChanged)
    Q_PROPERTY(QString currentBranch READ currentBranch CONSTANT)
    Q_PROPERTY(QStringList branches READ branches CONSTANT)

public:
    Git(QObject *parent = 0);
    ~Git();

    void setProject(KDevelop::IProject *project);
    KDevelop::IProject *project() const;

    bool initGit();

    bool isRepository();

    Q_INVOKABLE bool initializeRepository();

    QList<KDevelop::VcsEvent> commits();
    QStringList branches();

    QString currentBranch();
    Q_INVOKABLE bool renameBranch(const QString &oldName, const QString &newName);
    Q_INVOKABLE bool switchBranch(const QString &branchName);
    Q_INVOKABLE bool deleteBranch(const QString &branchName);
    Q_INVOKABLE bool createBranch(const QString &branchName);
    Q_INVOKABLE bool newSavePoint(const QString &commitMessage, bool saveDocuments = true);

    QAbstractItemModel* commitsModel() const;

Q_SIGNALS:
    void commitsModelChanged();

private:
    bool handleJob(KDevelop::VcsJob *job);
    QUrl m_repositoryPath;

    KDevelop::IProject *m_project;
    KDevelop::IDistributedVersionControl *m_dvcs;
    KDevelop::IBranchingVersionControl *m_branching;

    CommitsModel *m_commitsModel;
};

#endif

