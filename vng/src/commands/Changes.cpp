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
#include "Changes.h"
#include "CommandLineParser.h"
#include "GitRunner.h"
#include "Logger.h"
#include "Vng.h"
#include "hunks/ChangeSet.h"
#include "patches/Commit.h"
#include "patches/CommitsMatcher.h"

#include <QDebug>


static const CommandLineOption options[] = {
    // {"-a, --all", "answer yes to all patches"},
    {"--to-match PATTERN", "select changes up to a patch matching PATTERN"},
    {"--to-patch REGEXP", "select changes up to a patch matching REGEXP"},
    // {"--to-tag REGEXP", "select changes up to a tag matching REGEXP"},
    {"--from-match PATTERN", "select changes starting with a patch matching PATTERN"},
    {"--from-patch REGEXP", "select changes starting with a patch matching REGEXP"},
    // {"--from-tag REGEXP", "select changes starting with a tag matching REGEXP"},
    {"-n, --last NUMBER", "select the last NUMBER patches"},
    {"--match PATTERN", "select patches matching PATTERN"},
    {"-p, --patches REGEXP", "select patches matching REGEXP"},
    // {"-t, --tags=REGEXP", "select tags matching REGEXP"},
    // {"--context", "give output suitable for get --context"},
    // {"--xml-output", "generate XML formatted output"},
    // {"--human-readable", "give human-readable output"},
    {"-s, --summary", "summarize changes"},
    {"--no-summary", "don't summarize changes"},
    // {"--reverse", "show changes in reverse order"},
    CommandLineLastOption
};

Changes::Changes()
    : AbstractCommand("changes")
{
    CommandLineParser::addOptionDefinitions(options);
    CommandLineParser::setArgumentDefinition("changes [FILE or DIRECTORY]" );
}

QString Changes::argumentDescription() const
{
    return "[FILE or DIRECTORY]";
}

QString Changes::commandDescription() const
{
    return "Changes gives a changelog-style summary of the repository history.\n";
}

AbstractCommand::ReturnCodes Changes::run()
{
    if (! checkInRepository())
        return NotInRepo;
    CommandLineParser *args = CommandLineParser::instance();

    QProcess git;
    QStringList arguments;
    arguments << "whatchanged" << "--no-abbrev" << "--pretty=raw";

    if (args->contains("last"))
        arguments << "-n" << args->optionArgument("last");

    QList<int> usedArguments;
    usedArguments << 0;
    if (args->arguments().count() > 1) {
        foreach (Branch branch, m_config.allBranches()) {
            QString branchName = branch.branchName();
            if (branchName.endsWith("/HEAD"))
                continue;
            bool first = true;
            int index = -1;
            foreach (QString arg, args->arguments()) {
                index++;
                if (first) {
                    first = false; // skip command, args for this command are next.
                    continue;
                }
                if (branchName == arg || (branchName.endsWith(arg) && branchName[branchName.length() - arg.length() - 1] == '/')) {
                    arguments << branchName;
                    usedArguments << index;
                    break;
                }
            }
        }
    }

    // now we have to use the rest of the arguments the user passed.
    int index = 0;
    QStringList unknownArguments;
    foreach (QString arg, args->arguments()) {
        if (! usedArguments.contains(index++)) {
            arguments << arg;
            unknownArguments << arg;
        }
    }

    GitRunner runner(git, arguments);
    ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc) {
        Logger::error() << "Vng failed: Unknown branch or revision: ";
        foreach(QString arg, unknownArguments)
            Logger::error() <<  "`" << arg << "' ";;
        Logger::error() <<  endl;
        return rc;
    }
    if (shouldUsePager())
        Logger::startPager();

    const bool showSummery = (m_config.contains("summary") && !args->contains("no-summary")) || args->contains("summary");
    QTextStream &out = Logger::standardOut();
    CommitsMatcher matcher;
    while(true) {
        Commit commit = Commit::createFromStream(&git);
        if (! commit.isValid())
            break;
        switch(matcher.match(commit)) {
        case CommitsMatcher::SkipPatch: continue;
        case CommitsMatcher::ShowPatch: break;
        case CommitsMatcher::Exit:
            Logger::stopPager();
            git.terminate();
            git.waitForFinished();
            return Ok;
        }

        out << commit.commitTime().toString() << "  ";
        m_config.colorize(out);
        out << commit.author();
        m_config.normalColor(out);
        out << endl;
        if (commit.author() != commit.committer()) {
            out << " By ";
            m_config.colorize(out);
            out << commit.committer();
            m_config.normalColor(out);
            out << endl;
        }
        out << " ID " << commit.commitTreeIsm() << endl;
        out << commit.logMessage();
        if (Logger::verbosity() >= Logger::Verbose || showSummery) {
            ChangeSet cs = commit.changeSet();
            cs.generateHunks();
            if (cs.count())
                out << endl;
            for (int i=0; i < cs.count(); ++i) {
                File file = cs.file(i);
                if (file.oldFileName().isEmpty())
                    out <<"  A " << file.fileName();
                else if (file.fileName().isEmpty())
                    out <<"  D " << file.oldFileName();
                else
                    out <<"  M " << file.fileName();
                if (Logger::verbosity() < Logger::Verbose) {
                    if (file.linesRemoved() > 0)
                        out << " -" << file.linesRemoved();
                    if (file.linesAdded() > 0)
                        out << " +" << file.linesAdded();
                }
                out << endl;
                if (Logger::verbosity() >= Logger::Verbose)
                    file.outputWhatsChanged(out, m_config, false, false);
            }
        }
        out << endl;
        Logger::flushPager();
        if (! out.device()->isWritable()) { // output cancelled; lets kill git.
            git.kill();
            break;
        }
    }
    Logger::stopPager();
    git.waitForFinished();
    return Ok;
}

