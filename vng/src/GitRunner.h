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
#ifndef GITRUNNER_H
#define GITRUNNER_H

#include "AbstractCommand.h"

#include <QProcess>
#include <QStringList>

class GitRunner
{
public:
    enum WaitCondition {
        WaitForStandardOutput,
        WaitForStandardError,
        WaitUntilFinished,
        WaitUntilReadyForWrite
    };
    enum Fail {
        FailureAccepted, // if git returns with non-zero return code, thats ok.
        WarnOnFail      // Log an error if git returns with non-zero return code.
    };
    GitRunner(QProcess &process, const QStringList &arguments);
    AbstractCommand::ReturnCodes start(WaitCondition condition, Fail fail = WarnOnFail);

    void setArguments(const QStringList &arguments);

private:
    QProcess *m_process;
    QStringList m_arguments;
};

#endif
