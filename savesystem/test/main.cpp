/*
  Simple test app for the GitRunner class.
*/

#include <KApplication>
#include <KAboutData>
#include <KLocale>
#include <KCmdLineArgs>
#include	<KUrl>

#include	<QString>
#include	<QDir>
#include	<QFile>
#include	<QMessageBox>

#include	"../gitrunner.h"
#include	"../dvcsjob.h"

int main(int argc, char *argv[])
{
	KAboutData aboutData("gitrunner_test", 0, ki18n("GitRunner Test app"),
						 "0.1", ki18n("Shows how to use GitRunner"),
						 KAboutData::License_GPL,
						 ki18n("Diego [Po]lentino Casella"),
						 KLocalizedString(), "", "polentino911@gmail.com");
	aboutData.addAuthor(ki18n("Diego [Po]lentino Casella"), ki18n("Author"), "polentino911@gmail.com");

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication app;
	QString appDir = app.applicationDirPath();
	QDir dirHandler = QDir( appDir );
	QString rootDir = appDir+QString( "/root/" );

	// Let's build a simple directory structure, with some files
	if( dirHandler.mkdir( "root" ) )											// Start with creating our root dir.
	{
		// If success, add create a file named main.cpp .
		QFile *main = new QFile( QString( rootDir+QString( "main.cpp" ) ) );	// Defining ../root/main.cpp
		main->open(QIODevice::WriteOnly);
		main->close();
	}

	// Init our GitRunner instanxe
	GitRunner *git = new GitRunner();

	// Perform a check on the folder to cofirm we have a valid git repo
	if( !git->isValidDirectory( rootDir ) )
		git->init( KUrl( rootDir ) );											// Init an empty repo

	// Note: in the following GitRunner call, I'll skip testing the JobStatus value returned,
	// in order to focus the reader on how the runner works; in a real application, always
	// perform that check !!!


	// Eventually set the repo directory
	git->setDirectory( KUrl( rootDir ) );

	// Ok, now let's add and commit one file
	QStringList *list= new QStringList( QString("main.cpp") );
	git->add( KUrl::List( *list ) )	;										// Add the element in the git index
	git->commit( QString( "This is a comment: committed main." ) );			// Commit it

	// Now create some other files
	QFile *src1 = new QFile( QString( rootDir+QString( "src1.cpp" ) ) );	// Defining ../root/src2.cpp
	QFile *src2 = new QFile( QString( rootDir+QString( "src2.cpp" ) ) );	// Defining ../root/src2.cpp
	QFile *src3 = new QFile( QString( rootDir+QString( "src3.cpp" ) ) );	// Defining ../root/src2.cpp
	src1->open(QIODevice::WriteOnly);
	src2->open(QIODevice::WriteOnly);
	src3->open(QIODevice::WriteOnly);
	src1->close();
	src2->close();
	src3->close();

	// Now add them and then commit
	list= new QStringList( QString("src1.cpp") );
	*list << QString( "src1.cpp" );
	*list << QString( "src2.cpp" );
	git->add( KUrl::List( *list ) );
	git->commit( QString( "Multiline comment.\n\nThe first line briefly explains the commits purpose,\nwhile the remaining lines add more details." ) );

	// Now lets create a new branch
	QString brName( "devel" );
	git->newBranch( brName );
	// Switch to the new branch
	git->switchBranch( brName );

	// Add an other file
	QFile *h1 = new QFile( QString( rootDir+QString( "h1.h" ) ) );			// Defining ../root/h1.h
	h1->open(QIODevice::WriteOnly);
	h1->close();

	// Lets perform an other add/commit action
	list= new QStringList( QString("h1.h") );
	git->add( KUrl::List( *list ) );
	git->commit( QString( "Added h1.h in branch \"devel\"." ) );


		// Note: you can also try  git->createWorkingCopy(), to clone a local repo from an other.

	// Show the logs regarding the devel branch
	// Note: it derives from "master", so there will be also
	// the commits made in "master" since "devel" creation.
	git->log( rootDir );
	QString log = git->getResult();
	QMessageBox *mb = new QMessageBox ( QMessageBox::NoIcon, QString("Log result in \"devel\":") , log, 0, 0, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint );
	mb->exec();

	// Switch branch
	git->switchBranch( QString( "master" ) );

	// Show the logs regarding the "master" branch
	// Note: the commit made inside "devel" branch won't be listed!
	git->log( rootDir );
	QString log1 = git->getResult();
	QMessageBox *mb1 = new QMessageBox ( QMessageBox::NoIcon, QString("Log result in \"master\":") , log1, 0, 0, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint );
	mb1->exec();

	// Show branches
	git->branches();
	QString br = git->getResult();
	QMessageBox *mb2 = new QMessageBox ( QMessageBox::NoIcon, QString("Log result in \"master\":") , br, 0, 0, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint );
	mb2->exec();

	return 0;
}

