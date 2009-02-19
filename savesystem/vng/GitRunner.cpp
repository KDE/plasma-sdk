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
#include "GitRunner.h"
#include "Logger.h"

#include <QDebug>

GitRunner::GitRunner(QProcess &process, const QStringList &arguments)
    : m_process(&process),
    m_arguments(arguments)
{
}

AbstractCommand::ReturnCodes GitRunner::start(WaitCondition condition, Fail fail)
{
    Q_ASSERT(condition <= WaitUntilReadyForWrite);
    if (Logger::verbosity() >= Logger::Debug) {
        Logger::debug() << "command:  git";
        foreach(QString arg, m_arguments)
            Logger::debug() << " " << arg;
        Logger::debug() << endl;
        Logger::debug().flush();
    }

    if (condition == WaitForStandardOutput)
        m_process->setReadChannel(QProcess::StandardOutput);
    else if (condition == WaitForStandardError) {
        m_process->setReadChannel(QProcess::StandardError);
        condition = WaitForStandardOutput;
    }

    QIODevice::OpenMode mode = QIODevice::ReadOnly;
    if (condition == WaitUntilReadyForWrite)
        mode |= QIODevice::WriteOnly;

    m_process->start("git", m_arguments, mode);
    if ((condition == WaitForStandardOutput && !m_process->waitForReadyRead())
        || (condition == WaitUntilFinished && !m_process->waitForFinished())
        || (condition == WaitUntilReadyForWrite && !m_process->waitForStarted())) {
        if (m_process->state() != QProcess::NotRunning) { // time out
            Logger::error() << "ERROR: the backend (git) fails to start, timing out" << endl;
            m_process->kill();
            return AbstractCommand::GitTimedOut;
        }
        if (m_process->exitStatus() == QProcess::NormalExit) {
            if (m_process->exitCode() != 0) {
                if (fail == WarnOnFail)
                    Logger::error() << "ERROR: Failed calling git" << endl; // we may have to change this to debug later.
                Logger::debug() << "   we got return code:" << m_process->exitCode() << endl;
                return AbstractCommand::GitFailed;
            }
            return AbstractCommand::Ok;
        }
        Logger::error() << "ERROR: the backend (git) crashed" << endl;
        return AbstractCommand::GitCrashed;
    }
    if (m_process->exitCode() != 0) {
        Logger::debug() << "   we got return code:" << m_process->exitCode() << endl;
        return AbstractCommand::GitFailed;
    }
    return AbstractCommand::Ok;
}

void GitRunner::setArguments(const QStringList &arguments)
{
    m_arguments = arguments;
}
