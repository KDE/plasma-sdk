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
#include "UnRecord.h"

#include "CommandLineParser.h"
#include "Interview.h"
#include "GitRunner.h"
#include "Logger.h"
#include "hunks/ChangeSet.h"
#include "patches/Commit.h"
#include "patches/CommitsCursor.h"

#include <QDebug>

static const CommandLineOption options[] = {
    {"--from-match PATTERN", "select changes starting with a patch matching PATTERN"},
    {"--from-patch REGEXP", "select changes starting with a patch matching REGEXP"},
    //{"--from-tag REGEXP", "select changes starting with a tag matching REGEXP"},
    {"-n, --last NUMBER", "select the last NUMBER patches"},
    {"-a, --all", "select all patches till the first branchpoint found"},
    {"--match PATTERN", "select patches matching PATTERN"},
    {"-p, --patches REGEXP", "select patches matching REGEXP"},
    //{"-t REGEXP,--tags REGEXP", "select tags matching REGEXP"},
    CommandLineLastOption
};

UnRecord::UnRecord()
    :AbstractCommand("unrecord"),
    m_all(false),
    m_last(-1)
{
    CommandLineParser::addOptionDefinitions(options);
}

QString UnRecord::argumentDescription() const
{
    return QString();
}

QString UnRecord::commandDescription() const
{
    return "UnRecord does the opposite of record in that it makes the changes from\n"
        "patches active changes again which you may record or revert later.  The\n"
        "working copy itself will not change.\n";
}

AbstractCommand::ReturnCodes UnRecord::run()
{
    CommandLineParser *args = CommandLineParser::instance();
    m_all = args->contains("all");
    if (args->contains("last")) {
        int count = args->optionArgument("last").toInt();
        if (count <= 0) {
            Logger::error() << "need a positive number for `last'\n";
            return InvalidOptions;
        }
        m_last = count;
    }
    return unRecord();
}

AbstractCommand::ReturnCodes UnRecord::unRecord()
{
    if (! checkInRepository())
        return NotInRepo;
    if (m_config.isEmptyRepo()) {
        Logger::error() << "Vng failed: Can not run unrecord on a repo without recorded pathes\n";
        return Disabled;
    }

    Commit acceptedCommit = m_unrecordCommit;
    int unrecordCount = 0;
    bool oneAutoAcceptedPatch = false;
    if (acceptedCommit.isValid()) {
        unrecordCount = 1;
    } else {
        if (m_all) {
            Commit head("HEAD");
            Commit commit = head.firstCommitInBranch();
            if (! commit.isValid()) {
                Logger::error() << "Vng failed: Could not find the branch point, are you sure you are on a branch?\n";
                return OtherVngError;
            }
            acceptedCommit = commit;
            while(true) {
                unrecordCount++;
                if (commit == head)
                    break;
                commit = commit.next();
                oneAutoAcceptedPatch = true;
            }
        }
        else { // use a cursor
            CommitsCursor cursor(CommitsCursor::SelectRange);
            cursor.setUseMatcher(true);
            if (m_last > 0) {
                for (int i = 0; i < m_last && cursor.isValid(); i++) {
                    cursor.setResponse(true);
                    cursor.forward();
                }
            }
            else {
                Interview interview(cursor, name());
                interview.setUsePager(shouldUsePager());
                if (! interview.start()) {
                    Logger::warn() << "unrecord cancelled." << endl;
                    return Ok;
                }
            }

            Commit commit = cursor.head();
            while (true) {
                if (commit.acceptance() == Vng::Rejected) // can't use this one.
                    break;
                else if (commit.acceptance() == Vng::Accepted)
                    acceptedCommit = commit;
                if (commit.previousCommitsCount() == 0) // at first commit.
                    break;
                if (commit.acceptance() == Vng::Undecided) {
                    if (acceptedCommit.isValid()) // already found a 'yes'.
                        break;
                    oneAutoAcceptedPatch = true;
                }
                commit = commit.previous()[0];
                if (unrecordCount >= cursor.oldestCommitAltered())
                    break;
                unrecordCount++;
            }
            acceptedCommit = commit;
        }
    }

    if (unrecordCount == 0 || !acceptedCommit.isValid()) {
        Logger::warn() << "Ok, if you don't want to unrecord anything, that's fine!\n";
        return Ok;
    }
    if (oneAutoAcceptedPatch) {
        QString answer = Interview::ask(QString("Do you really want to unrecord %1 patches? ").arg(unrecordCount));
        if (! (answer.startsWith("y") || answer.startsWith("Y")))
            return Ok;
    }
    QProcess git;
    QStringList arguments;
    arguments << "diff-index" << acceptedCommit.commitTreeIsm(); // get all the affected files.
    GitRunner runner(git, arguments);
    ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc) {
        Logger::error() << "Vng failed:  Internal error" << endl;
        return rc;
    }
    Commit commit = Commit::createFromStream(&git);

    QStringList addFilesList; // list of files that we have to git-add since they were not known in this revision.
    QStringList resetFileList; // list of files that we need to git-reset to make the index notice their diffs
    ChangeSet changeSet = commit.changeSet();
    for (int i=0; i < changeSet.count(); ++i) {
        File file = changeSet.file(i);
        if (file.oldFileName().isEmpty())
            addFilesList << file.fileName();
        else if (! file.fileName().isEmpty()) // ignore removed files, git will notice this
            addFilesList << file.fileName();
    }

    if (dryRun())
        return Ok;

    arguments.clear();
    arguments << "update-ref" << "HEAD" << acceptedCommit.commitTreeIsm();
    runner.setArguments(arguments);
    rc = runner.start(GitRunner::WaitUntilFinished);
    if (rc != Ok) {
        Logger::error() << "Failed to update the ref, sorry.\n";
        return rc;
    }

    // revert the index for modified files
    arguments.clear();
    arguments << "reset" << "--mixed" << "-q" << "HEAD" << "--";
    arguments << resetFileList;
    runner.setArguments(arguments);
    runner.start(GitRunner::WaitUntilFinished);

    // add all added files.
    arguments.clear();
    arguments << "add" << "--force" << "--ignore-errors" << "--";
    arguments << addFilesList;
    runner.setArguments(arguments);
    runner.start(GitRunner::WaitUntilFinished);

    return Ok;
}
