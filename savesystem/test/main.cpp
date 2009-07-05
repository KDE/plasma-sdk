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

#include "../gitrunner.h"

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


	// Let's build a simple directory structure, with some files
	if( !dirHandler.mkdir( "root" ) )										// Start with creating our root dir.
		return 1;															// if not possible, exit.

	QString rootDir = appDir+QString( "/root/" );

	QFile *main = new QFile( QString( rootDir+QString( "main.cpp" ) ) );	// Defining ../root/main.cpp
	QFile *h1 = new QFile( QString( rootDir+QString( "h1.h" ) ) );			// Defining ../root/h1.h
	QFile *src1 = new QFile( QString( rootDir+QString( "src1.cpp" ) ) );	// Defining ../root/src2.cpp
	QFile *src2 = new QFile( QString( rootDir+QString( "src2.cpp" ) ) );	// Defining ../root/src2.cpp
	QFile *src3 = new QFile( QString( rootDir+QString( "src3.cpp" ) ) );	// Defining ../root/src2.cpp
	main->open(QIODevice::WriteOnly);
	h1->open(QIODevice::WriteOnly);
	src1->open(QIODevice::WriteOnly);
	src2->open(QIODevice::WriteOnly);
	src3->open(QIODevice::WriteOnly);
	main->close();
	h1->close();
	src1->close();
	src2->close();
	src3->close();


	// Note: since the parseOutput function is not ready, to see the final result you
	// have to to the root directory and call git log =)
	// If you want to re-run the test, delete the root folder.

	// Init our GitRunner instanxe
	GitRunner *git = new GitRunner();
	// Init an empty repo
	git->init( KUrl( rootDir ) );

	// Perform a check on the folder to cofirm we have a valid git repo
	if( git->isValidDirectory( rootDir ) ) {

		//Eventually set the repo directory
		git->setDirectory( QDir( rootDir ) );

		// Ok, now let's add and commit some files
		QStringList *list= new QStringList( QString("main.cpp") );
		git->add( KUrl::List( *list ) );
		*list << QString( "src1.cpp" );
		*list << QString( "src2.cpp" );
		*list << QString( "src3.cpp" );
		git->commit( QString( "This is a comment: committed main, src1/2/3." ), KUrl::List( *list ) );


		// Try if it works with a list with only one element
		list= new QStringList( QString("h1.h") );
		git->add( KUrl::List( *list ) );
		git->commit( QString( "Multiline comment.\n\nThe first line briefly explains the commits purpose,\nwhile the remaining lines are more detailed" ), KUrl::List( *list ) );

		// Note: you can also try  git->createWorkingCopy(), to clone a local repo to an other.
	}

	return 0;
}

