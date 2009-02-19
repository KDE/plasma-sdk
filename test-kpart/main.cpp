#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KUrl>
 
#include "mainwindow.h"
 
int main (int argc, char *argv[])
{
    KAboutData aboutData( "kparttutorial1", "kparttutorial1",
        ki18n("KPart Tutorial 1"), "0.1",
        ki18n("A MainWindow for a KatePart."),
        KAboutData::License_GPL,
        ki18n("Copyright (c) 2007 Developer") );
    KCmdLineArgs::init( argc, argv, &aboutData );
 
    KCmdLineOptions options;
    options.add("+[file]", ki18n("Document to open"));
    KCmdLineArgs::addCmdLineOptions(options);
 
    KApplication app;
 
    MainWindow* window = new MainWindow();
    window->show();
 
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if(args->count())
    {
        window->load(args->url(0).url());
    }
 
    return app.exec();
}