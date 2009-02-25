#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <QDir>
#include <QVBoxLayout>
#include <KPushButton>

#include "../kconfigxteditor.h"

int main( int argc, char **argv )
{
//    KCmdLineArgs::init(argc, argv);
//    KApplication app;

    QApplication app(argc, argv);

    QWidget *top = new QWidget();
    QVBoxLayout *vbox = new QVBoxLayout(top);

    KConfigXtEditor *ed = new KConfigXtEditor( top );
    vbox->addWidget(ed);

    KPushButton *save = new KPushButton( top );
    save->setText("&Save");
    vbox->addWidget(save);

    QString filename;
    if ( argc != 2 ) {
        filename = QDir::currentPath() + "/tests/kconfigxt-test.kcfg";
    }
    else {
        filename = QDir::currentPath() + "/" + argv[1];
    }

    /* TODO implement
    ed->setFilename( filename );
    ed->readFile();

    QObject::connect( save, SIGNAL(clicked()), ed, SLOT(writeFile()) );
    */
    top->show();

    return app.exec();
}
