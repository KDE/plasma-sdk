/**
  *	Git Hanlder class
  */

#include	<KDebug>
#include	<QFileInfo>
#include	<QDir>
#include	<QDateTime>
#include	<QProcess>
#include	<QRegExp>


#include "dvcsjob.h"
#include "gitrunner.h"


GitRunner::GitRunner(  )
{
	branchesShas = QList<QStringList>();
	m_lastRepoRoot = new KUrl();
}

GitRunner::~GitRunner()
{
}

bool GitRunner::isValidDirectory(const KUrl & dirPath)
{
	const QString initialPath(dirPath.toLocalFile(KUrl::RemoveTrailingSlash));

	// A possible git repo has a .git subdicerctory
	static const QString gitDir(".git");
	// Also, git rev-parse --is-inside-work-tree returns "true" if we are
	// inside any subdirectory of the git tree.
	DvcsJob *j = new DvcsJob();
	j->setDirectory( QDir( initialPath ) );
	*j<< "git";
	*j<< "rev-parse";
	*j<< "--is-inside-work-tree";
	j->start();

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

	if( dir.exists( gitDir ) && j->output().compare( QString( "true" ) ) ) {
		m_lastRepoRoot = new KUrl( initialPath );
		return true;
	}
	return false;
}

void GitRunner::init(const KUrl &directory)
{
	DvcsJob* job = new DvcsJob();
	job->setDirectory( QDir( directory.pathOrUrl() ) );
	*job << "git";
	*job << "init";
	job->start();
	m_lastRepoRoot = new KUrl( directory );
}

void GitRunner::setDirectory(const QDir &dir)
{
	m_lastRepoRoot = new KUrl( dir.absolutePath() );
}

void GitRunner::createWorkingCopy(const KUrl& repoOrigin, const KUrl& repoDestination)
{
	// TODO: now supports only cloning a local repo ( not very useful, I know =P ), so extend the method
	// to be used over the Internet.
	DvcsJob* job = new DvcsJob();
	job->setCommunicationMode( KProcess::SeparateChannels );
	job->setDirectory( QDir( repoDestination.pathOrUrl() ) );
	*job << "git";
	*job << "clone";
	*job << repoOrigin.pathOrUrl();
	job->start();
}

void GitRunner::add(const KUrl::List& localLocations)
{
	if (localLocations.empty())
		return;

	DvcsJob* job = new DvcsJob();
	job->setCommunicationMode( KProcess::SeparateChannels );
	job->setDirectory( QDir( m_lastRepoRoot->pathOrUrl() ) );
	*job << "git";
	*job << "add";

	QStringList stringFiles = localLocations.toStringList();
	while( !stringFiles.isEmpty() )
	{
		*job<<  m_lastRepoRoot->pathOrUrl()+QString("/")+stringFiles.takeAt(0);
	}
	job->start();
}

DvcsJob* GitRunner::status(const KUrl::List& localLocations, int recursionMode)
{
	Q_UNUSED(recursionMode)
	//it's a hack!!! See VcsCommitDialog::setCommitCandidates and the usage of DvcsJob/IDVCSexecutor
	//We need results just in status, so we set them here before execution in VcsCommitDialog::setCommitCandidates
	QString repo = localLocations[0].toLocalFile();
	QList<QVariant> statuses;
	qDebug("GitRunner::status");
	/*statuses << getCachedFiles(repo)
			 << getModifiedFiles(repo)
			 << getOtherFiles(repo);*/
	DvcsJob * noOp = new DvcsJob();
	noOp->setResults(QVariant(statuses));
	return noOp;
}


void GitRunner::commit(const QString& message, const KUrl::List& localLocations)
{
	// NOTE: git doesn't allow empty commit !
	if (localLocations.empty() || message.isEmpty())
		return;

	DvcsJob* job = new DvcsJob();
	job->setDirectory( QDir( m_lastRepoRoot->pathOrUrl() ) );
	*job << "git";
	*job << "commit";
	*job << "-m";
	//Note: the message is quoted somewhere else, so if we quote here then we have quotes in the commit log
	*job << message;

	/*QStringList stringFiles = localLocations.toStringList();
	while( !stringFiles.isEmpty() )
	{
		*job<<  m_lastRepoRoot->pathOrUrl()+QString("/")+stringFiles.takeAt(0);
	}*/
	job->start();

}

DvcsJob* GitRunner::remove(const KUrl::List& files)
{
	if (files.empty())
		return NULL;

	DvcsJob* job = new DvcsJob();
	*job << "git";
	*job << "rm";

	addFileList(job, files);
	return job;
}


DvcsJob* GitRunner::log(const KUrl& localLocation,	unsigned long limit)
{
	Q_UNUSED(limit)
	DvcsJob* job = new DvcsJob();
	*job << "git";
	*job << "log";
	*job << "--date=raw";

	addFileList(job, localLocation);
	connect(job, SIGNAL(readyForParsing(DvcsJob*)), this, SLOT(parseGitLogOutput(DvcsJob*)));
	return job;
}

DvcsJob* GitRunner::annotate(const KUrl &localLocation)
{
	DvcsJob* job = new DvcsJob();
	*job << "git";
	*job << "blame";
	*job << "--root";
	*job << "-t";

	addFileList(job, localLocation);
	connect(job, SIGNAL(readyForParsing(DvcsJob*)), this, SLOT(parseGitBlameOutput(DvcsJob*)));
	return job;
}

// To be implemented
void GitRunner::parseGitBlameOutput(DvcsJob *job)
{

}

void GitRunner::addFileList( DvcsJob *j,const KUrl::List &files )
{

}

DvcsJob* GitRunner::switchBranch(const QString &repository, const QString &GitRunner)
{

}

void GitRunner::branch()
{
	// Need to be revised
	DvcsJob* job = new DvcsJob();
	job->setDirectory( QDir( m_lastRepoRoot->pathOrUrl() ) );
	*job << "git";
	*job << "branch";
	job->start();
	QString branch( job->output() );
	branch = branch.prepend('\n').section("\n*", 1);
	branch = branch.section('\n', 0, 0).trimmed();
	kDebug() << "Current branch is: " << branch;
	m_currentBranch = new QString( branch );

}

void GitRunner::parseBranch(DvcsJob *job)
{
	Q_UNUSED(job)
}

void GitRunner::reset(const KUrl& repository, const QStringList &args, const KUrl::List& files)
{
	Q_UNUSED(repository)
	Q_UNUSED(files)
	Q_UNUSED(args)
}

void GitRunner::lsFiles(const QString &repository, const QStringList &args)
{
	Q_UNUSED(repository)
	Q_UNUSED(args)

}

QString GitRunner::curBranch(const QString &repository)
{
	Q_UNUSED(repository)
}

QStringList GitRunner::branches(const QString &repository)
{
	Q_UNUSED(repository)
}

void GitRunner::initBranchHash(const QString &repo)
{
	Q_UNUSED(repo)
}

void GitRunner::parseGitLogOutput(DvcsJob * job)
{
	Q_UNUSED(job)
}


QStringList GitRunner::getLsFiles(const QString &directory, const QStringList &args)
{
	Q_UNUSED(directory)
	Q_UNUSED(args)
}

void GitRunner::gitRevParse(const QString &repository, const QStringList &args)
{
	Q_UNUSED(repository)
	Q_UNUSED(args)
}

#include "gitrunner.moc"
