/******************************************************************************
 * Copyright (C) 2009 by Diego '[Po]lentino' Casella <polentino911@gmail.com> *
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
  *     DvcsJob *job = new DvcsJob();
  *     job->setCommunicationMode(KProcess::SeparateChannels);
  *     job->setDirectory(QDir("a/directory"));
  *
  *     job << "git" << "init";
  *     job->start();
  *
  *     QString result;
  *     QString error;
  *     if(job->status() == DvcsJob::JobSucceeded) {
  *         // Save job result as a string
  *         result = job->output();
  *         // Process the output
  *
  *     } else if(job->status() == DvcsJob::JobFailed) {
  *         error = job->output();
  *         // Process the error
  *     }
  *
  *     job->cancel();
  * @endcode
  *
  * @author Diego [Po]lentino Casella <polentino911@gmail.com>
  *
  * @note There is no need to cycle until the process has finished, since the main
  * thread is blocked until start() has finished. For particular implementations,
  * consider putting the job in a separate KThread and query isRunning().
  */
class DvcsJob : public KJob
{
    Q_OBJECT

public:
    /**
      * Used to identify the current status of the class.
      */
    enum JobStatus {
        JobRunning = 0      /**< The job is running */,
        JobSucceeded = 1    /**< The job succeeded */,
        JobCancelled = 2    /**< The job was cancelled */,
        JobFailed = 3       /**< The job failed */,
        JobNotStarted = 4   /**< The job is not yet started */
    };

    /**
      * Simple ctor
      */
    DvcsJob();
    ~DvcsJob();

    /**
      * Starts the process with the previously defined arguments.
      */
    void start();

    /**
      * Call this method to clean the job, for example before setting a new one.
      */
    void clear();

    /**
      * @return The status of the last process executed.
      */
    JobStatus status() const;

    /**
      * Sets the command arguments.
      * @param Args contains one string argument per element.
      */
    void setCommandArguments(const QStringList &args);

    /**
      * Sets the communication mode.
      * @param comm Accepts one of the KProcess:OutputChannelMode modes.
      */
    void setCommunicationMode(KProcess::OutputChannelMode comm);

    /**
      * Sets the process working directory.
      * @param directory Should contain only absolute path; relative or "" paths are
      * deprecated and will make the job fail.
      */
    void setDirectory(const QDir &directory);

    /**
      * Sets a file used to write the output in.
      * @param fileName The name of the file.
      */
    void setStandardInputFile(const QString &fileName);

    /**
      *@return The string containing the current commads into a single string.
      */
    QString dvcsCommand() const;

    /**
      * @return The handle to the current process reference.
      */
    KProcess *getChildproc();

    /**
      * @return The current working directory.
      */
    QDir const & getDirectory() const;

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
      * Append a new command to the existing command list.
      * @param arg The string representing the command argument.
      */
    DvcsJob& operator<<(const QString &arg);

    /**
      * Append a new command to the existing command list.
      * @param arg The char string representing the command argument.
      */
    DvcsJob& operator<<(const char *arg);

    /**
      * Append a new list of commands to the existing command list.
      * @param arg The stringlist representing the command arguments.
      */
    DvcsJob& operator<<(const QStringList &args);

    /**
      * Cancels the current process.
      */
    void cancel();

private Q_SLOTS:

    /**
      * Helper function, prints to the standard error if something goes wrong and
      * reset the DvcsJob status to ready.
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

private:

    /**
      * @return whehter the job is running or not.
      */
    bool isRunning() const;

    KProcess   *m_process;
    QStringList m_command;
    QVariant    m_results;
    QDir        m_directory;
    bool        m_isRunning;
    bool        m_wasStarted;
    bool        m_failed;
    QByteArray  m_output;
    KProcess::OutputChannelMode m_comm;

    void jobIsReady();
};

#endif // DVCSJOB_H
