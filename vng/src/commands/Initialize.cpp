/*
 * This file is part of the vng project
 * Copyright (C) 2008 Thomas Zander <tzander@trolltech.com>
 * Copyright (C) 2003-2005 David Roundy
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
#include "Initialize.h"

#include "Vng.h"
#include "GitRunner.h"
#include "Logger.h"

// #include <QDebug>

Initialize::Initialize()
    :AbstractCommand("initialize")
{
}

QString Initialize::argumentDescription() const
{
    return QString();
}

QString Initialize::commandDescription() const
{
    return "Generally you will only call initialize once for each project you work on,\n"
        "and calling it is just about the first thing you do.  Just make sure\n"
        "you are in the main directory of the project, and initialize will set up all\n"
        "the directories and files vng needs in order to start keeping track of\n"
        "revisions for your project.\n";
}

AbstractCommand::ReturnCodes Initialize::run()
{
    QProcess git;
    QStringList arguments;
    arguments << "init" << "--quiet";
    GitRunner runner(git, arguments);
    runner.start(GitRunner::WaitForStandardError);

    char buf[1024];
    qint64 lineSize = Vng::readLine(&git, buf, sizeof(buf));
    git.waitForFinished();
    if (git.exitCode() != 0) {
        Logger::error() << "Failed to initialize the repo.  Do you have writing rights in the current directory?\n";
        Logger::info() << "initialize tries to create a directory '.git` in the current directory\n";
        if (lineSize > 0)
            Logger::debug() << "Git said; " << buf;
        return WriteError;
    }

    Configuration newConfig("initialize");
    newConfig.repositoryMetaDir().mkpath("info");
    QFile file(newConfig.repositoryMetaDir().path() + "/info/exclude");
    if (! file.open(QIODevice::WriteOnly))
        return Ok; // silently ignore, its not that important...

    const char *ignoreContents = "*.o\n.git/*\n.*.sw[po]\n*.o.cmd\n# *.ko files aren't boring by default because they might\n# be Korean translations rather than kernel modules.\n#*.ko\n*.ko.cmd\n*.mod.c\n_darcs/*\nCVS/*\n.svn/*\nRCS/*\n*.bak\n*.BAK\n*.orig\n*vssver.scc\nMT/*\n*.class\n*.prof\n*.DS_Store\nBitKeeper/*\nChangeSet/*\n*.py[co]\n.cvsignore\nThumbs.db\n*autom4te.cache\n";
    file.write(ignoreContents, strlen(ignoreContents));
    file.close();
    return Ok;
}

