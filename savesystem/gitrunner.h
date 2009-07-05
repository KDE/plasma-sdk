
#ifndef		GITRUNNER_H
#define		GITRUNNER_H

#include	<QObject>
#include	<KUrl>
#include	<QStringList>
#include	<QDir>

class DvcsJob;

class GitRunner : public QObject
{
	Q_OBJECT

public:
	GitRunner();
	~GitRunner();

	bool isValidDirectory(const KUrl &dirPath);
	void setDirectory(const QDir &dir );

	void add(const KUrl::List& localLocations);
	void createWorkingCopy(const KUrl& repoOrigin, const KUrl& repoDestination);

	DvcsJob* remove(const KUrl::List& files);
	DvcsJob* status(const KUrl::List& localLocations,
							 int recursionMode);
	void commit(const QString& message, const KUrl::List& localLocations);


	DvcsJob* log(const KUrl& localLocation, unsigned long limit);
	DvcsJob* annotate(const KUrl &localLocation );

	void init(const KUrl & directory);
	void reset(const KUrl& repository, const QStringList &args,	const KUrl::List& files);


	DvcsJob* switchBranch(const QString &repository, const QString &branch);
	void branch();

	QString curBranch(const QString &repository);
	QStringList branches(const QString &repository);

	//used in log
	//QStringList parseLogOutput(const DvcsJob * job) const;

protected:


	void addFileList( DvcsJob *j, const KUrl::List &files );

	void lsFiles(const QString &repository, const QStringList &args);
	DvcsJob* gitRevList(const QString &repository, const QStringList &args);
	void gitRevParse(const QString &repository, const QStringList &args);

protected slots:
	void parseGitBlameOutput(DvcsJob *job);
	void parseGitLogOutput(DvcsJob *job);
	void parseBranch(DvcsJob *job);

private:
	//commit dialog "main" helper
	QStringList getLsFiles(const QString &directory, const QStringList &args = QStringList());

	void initBranchHash(const QString &repo);


	QList<QStringList> branchesShas;
	QString	*m_currentBranch;
	KUrl	*m_lastRepoRoot;
};

#endif
