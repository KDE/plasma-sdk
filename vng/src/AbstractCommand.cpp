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

#include "AbstractCommand.h"
#include "CommandLineParser.h"
#include "Logger.h"

#include <QDebug>

static const CommandLineOption options[] = {
    {"-v, --verbose", "give verbose output"},
    {"-q, --quiet", "suppress informational output"},
    {"--debug", "print debugging information"},
    {"--standard-verbosity", "neither verbose nor quiet output"},
    {"--repodir DIRECTORY", "specify the repository directory in which to run"},
    {"--dry-run", "don't actually take the action"},
    {"--disable", "disable this command"},
    {"-h, --help", "shows brief description of command and its arguments"},
    {"--use-pager", "use a pager to show the output screen by screen"},
    {"--no-pager", "don't use the pager when showing the output"},
    CommandLineLastOption
};

AbstractCommand::AbstractCommand(const QString &name)
    : m_config(name), m_dryRun(false), m_name(name)
{
    CommandLineParser::addOptionDefinitions(options);
}

AbstractCommand::~AbstractCommand()
{
}

AbstractCommand::ReturnCodes AbstractCommand::start()
{
    CommandLineParser *args = CommandLineParser::instance();

    if (m_config.contains("quiet"))
        Logger::setVerbosity(Logger::Quiet);
    else if (m_config.contains("verbose"))
        Logger::setVerbosity(Logger::Verbose);
    else if (m_config.contains("standard-verbosity"))
        Logger::setVerbosity(Logger::Chatty);
    else if (m_config.contains("debug"))
        Logger::setVerbosity(Logger::Debug);

    if (args->contains("quiet"))
        Logger::setVerbosity(Logger::Quiet);
    else if (args->contains("verbose"))
        Logger::setVerbosity(Logger::Verbose);
    else if (args->contains("standard-verbosity"))
        Logger::setVerbosity(Logger::Chatty);
    else if (args->contains("debug"))
        Logger::setVerbosity(Logger::Debug);

    if (args->contains("disable")) {
        Logger::warn() << "vng failed: Command `" << m_name << "' disabled with --disable option!" << endl;
        return Disabled;
    }
    if (m_config.contains("disable")) {
        Logger::warn() << "vng failed: Command `" << m_name << "' disabled by configuration!" << endl;
        return Disabled;
    }

    if (args->undefinedOptions().count()) {
        Logger::error() << "vng failed: ";
        bool first = true;
        foreach(QString option, args->undefinedOptions()) {
            if (! first)
                Logger::error() << "          : ";
            Logger::error() << "unrecognized option or missing argument for; `" << option << "'" << endl;
            first = false;
        }
        return InvalidOptions;
    }

    if (args->contains("help")) {
        args->usage(name(), argumentDescription());
        QString command = commandDescription();
        if (!command.isEmpty()) {
            Logger::standardOut() << endl;
            Logger::standardOut() << command;
        }
        return Ok;
    }

    if (args->parseErrors().count()) {
        Logger::error() << "vng failed:  ";
        foreach(QString e, args->parseErrors()) {
            Logger::error() << e << endl;
        }
        return InvalidOptions;
    }

    m_dryRun = args->contains("dry-run") || m_config.contains("dry-run");

    if (args->contains("repodir")) {
        m_repository = QDir(args->optionArgument("repodir"));
        QDir test = m_repository;
        if (! test.cd(".git")) {
            Logger::error() << "repository dir `" << args->optionArgument("repodir") << "' is not a repository" << endl;
            return InvalidOptions;
        }
    }
    else
        m_repository = m_config.repository();
    return run();
}

QDir AbstractCommand::repository() const
{
    return m_repository;
}

bool AbstractCommand::dryRun() const
{
    return m_dryRun;
}

QString AbstractCommand::name() const
{
    return m_name;
}

QStringList AbstractCommand::rebasedArguments() const
{
    return m_rebasedArguments;
}

bool AbstractCommand::checkInRepository() const
{
    if (! QDir(m_repository.absolutePath() + "/.git").exists()) {
        Logger::error() << "vng failed:  Unable to `" << m_name << "' here\n\n";
        Logger::error() << "You need to be in a repository directory to run this command.\n";
        return false;
    }
    return true;
}

bool AbstractCommand::shouldUsePager() const
{
    CommandLineParser *args = CommandLineParser::instance();
    return (m_config.contains("use-pager") && !args->contains("no-pager")) || args->contains("use-pager");
}

void AbstractCommand::moveToRoot(RebaseOptions options)
{
    QString currentDir = QDir::current().absolutePath();
    QString repoDir = m_repository.absolutePath();
    QDir::setCurrent(repoDir);
    if (! m_rebasedArguments.isEmpty())
        return;
    CommandLineParser *args = CommandLineParser::instance();
    if (currentDir != repoDir && currentDir.startsWith(repoDir)) {
        QString diff = currentDir.mid(repoDir.length() + 1);
        // calcuate rebased paths.
        bool first = true;
        foreach (QString arg, args->arguments()) {
            if (first) {
                first = false;
                continue; // first is the command name
            }
            QString target;
            if (arg == ".") // work around git bug that 'dir/.' doesn't work while 'dir' does work
                target = diff;
            else if (arg.startsWith('/')) //absolute path
                target = arg;
            else
                target = diff +'/'+ arg;
            if (options & CheckFileSystem) {
                QFile rebasedFile(target);
                if (rebasedFile.exists())
                    m_rebasedArguments << target;
                else {
                    m_rebasedArguments << arg;
                    if (options & PrintError) {
                        QFile file(arg);
                        if (!file.exists())
                            Logger::error() << "Error, unknown file " << arg << endl;
                    }
                }
            }
            else
                m_rebasedArguments << target;
        }
    }
    else
        m_rebasedArguments = args->arguments().mid(1);
}
