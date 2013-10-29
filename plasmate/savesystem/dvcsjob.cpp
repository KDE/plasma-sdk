/******************************************************************************
 * Copyright (C) 2009 by Diego '[Po]lentino' Casella <polentino911@gmail.com> *
 * Copyright (C) 2013 by Giorgos Tsiapaliokas <terietor@gmail.com>            *
 *    This program is free software; you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation; either version 2 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful, but     *
 *    WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 *    General Public License for more details.                                *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program; if not, write to the Free Software Foundation  *
 *    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA        *
 *                                                                            *
 ******************************************************************************/

#include <QTimer>

#include "dvcsjob.h"

DvcsJob::DvcsJob(const QStringList& command, const QString& workingDirectory, QObject *parent)
        :KJob(parent)
{
    m_process = new KProcess(this);
    m_process->setOutputChannelMode(KProcess::SeparateChannels);
    m_process->setProgram(command);

    m_dvcsCommand = command;
    m_directory = workingDirectory;

    m_process->setWorkingDirectory(workingDirectory);
    m_process->setEnvironment(QProcess::systemEnvironment());
}

DvcsJob::~DvcsJob()
{
    if (m_process) {
        delete m_process;
    }

    m_process = 0;
}

QString DvcsJob::workingDirectory() const
{
    return m_directory;
}

QString DvcsJob::output() const
{
    QByteArray stdoutbuf = rawOutput();

    return QString::fromLocal8Bit(stdoutbuf, stdoutbuf.size());
}

QByteArray DvcsJob::rawOutput() const
{
    return m_output;
}

void DvcsJob::start()
{
    QTimer::singleShot(0, this, SLOT(doWork()));
}

void DvcsJob::doWork()
{
    // Processing the signal result
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(slotProcessExited(int, QProcess::ExitStatus)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(slotProcessError(QProcess::ProcessError)));
    connect(m_process, SIGNAL(readyReadStandardError()),
            this, SLOT(slotReceivedStderr()));
    connect(m_process, SIGNAL(readyReadStandardOutput()),
            this, SLOT(slotReceivedStdout()));

    m_output.clear();
    //the started() and error() signals may be delayed! It causes crash with deferred deletion!!!
    m_process->start();
}

bool DvcsJob::doKill()
{
    m_process->kill();
    return true;
}

void DvcsJob::slotProcessError(QProcess::ProcessError error)
{
    // disconnect all connections to childproc's signals; they are no longer needed
    m_process->disconnect();

    switch (error) {
    case QProcess::FailedToStart:
        setError(DvcsJob::JobFailedToStart);
        break;
    case QProcess::Crashed:
        setError(DvcsJob::JobCrashed);
        break;
    case QProcess::Timedout:
        setError(DvcsJob::JobTimedout);
        break;
    case QProcess::WriteError:
        setError(DvcsJob::JobWriteError);
        break;
    case QProcess::ReadError:
        setError(DvcsJob::JobReadError);
        break;
    case QProcess::UnknownError:
        setError(DvcsJob::JobUnknownError);
        break;
    }

    setErrorText(i18n("Process exited with status %1 and exit code %2", error, m_process->exitCode()));

    emitResult();
}

void DvcsJob::slotProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
{
    // disconnect all connections to childproc's signals; they are no longer needed
    m_process->disconnect();

    if (exitStatus != QProcess::NormalExit || exitCode != 0 ) {
        slotProcessError(QProcess::UnknownError);
    }

    kDebug() << "process has finished with no errors";
    emitResult();
}

void DvcsJob::slotReceivedStdout()
{
    // accumulate output
    m_output.append(m_process->readAllStandardOutput());
}

void DvcsJob::slotReceivedStderr()
{
    // accumulate output
    m_output.append(m_process->readAllStandardError());
}

KProcess* DvcsJob::childproc()
{
    return m_process;
}

QString DvcsJob::dvcsCommand() const
{
    return m_dvcsCommand.join(" ");
}

#include "moc_dvcsjob.cpp"
