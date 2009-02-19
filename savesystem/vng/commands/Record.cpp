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
#include "Record.h"
#include "CommandLineParser.h"
#include "GitRunner.h"
#include "hunks/ChangeSet.h"
#include "hunks/HunksCursor.h"
#include "Logger.h"
#include "Interview.h"
#include "Vng.h"

#include <QDebug>
#include <QBuffer>

#ifdef Q_OS_WIN
# include <cstdlib>
# define getpid() rand()
#else
# include <sys/types.h>
# include <unistd.h>
#endif


static const CommandLineOption options[] = {
    {"-a, --all", "answer yes to all patches"},
    {"-i, --interactive", "prompt user interactively"},
    {"-m, --patch-name PATCHNAME", "name of patch"},
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

Record::Record()
    : AbstractCommand("record"),
    m_mode(Unset),
    m_editComment(false),
    m_all(false)
{
    CommandLineParser::addOptionDefinitions(options);
    CommandLineParser::setArgumentDefinition("record [FILE or DIRECTORY]" );
}

QString Record::argumentDescription() const
{
    return "[FILE or DIRECTORY]";
}

QString Record::commandDescription() const
{
    return "Record is used to name a set of changes and record the patch to the repository.\n";
}

void Record::setPatchName(const QByteArray &message)
{
    m_patchName = message;
}

QByteArray Record::patchName() const
{
    return m_patchName;
}

void Record::setUsageMode(UsageMode mode)
{
    m_mode = mode;
}

QString Record::sha1() const
{
    return m_sha1;
}

AbstractCommand::ReturnCodes Record::run()
{
    CommandLineParser *args = CommandLineParser::instance();
    m_all = m_config.isEmptyRepo()
        || (m_config.contains("all") && !args->contains("interactive"))
        || args->contains("all");
    m_patchName = args->optionArgument("patch-name", m_config.optionArgument("patch-name")).toUtf8();
    m_author = args->optionArgument("author", m_config.optionArgument("author"));

    return record();
}

AbstractCommand::ReturnCodes Record::record()
{
    if (! checkInRepository())
        return NotInRepo;
    moveToRoot(CheckFileSystem);

    if (m_mode != Unset)
        m_all = m_mode == AllChanges;
    const bool needHunks = !m_all || m_patchName.isEmpty();

    ChangeSet changeSet;
    changeSet.fillFromCurrentChanges(rebasedArguments(), needHunks);

    changeSet.waitForFinishFirstFile();
    bool shouldDoRecord = changeSet.count() > 0;
    if (!shouldDoRecord) {
        Logger::warn() << "No changes!" << endl;
        return Ok;
    }

    QString email = m_author;
    if (email.isEmpty())
        email = getenv("EMAIL");
    QStringList environment;
    if (! email.isEmpty()) {
        QRegExp re("(.*) <([@\\S]+)>");
        if (re.exactMatch(email)) { // meaning its an email AND name
            environment << "GIT_AUTHOR_NAME="+ re.cap(1);
            environment << "GIT_COMMITTER_NAME="+ re.cap(1);
            environment << "GIT_AUTHOR_EMAIL="+ re.cap(2);
            environment << "GIT_COMMITTER_EMAIL="+ re.cap(2);
        }
        else if (m_author.isEmpty()) { // if its an account or shell wide option; just use the git defs.
            environment << "GIT_AUTHOR_EMAIL="+ email;
            environment << "GIT_COMMITTER_EMAIL="+ email;
        }
        else {
            Logger::error() << "Author format invalid. Please provide author formatted like; `name <email@host>\n";
            return InvalidOptions;
        }
    }

    if (shouldDoRecord && !m_all && m_mode != Index) { // then do interview
        HunksCursor cursor(changeSet);
        cursor.setConfiguration(m_config);
        Interview interview(cursor, name());
        interview.setUsePager(shouldUsePager());
        if (! interview.start()) {
            Logger::warn() << "Cancelled." << endl;
            return UserCancelled;
        }
    }

    if (shouldDoRecord && !m_all && m_mode != Index) { // check if there is anything selected
        shouldDoRecord = changeSet.hasAcceptedChanges();
        if (! shouldDoRecord) {
            Logger::warn() << "Ok, if you don't want to record anything, that's fine!" <<endl;
            return UserCancelled;
        }
    }
    if (dryRun())
        return Ok;

    if ((m_editComment || m_patchName.isEmpty()) && getenv("EDITOR")) {
        class Deleter : public QObject {
        public:
            Deleter() : commitMessage(0) { }
            ~Deleter() {
                if (commitMessage)
                    commitMessage->remove();
            }
            QFile *commitMessage;
        };
        Deleter parent;
        QFile *commitMessage;
        int i = 0;
        do {
            commitMessage = new QFile(QString("vng-record-%1").arg(i++), &parent);
        } while (commitMessage->exists());
        parent.commitMessage = commitMessage; // make sure the file is removed from FS.
        if (! commitMessage->open(QIODevice::WriteOnly)) {
            Logger::error() << "Vng failed. Could not create a temporary file for the record message '"
                << commitMessage->fileName() << "`\n";
            return WriteError;
        }
        const char * defaultCommitMessage1 = "\n***END OF DESCRIPTION***"; // we will look for this string later
        const char * defaultCommitMessage2 = "\nPlace the long patch description above the ***END OF DESCRIPTION*** marker.\n\nThis patch contains the following changes:\n\n";
        if (! m_patchName.isEmpty())
            commitMessage->write(m_patchName);
        else
            commitMessage->write("\n", 1);
        commitMessage->write(defaultCommitMessage1, strlen(defaultCommitMessage1));
        commitMessage->write(defaultCommitMessage2, strlen(defaultCommitMessage2));
        QBuffer buffer;
        changeSet.writeDiff(buffer, m_all ? ChangeSet::AllHunks : ChangeSet::UserSelection);
        ChangeSet actualChanges;
        actualChanges.fillFromDiffFile(buffer);
        QTextStream out (commitMessage);
        for (int i=0; i < actualChanges.count(); ++i) {
            File file = actualChanges.file(i);
            file.outputWhatsChanged(out, m_config, true, false);
        }
        for (int i=0; i < changeSet.count(); ++i) {
            File file = changeSet.file(i);
            if (file.isBinary() && (m_all || file.binaryChangeAcceptance() == Vng::Accepted))
                out << "M " << QString::fromLocal8Bit(file.fileName());
            else if (file.fileName().isEmpty() && (m_all || file.renameAcceptance() == Vng::Accepted))
                out << "D " << QString::fromLocal8Bit(file.oldFileName());
        }
        out.flush();

        commitMessage->close();
        QDateTime modification = QFileInfo(*commitMessage).lastModified();
        QString command = QString("%1 %2").arg(getenv("EDITOR")).arg(commitMessage->fileName());
        int rc = system(command.toAscii().data());
        if (rc != 0) {
            // this will keep patchName empty and we fall through to the interview.
            Logger::warn() << "Vng-Warning: Could not start editor '" << getenv("EDITOR") << "`\n";
            Logger::warn().flush();
        }
        else if (modification == QFileInfo(*commitMessage).lastModified()) {
            Logger::warn() << "unchanged, won't record\n";
            return UserCancelled;
        }
        else {
            // get data until the separator line.
            commitMessage->open(QIODevice::ReadOnly);
            m_patchName = commitMessage->readAll();
            commitMessage->close();
            int cuttoff = m_patchName.indexOf(defaultCommitMessage1);
            if (cuttoff > 0)
                m_patchName.truncate(cuttoff);
            for (int i = m_patchName.length()-1; i >= 0; --i) {
                if (m_patchName[i] == '\n' || m_patchName[i] == '\r' || m_patchName[i] == ' ')
                    m_patchName.resize(i); // shrink
                else break;
            }
        }
    }
    if (m_patchName.isEmpty())
        m_patchName = Interview::ask("What is the patch name? ").toUtf8();

    ReturnCodes rc = addFilesPerAcceptance(changeSet, m_all);
    if (rc != Ok)
        return rc;

    QProcess git;
    QStringList arguments;
    arguments << "write-tree";
    GitRunner runner(git, arguments);
    rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc != Ok) {
        Logger::error() << "Git write-tree failed!, aborting record\n";
        return rc;
    }
    char buf[1024];
    Vng::readLine(&git, buf, sizeof(buf));
    QString tree(buf);
    git.waitForFinished(); // patiently wait for it to finish..
    Logger::debug() << "The tree got git ref; " << tree;
    Logger::debug().flush(); // flush since we do an ask next

    arguments.clear();
    git.setEnvironment(environment);

    arguments << "commit-tree" << tree.left(40);
    if (!m_config.isEmptyRepo())
        arguments << "-p" << "HEAD" ;

    runner.setArguments(arguments);
    rc = runner.start(GitRunner::WaitUntilReadyForWrite);
    if (rc != Ok) {
        Logger::error() << "Git commit-tree failed!, aborting record\n";
        return rc;
    }
    git.write(m_patchName);
    git.write("\n");
    git.closeWriteChannel();
    Vng::readLine(&git, buf, sizeof(buf));
    QString commit(buf);
    Logger::debug() << "commit is ref; " << commit;
    git.waitForFinished(); // patiently wait for it to finish..
    if (commit.isEmpty()) {
        Logger::error() << "Git update-ref failed to produce a reference!, aborting record\n";
        return GitFailed;
    }
    m_sha1 = commit.left(40);

    arguments.clear();
    arguments << "update-ref" << "HEAD" << m_sha1;
    runner.setArguments(arguments);
    rc = runner.start(GitRunner::WaitUntilFinished);
    if (rc != Ok) {
        Logger::error() << "Git update-ref failed!, aborting record\n";
        return rc;
    }

    // We removed files from the index in case they were freshly added, but the user didn't want it in this commit.
    // we have to re-add those files.
    arguments.clear();
    arguments << "update-index" << "--add";
    for (int i=0; i < changeSet.count(); ++i) {
        File file = changeSet.file(i);
        if (! file.oldFileName().isEmpty())
            continue; // not a new added file.
        if (file.renameAcceptance() == Vng::Rejected)
            arguments.append(file.fileName());
    }
    if (arguments.count() > 2) {
        runner.setArguments(arguments);
        runner.start(GitRunner::WaitUntilFinished);
    }

    int endOfLine = m_patchName.indexOf('\n');
    if (endOfLine > 0)
        m_patchName.truncate(endOfLine);
    Logger::warn() << "Finished recording patch `" << m_patchName << "'" << endl;
    return Ok;
}

AbstractCommand::ReturnCodes Record::addFilesPerAcceptance(const ChangeSet &changeSet, bool allChanges)
{
    typedef QPair<QString, QString> NamePair;
    class RevertCopier {
      public:
        RevertCopier() {}
        ~RevertCopier() {
            foreach(NamePair pair, m_fileNames) {
                QFile copy(pair.second);
                copy.remove();
                if (! pair.first.isEmpty()) {
                    QFile orig(pair.first);
                    orig.rename(copy.fileName());
                }
            }
        }
        void append(const QString &orig, const QString &copy) {
            QPair<QString, QString> pair(orig, copy);
            m_fileNames.append(pair);
        }
      private:
        // thats orig, copy
        QList< QPair<QString, QString> > m_fileNames;
    };
    RevertCopier reverter;// this will revert all file changes we make when we exit the scope of this method.
    ChangeSet patchChanges;

    QStringList filesForAdd;
    QStringList notUsedFiles;
    for (int i=0; i < changeSet.count(); ++i) {
        File file = changeSet.file(i);
        bool someEnabled = false;
        bool allEnabled = true;
        const bool renamed = file.fileName() != file.oldFileName();
        const bool protectionChanged = !renamed && file.protection() != file.oldProtection();
        if ((file.renameAcceptance() == Vng::Accepted && renamed)
            || (file.protectionAcceptance() == Vng::Accepted && protectionChanged)) // record it.
            someEnabled = true;

        foreach (Hunk hunk, file.hunks()) {
            Vng::Acceptance a = hunk.acceptance();
            if (a == Vng::Accepted) {
                someEnabled = true;
            } else if (a == Vng::MixedAcceptance) {
                someEnabled = true;
                allEnabled = false;
            } else {
                allEnabled = false;
            }
            if (someEnabled && !allEnabled)
                break;
        }
        const bool addUnaltered = allChanges || allEnabled;

        const bool removeFromIndex = !allChanges && !someEnabled; // user said 'no', lets make sure none of the changes are left in the index.
        if (removeFromIndex && ! file.fileName().isEmpty() && ! file.oldFileName().isEmpty()) { // just ignore file.
            notUsedFiles << file.fileName();
            continue;
        }
        Logger::debug() << "'" << file.fileName() << "` addUnaltered: " << addUnaltered << ", removeFromIndex: "
            << removeFromIndex << ", someChangesAccepted: " << someEnabled << ", allAccepted: " << allEnabled << endl;
        if (file.fileName().isEmpty())
            filesForAdd << file.oldFileName();
        else {
            filesForAdd << file.fileName();
            if (renamed)
                filesForAdd << file.oldFileName();
        }

        if (addUnaltered)
            continue; // thats easy; whole file to add.
        if (removeFromIndex && file.fileName().isEmpty()) { // user said no about recording a deleted file.
            // this is a funny situation; *just* in case the user already somehow added the deletion to the index
            // we need to reset that to make the index have the full file again.  Notice that we need to remove the file afterwards.
            Q_ASSERT(!file.oldFileName().isEmpty());
            QProcess git;
            QStringList arguments;
            arguments << "cat-file" << "blob" << file.oldSha1();
            GitRunner runner(git, arguments);
            ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
            Logger::debug() << "restoring '" << file.oldFileName() << "`\n";
            if (rc == Ok) {
                reverter.append(QString(), file.oldFileName());
                QFile deletedFile(file.oldFileName());
                Q_ASSERT(! deletedFile.exists());
                bool success = Vng::copyFile(git, deletedFile);
                git.waitForFinished();
                deletedFile.close();
                if (! success)
                    return WriteError;
            }
            continue;
        }

        // for the case where only some patches are selected we make a safety copy of the file.
        QFile sourceFile(file.fileName());
        Q_ASSERT(sourceFile.exists());
        QString fileName = file.fileName();
        for(int i=0; i < 10; i++) {
            fileName = fileName + ".orig";
            if (sourceFile.rename(fileName))
                break; // successful!
        }
        reverter.append(fileName, file.fileName());
        if (removeFromIndex) // need to rename it only, we won't patch it.
            continue;

        patchChanges.addFile(file);
        Logger::debug() << "cp " << fileName << " =>" << file.fileName() << endl;
        QFile orig(fileName);
        orig.open(QIODevice::ReadOnly);
        Vng::copyFile(orig, sourceFile); // TODO check return code
        sourceFile.setPermissions(file.permissions());
    }

    QFile patch(".vng.record." + QString::number(getpid()) + ".diff");
    patchChanges.writeDiff(patch, ChangeSet::InvertedUserSelection);
    patch.close();

    if (patch.size() != 0) {
        QProcess git;
        QStringList arguments;
        arguments << "apply" << "--apply" <<  "--reverse" << patch.fileName();
        GitRunner runner(git, arguments);
        ReturnCodes rc = runner.start(GitRunner::WaitUntilFinished);
        if (rc != Ok) {
            Logger::error() << "Vng failed: failed to patch, sorry! Aborting record.\n";
            return rc; // note that copied files will be moved back to avoid partially patched files lying around.
        }
    }
    patch.remove();
    // first clean out the index so that previous stuff other tools did doesn't influence us.
    if (!m_config.isEmptyRepo() && ! notUsedFiles.isEmpty()) {
        QProcess git;
        QStringList arguments;
        arguments << "reset" << "--mixed" << "-q" << "HEAD"; // -q stands for quiet.
        arguments += notUsedFiles;
        GitRunner runner(git, arguments);
        runner.start(GitRunner::WaitUntilFinished);
    }

    // git add of all files to get a nice list of changes into the index.
    while (!filesForAdd.isEmpty()) {
        QProcess git;
        QStringList arguments;
        arguments << "update-index" << "--add" << "--remove";
        int count = 25; // length of arguments
        do {
            QString first = filesForAdd[0];
            if (count + first.length() > 32000)
                break;
            count += first.length();
            arguments.append(first);
            filesForAdd.removeFirst();
        } while (!filesForAdd.isEmpty());
        GitRunner runner(git, arguments);
        ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
        if (rc != Ok) {
            Logger::error() << "Vng failed: Did not manage to add files to the git index, aborting record\n";
            return rc;
        }
    }

    return Ok; // exiting scope will revert all files in the local filesystem.  We use the index from here on.
}

