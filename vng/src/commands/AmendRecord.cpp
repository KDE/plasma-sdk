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

#include "AmendRecord.h"
#include "UnRecord.h"
#include "Record.h"
#include <CommandLineParser.h>
#include <CommitsCursor.h>
#include <Logger.h>
#include <Interview.h>
#include <GitRunner.h>

#include <QDebug>

static const CommandLineOption options[] = {
    {"-a, --all", "answer yes to all patches"},
    {"-i, --interactive", "prompt user interactively"},
    {"-m, --patch-name PATCHNAME", "name of patch"},
    {"--from-match PATTERN", "select changes starting with a patch matching PATTERN"},
    {"--from-patch REGEXP", "select changes starting with a patch matching REGEXP"},
    //{"--from-tag REGEXP", "select changes starting with a tag matching REGEXP"},
    {"--match PATTERN", "select patches matching PATTERN"},
    {"-p, --patches REGEXP", "select patches matching REGEXP"},
    {"-e, --edit", "(re)edit the patch name"},
    //{"-t REGEXP,--tags REGEXP", "select tags matching REGEXP"},
    {"-A, --author EMAIL", "specify author id"},
    //{"--logfile FILE", "give patch name and comment in file"},
    //{"--delete-logfile", "delete the logfile when done"},
    //{"--no-test", "don't run the test script"},
    //{"--test", "run the test script"},
    //{"-l, --look-for-adds", "Also look for files that are potentially pending addition"},
    //{"--dont-look-for-adds", "Don't look for any files that could be added"},
    //{"--posthook COMMAND", "specify command to run after this vng command."},
    //{"--no-posthook", "Do not run posthook command."},
    //{"--prompt-posthook", "Prompt before running posthook. [DEFAULT]"},
    //{"--run-posthook", "Run posthook command without prompting."},
    CommandLineLastOption
};

AmendRecord::AmendRecord()
    : AbstractCommand("Amend-Record")
{
    CommandLineParser::addOptionDefinitions(options);
}

AbstractCommand::ReturnCodes AmendRecord::run()
{
    class Recovery {
      public:
        Recovery() : switchedBranch(false), changedHead(false) {}
        ~Recovery() {
            QProcess git;
            QStringList arguments;
            GitRunner runner(git, arguments);
            if (switchedBranch && !oldBranch.isEmpty()) {
                arguments << "reset" << "--hard";
                runner.setArguments(arguments);
                runner.start(GitRunner::WaitUntilFinished);
                arguments.clear();
                arguments << "checkout" << oldBranch;
                runner.setArguments(arguments);
                runner.start(GitRunner::WaitUntilFinished);
            }
            if (!deleteBranch.isEmpty()) {
                arguments.clear();
                arguments << "branch" << "-D" << deleteBranch;
                runner.setArguments(arguments);
                runner.start(GitRunner::WaitUntilFinished);
            }
            if (changedHead && !oldHead.isEmpty()) {
                arguments.clear();
                arguments << "update-ref" << "HEAD" << oldHead;
                runner.setArguments(arguments);
                runner.start(GitRunner::WaitUntilFinished);
            }
        }

        QString deleteBranch; // the branch name we need to remove
        QString oldHead;  // the head before we started all this.
        QString oldBranch;// the branch we were on before we started all this
        bool switchedBranch;
        bool changedHead;
    };
    Recovery recovery;

    if (! checkInRepository())
        return NotInRepo;
    moveToRoot();

    // create a cursor to walk our patches.
    CommitsCursor cursor(CommitsCursor::SelectOnePatch);
    cursor.setUseMatcher(true);

    // use the fact that the cursor already has a commit with 'HEAD'
    // search which branch we are on.
    foreach (const Branch &branch, m_config.branches()) {
        if (branch.commitTreeIsmSha1() == cursor.head().commitTreeIsmSha1()) {
            recovery.oldBranch = branch.branchName();
            if (recovery.oldBranch.startsWith("heads/"))
                recovery.oldBranch = recovery.oldBranch.mid(6);
            break;
        }
    }

    if (recovery.oldBranch.isEmpty()) {
        Logger::error() << "vng Failed: Amend-record requires you to be on a named branch.\n";
        return OtherVngError;
    }

    Interview interview(cursor, name());
    interview.setUsePager(shouldUsePager());
    if (! interview.start()) {
        Logger::warn() << "Amend-record cancelled." << endl;
        return Ok;
    }

    Commit acceptedCommit = cursor.head();
    recovery.oldHead = acceptedCommit.commitTreeIsmSha1();
    while (acceptedCommit.acceptance() != Vng::Accepted) {
        acceptedCommit.commitTreeIsmSha1(); // resolve the real Sha1 internally.
        Commit commit = acceptedCommit.previous()[0];
        if (! commit.isValid())
            break;
        acceptedCommit = commit;
    }
//qDebug() << "going to amend" << acceptedCommit.commitTreeIsmSha1();
    if (dryRun())
        return Ok;

    if (acceptedCommit.previousCommitsCount() != 1) {
        Logger::error() << "Vng failed: could not find proper parent of commit\n";
        return InvalidOptions;
    }
    CommandLineParser *args = CommandLineParser::instance();
    const bool editMessage = (m_config.contains("edit") && !args->contains("skip")) || args->contains("edit");

    // do a normal record, passing in a dummy comment. This means we get the differences
    Record amendedData;
    const char* dummyPatchName = "vng amend-record temporary commit";
    const bool all = (m_config.contains("all") && !args->contains("interactive")) || args->contains("all");
    amendedData.setRecordAll(all);
    amendedData.setPatchName(QByteArray(dummyPatchName));
    ReturnCodes rc = amendedData.record();
    if (rc ) {
        if (rc != UserCancelled)
            Logger::error() << "Vng failed; internal error (record1/" << rc << ")\n";
        return rc;
    }
    if (amendedData.sha1().isEmpty() && !editMessage)
        return Ok;
    recovery.changedHead = true;

    // if there is anything left in the workdir, make sure we put that in a second patch.
    // the sha1 will just be empty if there is nothing.
    Record leftData;
    leftData.setPatchName(QByteArray(dummyPatchName));
    leftData.setRecordAll(true);
    rc = leftData.record();
    if (rc) // this means that the rest will fail, so we have to recover and abort.
        return rc;

    QString branch;
    int counter = 0;
    while(true) {
        // TODO should I replace this with various low-level commands?  (checkout-index etc)
        QProcess git;
        QStringList arguments;
        branch = QString("tmp-vng-branch-%1").arg(counter++);
        arguments << "checkout" << "-b" << branch << acceptedCommit.commitTreeIsmSha1();
        GitRunner runner(git, arguments);
        rc = runner.start(GitRunner::WaitUntilFinished);
        if (rc == Ok) // if the branch already exists, it fails.
            break;
    }
    recovery.deleteBranch = branch;
    recovery.switchedBranch = true;

    if (! amendedData.sha1().isEmpty()) { // could be empty of the user just wants to change the log message
        QProcess git;
        QStringList arguments;
        arguments << "cherry-pick" << amendedData.sha1();
        GitRunner runner(git, arguments);
        rc = runner.start(GitRunner::WaitUntilFinished);
        if (rc) {
            Logger::error() << "Vng failed: merging the selected changes failed, possibly a merge conflict.\n";
            return rc;
        }
    }

    QProcess git;
    QStringList arguments;
    arguments << "reset" << (amendedData.sha1().isEmpty() ? "HEAD^" : "HEAD^^");
    GitRunner runner(git, arguments);
    rc = runner.start(GitRunner::WaitUntilFinished);
    if (rc) {
        Logger::error() << "Vng failed; internal error (reset" << rc << ")\n";
        return rc;
    }

    Record replacementPatch;
    replacementPatch.setPatchName(acceptedCommit.logMessage());
    replacementPatch.setRecordAll(true);
    replacementPatch.setAuthor(args->optionArgument("author",
                m_config.optionArgument("author", acceptedCommit.author())));
    replacementPatch.setEditComment(editMessage);
    rc = replacementPatch.record();
    if (rc) {
        Logger::error() << "Vng failed; internal error (record2/" << rc << ")\n";
        return rc;
    }

    Record newPatch;
    newPatch.setPatchName(acceptedCommit.logMessage());
    newPatch.setRecordAll(true);
    newPatch.record();

    arguments.clear();
    arguments << "rebase" << "--onto" << branch << acceptedCommit.commitTreeIsmSha1() << recovery.oldBranch;
    runner.setArguments(arguments);
    rc = runner.start(GitRunner::WaitUntilFinished);
    if (rc) {
        Logger::error() << "Vng failed: replaying unchanged patches failed, possibly a merge conflict.\n";
        return rc;
    }
    recovery.switchedBranch = false; // we are back on our original branch now :)

    if (! leftData.sha1().isEmpty()) {
        arguments.clear();
        arguments << "reset" << "--mixed" << "HEAD^";
        runner.setArguments(arguments);
        runner.start(GitRunner::WaitUntilFinished);
    }
    recovery.oldHead.clear();

    return Ok;
}

QString AmendRecord::argumentDescription() const
{
    return "<FILE or DIRECTORY>";
}

QString AmendRecord::commandDescription() const
{
    return "Amend-record is used to replace a patch with a newer version with additional\n"
        "changes.\n"
        "\n"
        "WARNINGS: You should ONLY use amend-record on patches which only exist in a\n"
        "single repository!\n";

}
