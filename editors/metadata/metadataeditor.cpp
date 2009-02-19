#include <QDir>
#include <KDesktopFile>
#include <KConfigGroup>
#include <KDebug>

#include "ui_metadata.h"
#include "metadataeditor.h"

MetaDataEditor::MetaDataEditor( QWidget *parent )
    : QWidget( parent )
{
    view = new Ui::MetaDataEditor;
    view->setupUi(this);

    connect( view->write_button, SIGNAL(clicked()), this, SLOT(writeFile()) );
}

MetaDataEditor::~MetaDataEditor()
{
}

void MetaDataEditor::setFilename( const QString &filename  )
{
    this->filename = filename;
}

void MetaDataEditor::readFile()
{
    kDebug() << "readFile file" << filename;

    KDesktopFile desktopFile( filename );
    KConfigGroup desktopGroup = desktopFile.desktopGroup();

    view->name_edit->setText( desktopFile.readName() );
    view->comment_edit->setText( desktopFile.readComment() );
    view->icon_edit->setText( desktopFile.readIcon() );
    //type
    view->library_edit->setText( desktopGroup.readEntry("X-KDE-Library") );
    view->version_edit->setText( desktopGroup.readEntry("X-KDE-PluginInfo-Version") );
    view->website_edit->setText( desktopGroup.readEntry("X-KDE-PluginInfo-Website") );
    view->author_edit->setText( desktopGroup.readEntry("X-KDE-PluginInfo-Author") );
    view->email_edit->setText( desktopGroup.readEntry("X-KDE-PluginInfo-Email") );
    view->license_edit->setText( desktopGroup.readEntry("X-KDE-PluginInfo-License") );
}

void MetaDataEditor::writeFile()
{
    KConfig desktopFile( filename, KConfig::SimpleConfig );
    KConfigGroup desktopGroup = desktopFile.group( "Desktop Entry" );

    desktopGroup.writeEntry( "Name", view->name_edit->text() );
    desktopGroup.writeEntry( "Comment", view->comment_edit->text() );
    // desktopGroup.writeEntry( "Icon", view->version_edit->text() );

    desktopGroup.writeEntry( "X-KDE-Library", view->library_edit->text() );
    desktopGroup.writeEntry( "X-KDE-PluginInfo-Version", view->version_edit->text() );
    desktopGroup.writeEntry( "X-KDE-PluginInfo-Website", view->website_edit->text() );
    desktopGroup.writeEntry( "X-KDE-PluginInfo-Author", view->author_edit->text() );
    desktopGroup.writeEntry( "X-KDE-PluginInfo-Email", view->email_edit->text() );
    desktopGroup.writeEntry( "X-KDE-PluginInfo-License", view->license_edit->text() );

    desktopFile.sync();
}
