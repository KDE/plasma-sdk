/******************************************************************************
 * Copyright (C) 2009 by Diego '[Po]lentino' Casella <polentino911@gmail.com> *
 * Copyright (C) 2013 by Giorgos Tsiapaliokas <terietor@gmail.com>            *
 *                                                                            *
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

#ifndef DVCSJOB_H
#define DVCSJOB_H

#include <QByteArray>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QVariant>

#include <KDebug>
#include <KJob>
#include <klocalizedstring.h>
#include <KProcess>

/**
  * This class is used to execute dvcs jobs.
  * Result can be obtained by calling output() or rawOutput(), depending if you
  * want to retrieve a string or binary data.
  * To use it at its most, some setup is required; here it is an example:
  * @code
  *     KJob *job = new DvcsJob(command, workingDirectory);
  *     connect(job, SIGNAL(result(KJob*)), this, SLOT(handleResult(KJob*)));
  *     job->start();
  * @endcode
  */
class DvcsJob : public KJob
{
    Q_OBJECT

public:
    /**
      * Used to identify the current status of the class.
      */
    enum {
        JobFailedToStart = UserDefinedError,
        JobCrashed,
        JobTimedout,
        JobWriteError,
        JobReadError,
        JobUnknownError
    };

    /**
      * Simple ctor
      */
    DvcsJob(const QStringList& command, const QString& workingDirectory, QObject *parent = 0);
    ~DvcsJob();

    /**
      * Starts the process with the previously defined arguments.
      */
    void start();

    /**
      * @return The handle to the current process reference.
      */
    KProcess *childproc();

    /**
      * @return The current working directory.
      */
    QString workingDirectory() const;

    /**
      * @return The result of the last process executed as binary data
      * @see DvcsJob::output()
      */
    QByteArray rawOutput() const;

    /**
      * @return The result of the last process executed as a string.
      */
    QString output() const;

    /**
     * @return The command that we executed
     */
    QString dvcsCommand() const;
protected:

    /**
      * Kills the current process.
      */
    bool doKill();

private Q_SLOTS:

    /**
      * Helper function, prints to the standard error if something goes wrong
      * @param error The QProcess::ProcessError error identifier.
      */
    void slotProcessError(QProcess::ProcessError error);

    /**
      * Helper function, writes a debug string accordingly with the exit code/status.
      * @param exitCode The integer exit code.
      * @param exitStatus The QProcess::ExitStatus exit code.
      */
    void slotProcessExited(int exitCode, QProcess::ExitStatus exitStatus);

    /**
      * Function called whenever the job has available data on standard output.
      */
    void slotReceivedStdout();

    /**
      * Function called whenever the job has available data on standard output.
      */
    void slotReceivedStderr();

    void doWork();

private:
    KProcess   *m_process;
    QString     m_directory;
    QByteArray  m_output;

    //we will use m_dvcsCommand just for debugging purposes
    QStringList m_dvcsCommand;
};

#endif // DVCSJOB_H
