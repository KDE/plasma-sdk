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

    ed.readConfig( QDir::currentPath() + "/tests/plasma-applet-systemtray.desktop" );
    ed.show();

    return app.exec();
}
