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

#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

#include "Configuration.h"

#include <QString>
#include <QProcess>
#include <QDir>
#include <QTextStream>

/**
 * This baseclass can be used to inherit from in order to add new top-level commands to vng.
 * Top level commands are things like 'whatsnew' and 'add'.
 * @see CommandLineParser
 */
class AbstractCommand {
public:
    /// an enum of return codes to be used by start() and run() so all commands syncronize their codes.
    enum ReturnCodes {
        // note that Unix requires 'Ok' to be zero.
        Ok = 0,
        InvalidOptions = 1,
        Disabled,
        NotInRepo,
        WriteError,
        OtherVngError,
        UserCancelled,
        GitCrashed = 10,
        GitFailed,
        GitTimedOut
    };

    /**
     * Constructor.
     * @param name the name this command has. Used (o.a.) to create a Configuration object.
     */
    AbstractCommand(const QString &name);
    virtual ~AbstractCommand();

    /**
     * Start the execution of the commmand.
     * This will setup various items and read the default options after which it will call run().
     * @return the exit code as used by the comamnd.
     * @see ReturnCodes
     */
    ReturnCodes start();

    /// return the name of this command.
    QString name() const;

protected:
    /**
     * return the short visual description of the arguments you can pass to this command.
     * If the command takes arguments that are not options, you can make the help message print that
     * by returning something in this method.
     */
    virtual QString argumentDescription() const = 0;

    /**
     * return long (multiline) Description of the command.
     */
    virtual QString commandDescription() const = 0;

    /**
     * Run the commmand from the repository root.
     * @return the exit code as used by the comamnd.
     * @see ReturnCodes
     */
    virtual ReturnCodes run() = 0;

    /// returns the repository we are in.
    QDir repository() const;
    /// returns if the command is a dry run which means that user interaction should take place, but no files changed.
    bool dryRun() const;

    /// The configuation object for this command.
    Configuration m_config;

    /**
     * Changes the arguments passed like they are paths to compensate for the repodir.
     * In vng all commands work on the whole repo, which is accomplished by changing the current
     * directory to the repo root before run() is called.
     * In case the user passed relative path arguments the change in current directory needs to be
     * reflected in the paths.
     * This method will take all the arguments and assume they are paths. For relative paths (not
     * starting with slash or a drive name) the change in directory will be applied.
     */
    QStringList rebasedArguments() const;

    /// check if we are in a repository; return true if we are
    bool checkInRepository() const;

    bool shouldUsePager() const;

    enum RebaseOptionFlag {
        NoOption = 0,       ///< No options
        /// For each command line argument, do a stat to find if the original file or the rebased file is meant.
        CheckFileSystem = 1,
        /// In case the rebased filename does not exist, print an error message saying so.
        PrintError = 2
    };
    Q_DECLARE_FLAGS( RebaseOptions, RebaseOptionFlag )

    /// make the current directory for vng the root of the repo. This will also populate the m_rebasedArguments list.
    void moveToRoot(RebaseOptions options = NoOption);

private:
    QDir m_repository;
    bool m_dryRun;
    QString m_name;
    QStringList m_rebasedArguments;
};

#endif
