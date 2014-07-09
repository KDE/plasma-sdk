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

#include <vcs/vcsevent.h>

namespace KDevelop {
    class IProject;
    class VcsJob;
    class IDistributedVersionControl;
    class IBranchingVersionControl;
}; // end namespace

class Git: public QObject
{
    Q_OBJECT
public:
    Git(KDevelop::IProject *project, QObject *parent = 0);
    ~Git();

    bool initGit();

    void initializeRepository();
    QList<KDevelop::VcsEvent> commits();
    QStringList branches();
    QString currentBranch();
    bool renameBranch(const QString &oldName, const QString &newName);
    bool switchBranch(const QString &branchName);
    bool deleteBranch(const QString &branchName);
    bool createBranch(const QString &branchName);

private:
    bool handleJob(KDevelop::VcsJob *job);
    QString m_repositoryPath;

    KDevelop::IProject *m_project;
    KDevelop::IDistributedVersionControl *m_dvcs;
    KDevelop::IBranchingVersionControl *m_branching;
};

#endif

