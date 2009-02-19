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
#include "UnRevert.h"

#include "CommandLineParser.h"
#include "Revert.h"
#include "Interview.h"
#include "GitRunner.h"
#include "Logger.h"
#include "hunks/ChangeSet.h"
#include "hunks/HunksCursor.h"

#include <QDebug>

static const CommandLineOption options[] = {
    {"-a, --all", "answer yes to all patches"},
    {"-i, --interactive", "prompt user interactively"},
    CommandLineLastOption
};

UnRevert::UnRevert()
    :AbstractCommand("unrevert")
{
    CommandLineParser::addOptionDefinitions(options);
}

QString UnRevert::argumentDescription() const
{
    return QString();
}

QString UnRevert::commandDescription() const
{
    return "Unrevert is used to undo the results of a revert command. It is only\n"
        "guaranteed to work properly if you haven't made any changes since the\n"
        "revert was performed.\n";
}

AbstractCommand::ReturnCodes UnRevert::run()
{
    if (! checkInRepository())
        return NotInRepo;
    moveToRoot();
    CommandLineParser *args = CommandLineParser::instance();
    const bool all = (m_config.contains("all") && !args->contains("interactive")) || args->contains("all");

    ChangeSet changeSet;
    QFile file(Revert::patchFileName());
    if (file.exists())
        changeSet.fillFromDiffFile(file);

    bool shouldDoRevert = changeSet.count() > 0;
    if (!shouldDoRevert)
        Logger::warn() << "There is nothing to unrevert!" << endl;

    if (shouldDoRevert && !all) { // then do interview
        HunksCursor cursor(changeSet);
        Interview interview(cursor, name());
        interview.setUsePager(shouldUsePager());
        if (! interview.start()) {
            Logger::warn() << "Unrevert cancelled." << endl;
            return Ok;
        }
    }

    if (! dryRun() && shouldDoRevert) { // we then write out the patch and call git apply with it
        QFile patchFile(Revert::patchFileName() + ".tmp");
        changeSet.writeDiff(patchFile, all ? ChangeSet::AllHunks : ChangeSet::UserSelection);
        QProcess git;
        QStringList arguments;
        arguments << "apply" << "--apply" << patchFile.fileName();
        GitRunner runner(git, arguments);
        AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitUntilFinished);
        patchFile.remove();
        if (rc != 0) {
            Logger::error() << "Unreverting failed on one or more patches";
            if (Logger::verbosity() < Logger::Chatty)
                Logger::error() << ", rerun with --standard-verbosity to see the warnings";
            Logger::error() << endl;
            Logger::warn() << git.readAllStandardError();
            Logger::warn().flush();
            return rc;
        }
        changeSet.writeDiff(file, all ? ChangeSet::AllHunks : ChangeSet::InvertedUserSelection);
        if (file.size() == 0)
            file.remove();
    }
    Logger::warn() << "Finished unreverting." << endl;
    return Ok;
}
