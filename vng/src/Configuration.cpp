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
#include "Configuration.h"
#include "Logger.h"
#include "GitRunner.h"
#include "AbstractCommand.h"

#include <QDebug>
#include <QMutexLocker>
#include <QProcess>

#ifndef Q_OS_WIN
#include <unistd.h>
#endif

#define EditedFilesDatabase "vng-editedfiles"

Configuration::Configuration(const QString &section)
    : m_repoDir("."),
    m_section(section),
    m_dirty(true),
    m_emptyRepo(false),
    m_fetchedBranches(false)
{
}

bool Configuration::contains(const QString & key) const
{
    const_cast<Configuration*> (this)->readConfig();
    return m_options.contains(key);
}

QDir Configuration::repository() const
{
    const_cast<Configuration*> (this)->readConfig();
    return m_repoDir;
}

QDir Configuration::repositoryMetaDir() const
{
    const_cast<Configuration*> (this)->readConfig();
    return m_repoMetaDataDir;
}

void Configuration::readConfig()
{
    if (!m_dirty)
        return;
    m_dirty = false;
    QObject deleterParent;

    QDir dir = QDir::current();
    do {
        QDir git = dir;
        if (git.cd(".git")) {
            m_repoDir = dir;
            m_repoMetaDataDir = git;
            QDir refs(git.absoluteFilePath("refs/heads"));
            m_emptyRepo = refs.count() == 2; // only '.' and '..'
            break;
        }
        if (!dir.cdUp())
            break;
    } while(!dir.isRoot());

    QString home = QDir::homePath();
    QFile *config;
    config = new QFile(home + "/.vng/config", &deleterParent);
    if (! config->exists())
        config = new QFile(home + "/.darcs/defaults", &deleterParent);
    if (config->exists()) {
        if (! config->open(QIODevice::ReadOnly)) {
            Logger::error() << "Failed to open config file, is it readable?\n";
            return;
        }

        char buf[1024];
        while(true) {
            qint64 lineLength = config->readLine(buf, sizeof(buf));
            if (lineLength == -1)
                break;
            QString line = QString::fromUtf8(buf, lineLength);
            QString option;
            if (line.startsWith("ALL "))
                option = line.mid(3).trimmed();
            else if (line.length() > m_section.length() && line.startsWith(m_section))
                option = line.mid(m_section.length()).trimmed();
            if (! option.isEmpty()) {
                const int index = option.indexOf(' ');
                if (index > 0) {
                    m_options.insert(option.left(index).trimmed(), option.mid(index).trimmed());
                }
                else
                    m_options.insert(option, QString());
            }
        }
        config->close();
    }
}

QString Configuration::optionArgument(const QString &optionName, const QString &defaultValue) const
{
    if (m_options.contains(optionName))
        return m_options[optionName];
    return defaultValue;
}

bool Configuration::colorTerm() const
{
#ifndef Q_OS_WIN
    if (isatty(1))
        return QString(getenv("TERM")) != QString("dumb") && !Logger::hasNonColorPager();
#endif
    return false;
}

bool Configuration::isEmptyRepo() const
{
    const_cast<Configuration*> (this)->readConfig();
    return m_emptyRepo;
}

QList<Branch> Configuration::allBranches()
{
    fetchBranches();
    QList<Branch> answer;
    answer += m_localBranches;
    answer += m_remoteBranches;
    return answer;
}

QList<Branch> Configuration::branches()
{
    fetchBranches();
    return m_localBranches;
}

QList<Branch> Configuration::remoteBranches()
{
    fetchBranches();
    return m_remoteBranches;
}

void Configuration::fetchBranches()
{
    if (m_fetchedBranches)
        return;
    m_fetchedBranches = true;

    QProcess git;
    QStringList arguments;
    arguments << "ls-remote" << ".";
    GitRunner runner(git, arguments);
    AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc != AbstractCommand::Ok) {
        return;
    }
    char buf[1024];
    while(true) {
        qint64 lineLength = Vng::readLine(&git, buf, sizeof(buf));
        if (lineLength == -1)
            break;
        if (lineLength > 46) { // only take stuff that is in the 'refs' dir.
            QString name(buf + 46);
            name = name.trimmed(); // remove linefeed
            Branch branch(name, QString::fromLatin1(buf, 40));
            if (name.startsWith("remotes"))
                m_remoteBranches.append(branch);
            else
                m_localBranches.append(branch);
        }
    }
}
