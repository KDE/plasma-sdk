#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <QDir>

#include "../metadataeditor.h"

int main( int argc, char **argv )
{
//    KCmdLineArgs::init(argc, argv);
//    KApplication app;

    QApplication app(argc, argv);
    MetaDataEditor ed;

    ed.setFilename( QDir::currentPath() + "/tests/plasma-applet-systemtray.desktop" );
    ed.readFile();
    ed.show();

    return app.exec();
}
