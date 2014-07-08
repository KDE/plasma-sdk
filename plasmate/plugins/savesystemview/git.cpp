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

#include <interfaces/icore.h>
#include <interfaces/iproject.h>
#include <interfaces/iplugincontroller.h>

#include <util/path.h>

#include <vcs/interfaces/ibasicversioncontrol.h>
#include <vcs/interfaces/ibranchingversioncontrol.h>
#include <vcs/interfaces/idistributedversioncontrol.h>
#include <vcs/vcsjob.h>
#include <vcs/dvcs/dvcsjob.h>

#include <KLocalizedString>
#include <KMessageBox>

#include <QDirIterator>
#include <QDebug>

Git::Git(KDevelop::IProject *project, QObject *parent)
    : QObject(parent)
{
    m_project = project;
}

Git::~Git()
{
}

bool Git::initGit()
{
    if (!m_project) {
        return false;
    }

    if (!m_project->versionControlPlugin()) {
        KDevelop::IPlugin* plugin = KDevelop::ICore::self()->pluginController()->loadPlugin(QStringLiteral("kdevgit"));

        if (!plugin) {
            // the git plugin doesn't exist
            // there is nothing more for us to do then
            return false;
        }

        m_git = plugin->extension<KDevelop::IDistributedVersionControl>();

        if (!m_git) {
            // something went wrong
            return false;
        }
    }

    return true;
}


void Git::initializeRepository()
{
    if (!m_git) {
        return;
    }

    KDevelop::VcsJob *job = m_git->init(m_project->path().toUrl());
    if (!handleJob(job)) {
        return;
    }

    QStringList filesAndDirs;
    QDirIterator it(m_project->path().toUrl().toLocalFile(), QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString entry = it.next();
        const QString excludedFile = m_project->name() + ".plasmate";
        if (it.fileInfo().isFile() && it.fileName() != excludedFile) {
            filesAndDirs << it.filePath();
        }

    }

    job = m_git->add(filesAndDirs, KDevelop::IBasicVersionControl::Recursive);

    if (!handleJob(job)) {
        return;
    }

    job = m_git->commit(QStringLiteral("initial Commit"),
                      filesAndDirs,
                      KDevelop::IBasicVersionControl::Recursive);

    if (!handleJob(job)) {
        return;
    }
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

#include "git.moc"

