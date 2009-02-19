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

#include "Push.h"
#include "CommandLineParser.h"
#include "Logger.h"
#include "GitRunner.h"
#include "Vng.h"

#include <QProcess>
#include <QDebug>
#include <QRegExp>

static const CommandLineOption options[] = {
    // TODO :)
    // {"--matches=PATTERN", "select patches matching PATTERN"},
    // {"-p REGEXP, --patches=REGEXP", "select patches matching REGEXP"},
    // {"-t REGEXP, --tags=REGEXP", "select tags matching REGEXP"},
    // {"-a, --all", "answer yes to all patches"},
    // {"-i, --interactive", "prompt user interactively"},
    // {"-s, --summary", "summarize changes"},
    // {"--no-summary", "don't summarize changes"},
    {"--set-default", "set default repository [DEFAULT]"},
    {"--no-set-default", "don't set default repository"},
    CommandLineLastOption
};

Push::Push()
    : AbstractCommand("Push")
{
    CommandLineParser::addOptionDefinitions(options);
    CommandLineParser::setArgumentDefinition("push [Repository]" );
}

AbstractCommand::ReturnCodes Push::run()
{
    if (! checkInRepository())
        return NotInRepo;
    CommandLineParser *args = CommandLineParser::instance();

    QProcess git;
    QStringList arguments;
    arguments << "config" << "-l";
    GitRunner runner(git, arguments);
    AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc != Ok) {
        Logger::error() << "Vng failed; could not read configuration\n";
        return rc;
    }

    QRegExp re("remote\\.(.*)url=(.*)\n");
    QHash<QString, QString> repoNames;
    char buf[1024];
    while (true) { //read relevant lines into repoNames hash
        qint64 length = Vng::readLine(&git, buf, sizeof(buf));
        if (length < 0)
            break;
        QString line(buf);
        if (re.exactMatch(line)) {
            QString context = re.cap(1);
            if (context.endsWith("."))
                context = context.left(context.length()-1);
            repoNames.insert(context, re.cap(2));
        }
    }

    QString repo;
    if (args->arguments().count() > 1)
        repo = args->arguments()[1];

    QString url = repo;
    if (repo.isEmpty())
        repo = "default"; // the name we store the default as.
    if (repoNames.contains(repo))
        url = repoNames[repo];
    if (url.isEmpty()) { // print help
        Logger::standardOut() << "Vng failed: Missing argument [REPOSITORY]\n";
        Logger::standardOut().flush();
        args->usage(name(), argumentDescription());
        Logger::standardOut() << endl << commandDescription();
        return Ok;
    }

    Logger::warn() << "Pushing to `" << url << "'\n";
    Logger::warn().flush(); // make sure its printed before git asks for an ssh pwd.
    if (dryRun())
        return Ok;

    // TODO when not using --all ask the remote for all the refs it has and detect which ones we still have to push
    // TODO use interview to ask which refs to push instead of all below
    arguments.clear();
    arguments << "push" << url;
    runner.setArguments(arguments);
    rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc != Ok) {
        Logger::error() << "Git push failed\n";
        return rc;
    }

/* TODO for some reason qprocess doesn't give me any output if the child process already finished :(
    while (true) {
        qint64 length = Vng::readLine(&git, buf, sizeof(buf));
        if (length < 0)
            break;
        Logger::info() << QString(buf);
    }
*/
    git.waitForFinished(-1);

    if ( !(args->contains("no-set-default")
            || (m_config.contains("no-set-default") && ! args->contains("set-default")))
            && repo == url) { // lets set as default
        arguments.clear();
        arguments << "config" << "--add" << "remote.default.url" << repo;
        runner.setArguments(arguments);
        rc = runner.start(GitRunner::WaitUntilFinished);
        if (rc != Ok) {
            Logger::warn() << "Could not store the default value for future reuse\n";
            return rc;
        }
    }

    return Ok;
}

QString Push::argumentDescription() const
{
    return "[REPOSITORY]";
}

QString Push::commandDescription() const
{
    return "Push is the opposite of pull.  Push allows you to copy changes from the\n"
    "current repository into another repository.\n";
}
