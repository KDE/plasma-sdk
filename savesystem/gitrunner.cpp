



#include	<KDebug>
#include	<KProcess>
#include	<QFileInfo>
#include	<QDir>
#include	<QProcess>


#include	"gitrunner.h"
#include "dvcsjob.h"



GitRunner::GitRunner(  )
{
	m_job = new DvcsJob();
	m_commMode = KProcess::SeparateChannels;
	m_lastRepoRoot = new KUrl();
	m_result = QString();
	m_isRunning = false;

}

GitRunner::~GitRunner()
{
	if( m_job )
		delete m_job;
	if( m_lastRepoRoot )
		delete m_lastRepoRoot;
}

void GitRunner::resetJob()
{
	m_job = new DvcsJob();
	m_job->setCommunicationMode( m_commMode );
	m_job->setDirectory( QDir( m_lastRepoRoot->pathOrUrl() ) );
	*m_job<< "git";
	m_job->start();
	m_result.clear();
	m_isRunning = false;
}

void GitRunner::startJob()
{
	m_isRunning = true;
	m_job->start();
	m_result.append( m_job->output() );
	m_isRunning = false;
}

void GitRunner::setCommunicationMode(KProcess::OutputChannelMode comm)
{
	m_commMode = comm;
}

void GitRunner::setDirectory(const QDir &dir)
{
	m_lastRepoRoot = new KUrl( dir.absolutePath() );
}

bool GitRunner::isRunning()
{
	return m_isRunning;
}

bool GitRunner::isValidDirectory(const KUrl & dirPath)
{
	const QString initialPath(dirPath.toLocalFile(KUrl::RemoveTrailingSlash));

	// A possible git repo has a .git subdicerctory
	const QString gitDir(".git");

	// Also, git rev-parse --is-inside-work-tree returns "true" if we are
	// inside any subdirectory of the git tree.
	resetJob();
	*m_job<< "rev-parse";
	*m_job<< "--is-inside-work-tree";
	startJob();

	QFileInfo finfo(initialPath);
	QDir dir;
	if (finfo.isFile())
	{
		dir = finfo.absoluteDir();
	} else
	{
		dir = QDir(initialPath);
		dir.makeAbsolute();
	}

	return (dir.exists( gitDir ) && m_job->output().compare( QString( "true" ) ) ) ? true : false;
}

QString& GitRunner::getResult()
{
	return	m_result;
}


DvcsJob::JobStatus GitRunner::init(const KUrl &directory)
{
	// We need to change dir!
	m_lastRepoRoot = new KUrl( directory );
	resetJob();

	*m_job<< "init";
	startJob();
	return m_job->status();
}


DvcsJob::JobStatus GitRunner::createWorkingCopy(const KUrl& repoOrigin, const KUrl& repoDestination)
{
	// TODO: now supports only cloning a local repo ( not very useful, I know =P ), so extend the method
	// to be used over the Internet.
	m_lastRepoRoot = new KUrl( repoDestination );
	resetJob();

	*m_job << "clone";
	*m_job << repoOrigin.pathOrUrl();
	startJob();
	return m_job->status();
}

DvcsJob::JobStatus GitRunner::add(const KUrl::List& localLocations)
{
	if (localLocations.empty())
		return DvcsJob::JobCancelled;

	resetJob();
	*m_job<< "add";

	// Adding files to the runner.
	QStringList stringFiles = localLocations.toStringList();
	while( !stringFiles.isEmpty() )
	{
		*m_job<<  m_lastRepoRoot->pathOrUrl()+QString("/")+stringFiles.takeAt(0);
	}

	startJob();
	return m_job->status();
}

DvcsJob::JobStatus GitRunner::status(const KUrl &dirPath)
{
	m_lastRepoRoot = new KUrl( dirPath );
	resetJob();
	*m_job<< "status";

	startJob();
	return m_job->status();
}


DvcsJob::JobStatus GitRunner::commit(const QString& message)
{
	// NOTE: git doesn't allow empty commit !
	if (message.isEmpty())
		return DvcsJob::JobCancelled;

	resetJob();
	*m_job << "commit";
	*m_job << "-m";
	//Note: the message is quoted somewhere else, so if we quote here then we have quotes in the commit slotSaveResult
	*m_job << message;

	startJob();
	return m_job->status();
}

DvcsJob::JobStatus GitRunner::remove(const KUrl::List& files)
{
	if (files.empty())
		return DvcsJob::JobCancelled;

	resetJob();
	*m_job << "rm";
	QStringList stringFiles = files.toStringList();
	while( !stringFiles.isEmpty() )
	{
		*m_job<<  m_lastRepoRoot->pathOrUrl()+QString("/")+stringFiles.takeAt(0);
	}

	startJob();
	return m_job->status();
}


DvcsJob::JobStatus GitRunner::log(const KUrl& localLocation)
{
	m_lastRepoRoot = new KUrl( localLocation );
	resetJob();
	*m_job << "log";

	startJob();
	return m_job->status();
}

DvcsJob::JobStatus GitRunner::annotate(const KUrl &localLocation)
{/*
	DvcsJob* job = new DvcsJob();
	*job << "git";
	*job << "blame";
	*job << "--root";
	*job << "-t";

	if (job)
	{
		delete job;
		return NULL;
	}
	addFileList(job, localLocation);
	connect(job, SIGNAL(readyForParsing(DvcsJob*)), this, SLOT(parseGitBlameOutput(DvcsJob*)));
	return job;*/
	return DvcsJob::JobNotStarted;
}


DvcsJob::JobStatus GitRunner::switchBranch(const QString &newBranch)
{
	resetJob();
	*m_job << "checkout";
	*m_job << newBranch;

	startJob();
	return m_job->status();
}

DvcsJob::JobStatus GitRunner::branches()
{
	resetJob();
	*m_job << "branch";
	m_job->start();

	startJob();
	return m_job->status();
}

DvcsJob::JobStatus GitRunner::newBranch(const QString &newBranch)
{
	resetJob();
	*m_job<< "branch";
	*m_job<< newBranch;

	startJob();
	return m_job->status();
}

DvcsJob::JobStatus GitRunner::reset(const QStringList &args)
{
	resetJob();
	*m_job << "reset";
	if (!args.isEmpty())
		*m_job << args;

	startJob();
	return m_job->status();
}

#include "gitrunner.moc"
