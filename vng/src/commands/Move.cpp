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

#include "Move.h"
#include "CommandLineParser.h"
#include "Logger.h"
#include "GitRunner.h"
#include "hunks/File.h"

#include <QPair>

Move::Move()
    : AbstractCommand("mv")
{
    CommandLineParser::setArgumentDefinition("mv [FILE or DIRECTORY]" );
}

AbstractCommand::ReturnCodes Move::run()
{
    if (! checkInRepository())
        return NotInRepo;
    moveToRoot();
    // Move in git is really remove and add. Git will auto detect that being a move.

    typedef QPair<QString, QString> RenamePair;

    QList<RenamePair> renamePairs;
    QStringList files = rebasedArguments();
    if (files.count() < 2) { // not sure what that would mean...
        Logger::error() << "Vng failed: you must specify at least two arguments for mv\n";
        return InvalidOptions;
    }
    QString target = files.takeLast();
    QFileInfo path(target);
    if (files.count() > 1) { // multiple files into a dir.
        if (! path.isDir()) {
            Logger::error() << "Vng failed:  The target directory '" << target <<
                "` does not seem to be an existing directory\n";
            return InvalidOptions;
        }
    }
    else { // target is a file
        if(File::fileKnownToGit(path)) {
            Logger::error() << "Vng failed: The file named '"<<  target <<
                "` already exists in working directory.\n";
            return InvalidOptions;
        }
    }
    foreach (QString arg, files) {
        if (canMoveFile(arg)) {
            if (files.count() > 1)
                renamePairs.append(qMakePair(arg, target + QDir::separator() + arg));
            else
                renamePairs.append(qMakePair(arg, target));
        }
    }

    if (dryRun())
        return Ok;

    QProcess git;
    QStringList arguments;
    arguments << "update-index" << "--add" << "--remove";
    foreach(RenamePair p, renamePairs) {
        Logger::debug() << "rename " << p.first << " => " << p.second << endl;
        if (QFile::rename(p.first, p.second))
            arguments << p.first << p.second;
        else
            Logger::error() << "Vng failed: Could not rename '" << p.first << "` to '" <<
                    p.second << "`, check permissions\n";
    }

    if (arguments.count() == 3) { // no files!
        Logger::warn() << "Nothing done\n";
        return Ok;
    }
    GitRunner runner(git, arguments);
    return runner.start(GitRunner::WaitUntilFinished);
}

bool Move::canMoveFile(const QString &file)
{
    QFileInfo path(file);
    if (path.exists()) {
        if (path.isFile()) {
            bool knownToGit = File::fileKnownToGit(path);
            if (! knownToGit) {
                Logger::warn() << "Not moving file, its not added in the repository: `" << path.filePath() << "'\n";
                return false;
            }
            return true;
        }
        else if (path.isSymLink())
            Logger::warn() << "Ignoring symbolic link '" << path.filePath() << "'" << endl;
        else
            Logger::warn() << "Ignoring non-file object '" << path.filePath() << "'" << endl;
    }
    else
        Logger::error() << "Can not move non existing' " << path.filePath() << "'" << endl;
    return false;
}

QString Move::argumentDescription() const
{
    return "<FILE or DIRECTORY>";
}

QString Move::commandDescription() const
{
    return "Mv can be called whenever you want to move files or\n"
        "directories. Unlike remove, mv actually performs the move itself in your\n"
        "working copy.\n";
}

