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
}

MetaDataEditor::~MetaDataEditor()
{
}

void MetaDataEditor::readConfig( const QString &filename )
{
    kDebug() << "readConfig file" << filename;

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
