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

#include "Revert.h"
#include "CommandLineParser.h"
#include "Interview.h"
#include "Logger.h"
#include "GitRunner.h"
#include "hunks/ChangeSet.h"
#include "hunks/HunksCursor.h"

#include <QProcess>
#include <QDebug>

static const CommandLineOption options[] = {
    {"-a, --all", "answer yes to all patches"},
    {"-i, --interactive", "prompt user interactively"},
    CommandLineLastOption
};

Revert::Revert()
    : AbstractCommand("revert")
{
    CommandLineParser::addOptionDefinitions(options);
    CommandLineParser::setArgumentDefinition("revert [FILE or DIRECTORY]" );
}

AbstractCommand::ReturnCodes Revert::run()
{
    if (! checkInRepository())
        return NotInRepo;
    moveToRoot( static_cast <RebaseOptions> (CheckFileSystem | PrintError) ); // XXX why is the static cast needed :(

    CommandLineParser *args = CommandLineParser::instance();
    const bool all = (m_config.contains("all") && !args->contains("interactive")) || args->contains("all");

    ChangeSet changeSet;
    changeSet.fillFromCurrentChanges(rebasedArguments());

    changeSet.waitForFinishFirstFile();
    bool shouldDoRevert = changeSet.count() > 0;
    if (!shouldDoRevert)
        Logger::warn() << "There are no changes to revert!" << endl;

    if (shouldDoRevert && !all) { // then do interview
        HunksCursor cursor(changeSet);
        cursor.setConfiguration(m_config);
        Interview interview(cursor, name());
        interview.setUsePager(shouldUsePager());
        if (! interview.start()) {
            Logger::warn() << "Revert cancelled." << endl;
            return Ok;
        }
    }

    if (shouldDoRevert && !all) { // check if there is anything selected
        shouldDoRevert = changeSet.hasAcceptedChanges();
        if (! shouldDoRevert)
            Logger::warn() << "If you don't want to revert after all, that's fine with me!" <<endl;
    }

    if (shouldDoRevert && !all) { // ask user confirmation
        QString answer = Interview::ask("Do you really want to revert these changes? ");
        if (! (answer.startsWith("y") || answer.startsWith("Y")))
            return Ok;
    }

    if (! dryRun() && shouldDoRevert) { // we then write out the patch and call git apply with it
        QFile outFile(patchFileName());
        changeSet.writeDiff(outFile, all ? ChangeSet::AllHunks : ChangeSet::UserSelection);

        QProcess git;
        QStringList arguments;
        arguments << "apply" << "--apply" <<  "--reverse" << outFile.fileName();

        GitRunner runner(git, arguments);
        AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitUntilFinished);
        if (rc != Ok) {
            Logger::error() << "vng failed, internal error; failed to patch, sorry\n";
            outFile.remove();
            return rc;
        }
    }

    Logger::warn() << "Finished reverting." << endl;
    return Ok;
}

QString Revert::argumentDescription() const
{
    return "[FILE or DIRECTORY]";
}

QString Revert::commandDescription() const
{
    return "Revert is used to undo changes made to the working copy which have\n"
        "not yet been recorded.  You will be prompted for which changes you\n"
        "wish to undo. The last revert can be undone safely using the unrevert\n"
        "command if the working copy was not modified in the meantime.\n";
}

// static
QString  Revert::patchFileName()
{
    return QString::fromLatin1(".git/.vng-revert.diff");
}

/*
    TODO
    make sure we can revert file-renames too and file adds too
    check how we handle binary patches (i.e. no dataloss)
*/
