



#include	<KDebug>
#include	<KProcess>
#include	<QFileInfo>
#include	<QDir>
#include	<QProcess>


#include	"gitrunner.h"
#include	"dvcsjob.h"



GitRunner::GitRunner(  )
{
	//m_job = new DvcsJob();
	m_commMode = KProcess::SeparateChannels;
	m_lastRepoRoot = new KUrl();
	m_result = QString();
	m_isRunning = false;
	m_jobStatus = DvcsJob::JobNotStarted;

}

GitRunner::~GitRunner()
{
	/*if( m_job )
		delete m_job;*/
	if( m_lastRepoRoot )
		delete m_lastRepoRoot;
}

void GitRunner::resetJob()
{/*
	m_job = new DvcsJob();
	m_job->setCommunicationMode( m_commMode );
	m_job->setDirectory( QDir( m_lastRepoRoot->pathOrUrl() ) );
	*m_job<< "git";
	m_job->start();
	m_result.clear();
	m_isRunning = false;*/
}

void GitRunner::startJob()
{/*
	m_isRunning = true;
	m_job->start();
	m_result.append( m_job->output() );
	m_isRunning = false;*/
}

void GitRunner::initJob( DvcsJob &job )
{
	job.setCommunicationMode( m_commMode );
	job.setDirectory( QDir( m_lastRepoRoot->pathOrUrl() ) );
	job<< "git";
}

void GitRunner::startJob( DvcsJob &job )
{
	m_result.clear();
	m_isRunning = true;
	job.start();
	m_result.append( job.output() );	// Save the result
	m_isRunning = false;
	m_jobStatus = job.status();			// Save job status
	job.cancel();						// Kill the job
	delete &job;
}

void GitRunner::setCommunicationMode(KProcess::OutputChannelMode comm)
{
	m_commMode = comm;
}

void GitRunner::setDirectory( const KUrl &dir )
{
	m_lastRepoRoot = new KUrl( dir );
}

bool GitRunner::isRunning()
{
	return m_isRunning;
}

bool GitRunner::isValidDirectory(const KUrl & dirPath)
{
	const QString initialPath(dirPath.toLocalFile(KUrl::RemoveTrailingSlash));
	setDirectory( dirPath );

	// A possible git repo has a .git subdicerctory
	const QString gitDir(".git");

	// Also, git rev-parse --is-inside-work-tree returns "true" if we are
	// inside any subdirectory of the git tree.
	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job<< "rev-parse";
	*job<< "--is-inside-work-tree";
	startJob(*job);

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

	return ( dir.exists( gitDir ) && m_result.compare( QString( "true" ) ) ) ? true : false;
}

QString& GitRunner::getResult()
{
	return	m_result;
}


DvcsJob::JobStatus GitRunner::init(const KUrl &directory)
{
	// We need to tell the runner to change dir!
	m_lastRepoRoot = new KUrl( directory );
	DvcsJob *job = new DvcsJob();
	initJob( *job );

	*job<< "init";
	startJob( *job );
	return m_jobStatus;
}


DvcsJob::JobStatus GitRunner::createWorkingCopy(const KUrl& repoOrigin, const KUrl& repoDestination)
{
	// TODO: now supports only cloning a local repo ( not very useful, I know =P ), so extend the method
	// to be used over the Internet.
	m_lastRepoRoot = new KUrl( repoDestination );
	DvcsJob *job = new DvcsJob();
	initJob( *job );

	*job << "clone";
	*job << repoOrigin.pathOrUrl();
	startJob( *job );
	return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::add(const KUrl::List& localLocations)
{
	if (localLocations.empty())
		return m_jobStatus = DvcsJob::JobCancelled;

	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job<< "add";

	// Adding files to the runner.
	QStringList stringFiles = localLocations.toStringList();
	while( !stringFiles.isEmpty() )
	{
		*job<<  m_lastRepoRoot->pathOrUrl()+QString("/")+stringFiles.takeAt(0);
	}

	startJob( *job );
	return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::status(const KUrl &dirPath)
{
	m_lastRepoRoot = new KUrl( dirPath );
	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job<< "status";

	startJob( *job );
	return m_jobStatus;
}


DvcsJob::JobStatus GitRunner::commit(const QString& message)
{
	// NOTE: git doesn't allow empty commit !
	if (message.isEmpty())
		return m_jobStatus = DvcsJob::JobCancelled;

	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job << "commit";
	*job << "-m";
	//Note: the message is quoted somewhere else
	*job << message;

	startJob( *job );
	return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::moveToCommit( const QString &sha1hash, const QString& newBranch )
{
	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job << "checkout";
	*job << sha1hash;

	startJob( *job );
	if( m_jobStatus != DvcsJob::JobSucceeded )
		return m_jobStatus;

	job = new DvcsJob();
	initJob( *job );
	*job << "checkout";
	*job << "-b";
	*job << newBranch;

	startJob( *job );
	return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::deleteCommit( const QString &sha1hash )
{
	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job << "reset";
	*job << "--hard";
	*job << sha1hash;

	startJob( *job );
	return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::remove(const KUrl::List& files)
{
	if (files.empty())
		return m_jobStatus = DvcsJob::JobCancelled;

	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job << "rm";
	QStringList stringFiles = files.toStringList();
	while( !stringFiles.isEmpty() )
	{
		*job<<  m_lastRepoRoot->pathOrUrl()+QString("/")+stringFiles.takeAt(0);
	}

	startJob( *job );
	return m_jobStatus;
}


DvcsJob::JobStatus GitRunner::log(const KUrl& localLocation)
{
	m_lastRepoRoot = new KUrl( localLocation );
	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job << "log";

	startJob( *job );
	return m_jobStatus;
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
	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job << "checkout";
	*job << newBranch;

	startJob( *job );
	return m_jobStatus;
}
DvcsJob::JobStatus GitRunner::deleteBranch( const QString &branch )
{
	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job << "branch";
	*job << "-D";
	*job << branch;

	startJob( *job );
	return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::currentBranch()
{
	DvcsJob::JobStatus status = branches();
	if( status != DvcsJob::JobSucceeded )
		return status;
	// Every branch is listed in one line. so first split by lines,
	// then look for the branch marked with a "*".
	QStringList list = m_result.split( "\n" );
	QString tmp = list.takeFirst();
	while( !tmp.contains( QString( "*" ), Qt::CaseInsensitive ) )
		tmp = list.takeFirst();

	tmp.remove( 0, 2 );
	m_result = tmp;
	return status;
}

DvcsJob::JobStatus GitRunner::branches()
{
	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job << "branch";

	startJob( *job );
	return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::newBranch(const QString &newBranch)
{
	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job<< "branch";
	*job<< newBranch;

	startJob( *job );
	return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::renameBranch( const QString &newBranch )
{
	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job<< "branch";
	*job<< "-m";
	*job<< newBranch;

	startJob( *job );
	return m_jobStatus;
}

DvcsJob::JobStatus GitRunner::reset(const QStringList &args)
{
	DvcsJob *job = new DvcsJob();
	initJob( *job );
	*job << "reset";
	if (!args.isEmpty())
		*job << args;

	startJob( *job );
	return m_jobStatus;
}

#include "gitrunner.moc"
