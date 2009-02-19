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

#include "Remove.h"
#include "CommandLineParser.h"
#include "GitRunner.h"

Remove::Remove()
    : AbstractCommand("remove")
{
    CommandLineParser::setArgumentDefinition("rename <FILE or DIRECTORY>" );
}

AbstractCommand::ReturnCodes Remove::run()
{
    if (! checkInRepository())
        return NotInRepo;
    moveToRoot();
    // all we need to do is tell gits index that the file is deleted.
    // easiest way to do that is to call update-index --remove --force-remove [file]

    if (dryRun())
        return Ok;

    QProcess git;
    QStringList arguments;
    arguments << "update-index" << "--remove" << "--force-remove";
    arguments += rebasedArguments();
    GitRunner runner(git, arguments);
    return runner.start(GitRunner::WaitUntilFinished);
}

QString Remove::argumentDescription() const
{
    return "<FILE or DIRECTORY>";
}

QString Remove::commandDescription() const
{
    return "Remove should be called when you want to remove a file from your project,\n"
        "but don't actually want to delete the file.  Otherwise just delete the\n"
        "file or directory, and vng will notice that it has been removed.\n"
        "Be aware that the file WILL be deleted from any other copy of the\n"
        "repository to which you later apply the patch.\n";
}

