/*
 * This file is part of the vng project
 * Copyright (C) 2008 Thomas Zander <tzander@trolltech.com>
 * Copyright (C) 2002-2004 David Roundy
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

#include "Add.h"
#include "../CommandLineParser.h"
#include "../Logger.h"
#include "../GitRunner.h"
#include "../Vng.h"
#include "../hunks/File.h"

#include <QFileInfo>
#include <QProcess>
#include <QDebug>

static const CommandLineOption options[] = {
    {"-r, --recursive", "recursively add files"},
    {"--not-recursive", "do not add files in directories recursively"},
    {"-f, --boring", "don't skip boring files"},
    CommandLineLastOption
};

Add::Add()
    : AbstractCommand("add"),
    m_excludeMatcher(m_config),
    m_argumentLength(0),
    m_fetchedTree(false)
{
    CommandLineParser::addOptionDefinitions(options);
    CommandLineParser::setArgumentDefinition("add <FILE or DIRECTORY>" );
}

AbstractCommand::ReturnCodes Add::run()
{
    if (! checkInRepository())
        return NotInRepo;
    moveToRoot();
    CommandLineParser *args = CommandLineParser::instance();
    const bool recursive = !m_config.contains("not-recursive") && !args->contains("not-recursive");

    int argIndex = 0;
    foreach(QString arg, rebasedArguments()) {
        ++argIndex;
        QFileInfo path(arg);
        if (! arg.endsWith('/') && path.isDir())
            arg = arg + '/';
        if (!args->contains("boring") && m_excludeMatcher.isExcluded(arg)) {
            Logger::warn() << "Skipping boring file: `" << args->arguments()[argIndex]  << "'\n";
            continue;
        }
        if (path.exists()) {
            if (path.isFile()) {
                ReturnCodes rc = addFile(path, true);
                if (rc != Ok)
                    return rc;
            }
            else if (path.isDir()) {
                if (recursive)
                    recurse(QDir(arg));
            }
            else if (path.isSymLink())
                Logger::warn() << "Ignoring symbolic link '" << path.filePath() << "'" << endl;
            else
                Logger::warn() << "Ignoring non-file object '" << path.filePath() << "'" << endl;
        }
        else
            Logger::error() << "Can not add non existing' " << path.filePath() << "'" << endl;
    }
    return flushAdds();
}

QString Add::argumentDescription() const
{
    return "<FILE or DIRECTORY>";
}

QString Add::commandDescription() const
{
    return "Add needs to be called whenever you add a new file or directory to your\n"
    "project.  Of course, it also needs to be called when you first create the\n"
    "project, to let vng know which files should be kept track of.\n";
}

AbstractCommand::ReturnCodes Add::addFile(const QFileInfo &path, bool warn)
{
    if (! m_config.isEmptyRepo()) {
        if (! m_fetchedTree) {
            foreach(File file, Commit::allFiles()) {
                m_fileTree << QString::fromLocal8Bit(file.fileName());
            }
            m_fetchedTree = true;
        }

        bool shouldAdd = !m_fileTree.contains(path.filePath());
        if (! shouldAdd) {
            (warn ? Logger::warn() : Logger::info()) << "Not adding file, already in the repository: `" << path.filePath() << "'\n";
            return Ok;
        }
    }
    Logger::info() << "Adding '" << path.filePath() << "'" << endl;
    if (dryRun())
        return Ok;
    QString filePath = path.filePath();
    m_filesToAdd << filePath;
    m_argumentLength += filePath.size();
    if (m_argumentLength > 30000)
        return flushAdds();
    return Ok;
}

AbstractCommand::ReturnCodes Add::flushAdds()
{
    if (m_argumentLength == 0)
        return Ok;
    QProcess git;
    QStringList arguments;
    arguments << "update-index" << "--add";
    arguments += m_filesToAdd;
    GitRunner runner(git, arguments);
    ReturnCodes rc = runner.start(GitRunner::WaitUntilFinished);
    if (rc == Ok) {
        m_filesToAdd.clear();
        m_argumentLength = 0;
    }
    return rc;
}

void Add::recurse(const QDir &dir)
{
    CommandLineParser *args = CommandLineParser::instance();
    foreach(QFileInfo file, dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (!args->contains("boring")) {
            QString path = file.filePath();
            if (file.isDir())
                path += '/';
            if (m_excludeMatcher.isExcluded(path)) {
                Logger::info() << "Skipping boring " << (file.isDir() ? "directory": "file") << "`" << file.filePath()  << "'\n";
                continue;
            }
        }
        if(file.isDir())
            recurse(QDir(file.filePath()));
        else if (file.isFile())
            addFile(file, false);
    }
}
