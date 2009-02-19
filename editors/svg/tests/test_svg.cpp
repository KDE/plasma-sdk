#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <QDir>
#include <QVBoxLayout>
#include <QPushButton>

#include "../svgviewer.h"

int main( int argc, char **argv )
{
//    KCmdLineArgs::init(argc, argv);
//    KApplication app;

    QApplication app(argc, argv);

    QWidget *top = new QWidget();
    QVBoxLayout *vbox = new QVBoxLayout(top);

    SvgViewer *viewer = new SvgViewer( top );
    vbox->addWidget(viewer);

    QString filename;
    if ( argc != 2 ) {
	filename = QDir::currentPath() + "/tests/battery-oxygen.svgz";
    }
    else {
	filename = QDir::currentPath() + "/" + argv[1];
    }

    viewer->setFilename( filename );
    viewer->readFile();

    top->show();

    return app.exec();
}
