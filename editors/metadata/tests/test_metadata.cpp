#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <QDir>
#include <QVBoxLayout>
#include <QPushButton>

#include "../metadataeditor.h"

int main( int argc, char **argv )
{
//    KCmdLineArgs::init(argc, argv);
//    KApplication app;

    QApplication app(argc, argv);

    QWidget *top = new QWidget();
    QVBoxLayout *vbox = new QVBoxLayout(top);

    MetaDataEditor *ed = new MetaDataEditor( top );
    vbox->addWidget(ed);

    QPushButton *save = new QPushButton( top );
    save->setText("&Save");
    vbox->addWidget(save);

    QString filename;
    if ( argc != 2 ) {
	filename = QDir::currentPath() + "/tests/plasma-applet-systemtray.desktop";
    }
    else {
	filename = QDir::currentPath() + "/" + argv[1];
    }

    ed->setFilename( filename );
    ed->readFile();

    QObject::connect( save, SIGNAL(clicked()), ed, SLOT(writeFile()) );
    top->show();

    return app.exec();
}
