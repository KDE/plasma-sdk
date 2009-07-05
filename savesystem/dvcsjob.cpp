/*
 *	Class which represents the Distributed Version Control job.
 */

#include "dvcsjob.h"

DvcsJob::DvcsJob()
{
	m_process = new KProcess();
}

DvcsJob::~DvcsJob()
{

}

void DvcsJob::clear()
{
	m_command.clear();
	m_output.clear();
	m_directory = QDir::temp();
	m_isRunning = m_failed = m_wasStarted = false;
}



void DvcsJob::setDirectory(const QDir& directory)
{
	m_directory = directory;
}

void DvcsJob::setStandardInputFile(const QString &fileName)
{
	m_process->setStandardInputFile(fileName);
}

const QDir & DvcsJob::getDirectory() const
{
	return m_directory;
}

bool DvcsJob::isRunning() const
{
	return m_isRunning;
}

DvcsJob& DvcsJob::operator<<(const QString& arg)
{
	m_command.append( arg );
	return *this;
}

DvcsJob& DvcsJob::operator<<(const char* arg)
{
	m_command.append( arg );
	return *this;
}

DvcsJob& DvcsJob::operator<<(const QStringList& args)
{
	m_command << args;
	return *this;
}

QString DvcsJob::dvcsCommand() const
{
	return m_command.join(" ");
}

QString DvcsJob::output() const
{
	QByteArray stdoutbuf = rawOutput();
	int endpos = stdoutbuf.size();
	if (isRunning()) {    // We may have received only part of a code-point
		endpos = stdoutbuf.lastIndexOf('\n')+1; // Include the final newline or become 0, when there is no newline
	}

	return QString::fromLocal8Bit(stdoutbuf, endpos);
}

QByteArray DvcsJob::rawOutput() const
{
	return m_output;
}

void DvcsJob::setResults(const QVariant &res)
{
	m_results = res;
}

QVariant DvcsJob::fetchResults()
{
	return m_results;
}

void DvcsJob::setExitStatus(const bool exitStatus)
{
	m_failed = exitStatus;
}

void DvcsJob::start()
{
	m_wasStarted = true;
	QString workingDirectory = m_directory.absolutePath();
	m_process->setWorkingDirectory( workingDirectory );

	// Processing the signal result
	connect(m_process, SIGNAL(started()),
			this, SLOT(slotStarted()));
	connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
			this, SLOT(slotProcessExited(int, QProcess::ExitStatus)));
	connect(m_process, SIGNAL(error( QProcess::ProcessError )),
			this, SLOT(slotProcessError(QProcess::ProcessError)));
	connect(m_process, SIGNAL(readyReadStandardError()),
			this, SLOT(slotReceivedStderr()));
	connect(m_process, SIGNAL(readyReadStandardOutput()),
			this, SLOT(slotReceivedStdout()));

	m_output.clear();
	m_isRunning = true;
	m_process->setOutputChannelMode( comm );
	m_process->setProgram( m_command );
	m_process->setEnvironment(QProcess::systemEnvironment());
	//the started() and error() signals may be delayed! It causes crash with deferred deletion!!!
	m_process->waitForStarted();
	m_process->start();
}

void DvcsJob::setCommunicationMode(KProcess::OutputChannelMode comm)
{
	comm = comm;
}

void DvcsJob::cancel()
{
	m_process->kill();
}

void DvcsJob::slotProcessError( QProcess::ProcessError err )
{
	// disconnect all connections to childproc's signals; they are no longer needed
	m_process->disconnect();

	m_isRunning = false;

	//NOTE: some DVCS commands can use stderr...
	m_failed = true;

	//Do not use d->childproc->exitCode() to set an error! If we have FailedToStart exitCode will return 0,
	//and if exec is used, exec will return true and that is wrong!
	setError(UserDefinedError);
	setErrorText( i18n("Process exited with status %1", m_process->exitCode() ) );

	QString errorValue;
	switch (err)
	{
	case QProcess::FailedToStart:
		errorValue = "FailedToStart";
		break;
	case QProcess::Crashed:
		errorValue = "Crashed";
		break;
	case QProcess::Timedout:
		errorValue = "Timedout";
		break;
	case QProcess::WriteError:
		errorValue = "WriteErro";
		break;
	case QProcess::ReadError:
		errorValue = "ReadError";
		break;
	case QProcess::UnknownError:
		errorValue = "UnknownError";
		break;
	}
	kDebug() << "oops, found an error while running" << dvcsCommand() << ":" << errorValue
													 << "Exit code is:" << m_process->exitCode();
	jobIsReady();
}

void DvcsJob::slotStarted()
{

}

void DvcsJob::slotProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
{
	// disconnect all connections to childproc's signals; they are no longer needed
	m_process->disconnect();

	m_isRunning = false;

	if (exitStatus != QProcess::NormalExit || exitCode != 0)
		slotProcessError(QProcess::UnknownError);

	kDebug() << "process has finished with no errors";
	jobIsReady();
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

void DvcsJob::setCommandArguments( const QStringList &args )
{
	m_command.append( args );
}

DvcsJob::JobStatus DvcsJob::status() const
{
	if (!m_wasStarted)
		return JobNotStarted;
	if (m_failed)
		return JobFailed;
	if (m_isRunning)
		return JobRunning;
	return JobSucceeded;
}


void DvcsJob::jobIsReady()
{
	emit readyForParsing(this); //let parsers to set status
	emitResult(); //KJob
	emit resultsReady(this); //VcsJob
	//reset stases;
	m_isRunning = m_failed = m_wasStarted = false;
}

KProcess* DvcsJob::getChildproc()
{
	return m_process;
}


#include "moc_dvcsjob.cpp"





