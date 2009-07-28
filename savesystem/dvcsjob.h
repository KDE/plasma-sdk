#ifndef DVCSJOB_H
#define DVCSJOB_H

#include    <klocalizedstring.h>
#include    <KJob>
#include    <KDebug>
#include    <KProcess>
#include    <QStringList>
#include    <QVariant>
#include    <QString>
#include    <QDir>
#include    <QByteArray>


class DvcsJob : public KJob
{
    Q_OBJECT

public:

    enum JobStatus {
        JobRunning = 0      /**< The job is running */,
        JobSucceeded = 1    /**< The job succeeded */,
        JobCancelled = 2    /**< The job was cancelled */,
        JobFailed = 3       /**< The job failed */,
        JobNotStarted = 4   /**< The job is not yet started */
    };


    DvcsJob();
    ~DvcsJob();

    void clear();

    void setDirectory(const QDir & directory);

    void setStandardInputFile(const QString &fileName);

    void setCommandArguments(const QStringList &args);

    QDir const & getDirectory() const;

    DvcsJob& operator<<(const QString& arg);

    DvcsJob& operator<<(const char* arg);

    DvcsJob& operator<<(const QStringList& args);

    void start();

    void setCommunicationMode(KProcess::OutputChannelMode comm);

    QString dvcsCommand() const;

    QString output() const;

    QByteArray rawOutput() const;

    void setResults(const QVariant &res);

    QVariant fetchResults();

    void setExitStatus(const bool exitStatus);

    JobStatus status() const;

    KProcess *getChildproc();

public Q_SLOTS:
    void cancel();

    bool isRunning() const;

Q_SIGNALS:
    void readyForParsing(DvcsJob *job);
    void resultsReady(DvcsJob *job);

private Q_SLOTS:
    void slotProcessError(QProcess::ProcessError);
    void slotProcessExited(int exitCode, QProcess::ExitStatus exitStatus);
    void slotReceivedStdout();
    void slotReceivedStderr();

private:
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
