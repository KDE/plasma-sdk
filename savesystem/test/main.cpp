/*
  Simple test app for the GitRunner class.
*/

#include <KApplication>
#include <KAboutData>
#include <KIcon>
#include <KLocale>
#include <KCmdLineArgs>
#include <KUrl>

#include <QDialog>
#include <QDir>
#include <QDockWidget>
#include <QFile>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QString>

#include "../gitrunner.h"
#include "../dvcsjob.h"
#include "../timeline.h"

QString appDir;
QString rootDir;


void testGitRunner() {
        QDir dirHandler = QDir(appDir);

    // Let's build a simple directory structure, with some files
    if (dirHandler.mkdir("root")) {                                           // Start with creating our root dir.
        // If success, add create a file named main.cpp .
        QFile *main = new QFile(rootDir + "main.cpp");       // Defining ../root/main.cpp
        main->open(QIODevice::WriteOnly);
        main->close();
    }

    // Init our GitRunner instance
    GitRunner *git = new GitRunner();

    // Set the repo directory
    git->setDirectory(KUrl(rootDir));

    // Perform a check on the folder to confirm we have a valid git repo
    if (!git->isValidDirectory())
        git->init(KUrl(rootDir));                                             // Init an empty repo

    // Note: in the following GitRunner call, I'll skip testing the JobStatus value returned,
    // in order to focus the reader on how the runner works; in a real application, always
    // perform that check !!!

    // Ok, now let's add and commit one file
    QStringList *list = new QStringList("main.cpp");
    git->add(KUrl::List(*list))   ;                                           // Add the element in the git index
    git->commit(QString("This is a comment: committed main."));               // Commit it

    // Now create some other files
    QFile *src1 = new QFile(rootDir + "src1.cpp");       // Defining ../root/src2.cpp
    QFile *src2 = new QFile(rootDir + "src2.cpp");       // Defining ../root/src2.cpp
    QFile *src3 = new QFile(rootDir + "src3.cpp");       // Defining ../root/src2.cpp
    src1->open(QIODevice::WriteOnly);
    src2->open(QIODevice::WriteOnly);
    src3->open(QIODevice::WriteOnly);
    src1->close();
    src2->close();
    src3->close();

    // Now add them and then commit
    list->clear();
    *list << "src1.cpp";
    *list << "src2.cpp";
    *list << "src3.cpp";
    git->add(KUrl::List(*list));
    git->commit("Multiline comment.\n\nThe first line briefly explains the commits purpose,\nwhile the remaining lines add more details.");

    // Now lets create a new branch
    QString brName("devel");
    git->newBranch(brName);
    // Switch to the new branch
    git->switchBranch(brName);

    // Add an other file
    QFile *h1 = new QFile(rootDir + "h1.h");             // Defining ../root/h1.h
    h1->open(QIODevice::WriteOnly);
    h1->close();

    // Lets perform an other add/commit action
    list->clear();
    list->append("h1.h");
    git->add(KUrl::List(*list));
    git->commit("Added h1.h in branch \"devel\".");


    // Note: you can also try  git->createWorkingCopy(), to clone a local repo from an other.

    // Show the logs regarding the devel branch
    // Note: it derives from "master", so there will be also
    // the commits made in "master" since "devel" creation.
    git->log();
    QString log = git->getResult();
    QMessageBox *mb = new QMessageBox(QMessageBox::NoIcon, QString("Log result in \"devel\":") , log, 0, 0, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    mb->exec();

    // Switch branch
    git->switchBranch(QString("master"));

    // Show the logs regarding the "master" branch
    // Note: the commit made inside "devel" branch won't be listed!
    git->log();
    QString log1 = git->getResult();
    QMessageBox *mb1 = new QMessageBox(QMessageBox::NoIcon, QString("Log result in \"master\":") , log1, 0, 0, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    mb1->exec();

    // Show branches
    git->branches();
    QString br = git->getResult();
    QMessageBox *mb2 = new QMessageBox(QMessageBox::NoIcon, QString("Log result in \"master\":") , br, 0, 0, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    mb2->exec();

    // Now lets create an other file, but we don't commit it, so in the testTimeLine()
    // you'll be able to perform a new SavePoint by clicking the button.
    QFile *src4 = new QFile(rootDir + "src4.cpp");       // Defining ../root/src2.cpp
    src4->open(QIODevice::WriteOnly);
    src4->close();
}

void testTimeLine() {
    // Setup the main window, a dock widget and the TimeLine
    QMainWindow *w = new QMainWindow(0, Qt::Window);
    //QDockWidget *dockTimeLine = new QDockWidget(i18n("TimeLine"), w);
    TimeLine *timeline = new TimeLine(w,
                                      KUrl(rootDir),
                                      Qt::RightDockWidgetArea);

    // Setting some relationships between widgets
    //dockTimeLine->setWidget(timeline);
    w->addDockWidget(Qt::LeftDockWidgetArea, timeline);
    timeline->loadTimeLine(KUrl(rootDir));

    // Some policy
    //dockTimeLine->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    //timeline->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    // Now lets create a New SavePoint button, that will be connected to the
    // TimeLine::newSavePoint() slot
    //QWidget *centralDialog = new QWidget(w);
    QPushButton *newCommit = new QPushButton(KIcon("document-save"),
                                             i18n("New save point"),
                                             w);//centralDialog);
    //w->setCentralWidget(centralDialog);
    //centralDialog->show();

    QObject::connect(newCommit, SIGNAL(clicked()),
            timeline, SLOT(newSavePoint()));

    w->resize(400, 400);
    w->show();
}

int main(int argc, char *argv[])
{
    KAboutData aboutData("timeline_test", 0, ki18n("TimeLine and GitRunner Test app"),
                         "0.1", ki18n("Shows how to use TimeLine and GitRunner"),
                         KAboutData::License_GPL,
                         ki18n("Diego [Po]lentino Casella"),
                         KLocalizedString(), "", "polentino911@gmail.com");
    aboutData.addAuthor(ki18n("Diego [Po]lentino Casella"), ki18n("Author"), "polentino911@gmail.com");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KApplication app;
    appDir = app.applicationDirPath();
    rootDir = appDir + "/root/";

    testGitRunner();
    testTimeLine();

    return app.exec();
}

