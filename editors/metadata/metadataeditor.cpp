#include <QDir>
#include <QRegExpValidator>
#include <KDesktopFile>
#include <KConfigGroup>
#include <KDebug>

#include <plasma/packagemetadata.h>

#include "ui_metadata.h"
#include "metadataeditor.h"

MetaDataEditor::MetaDataEditor( QWidget *parent )
    : QWidget( parent ),
      metadata( 0 )
{
    view = new Ui::MetaDataEditor;
    view->setupUi(this);
}

MetaDataEditor::~MetaDataEditor()
{
    delete metadata;
}

void MetaDataEditor::setFilename( const QString &filename  )
{
    this->filename = filename;
}

void MetaDataEditor::readFile()
{
    kDebug() << "readFile file" << filename;

    metadata = new Plasma::PackageMetadata( filename );

    if ( !metadata->isValid() ) {
	kWarning() << "Package file " << filename << " is invalid";
    }

    view->name_edit->setText( metadata->name() );
    view->comment_edit->setText( metadata->description() );

// TODO: icon is missing
//    view->icon_edit->setText( desktopFile.readIcon() );
//    view->icon_button->setIcon( desktopFile.readIcon() );
//    connect( view->icon_button, SIGNAL(iconChanged(const QString &)),
//	     view->icon_edit, SLOT(setText(const QString &)) );
    if ( view->icon_edit->text().isEmpty() ) {
        view->icon_button->setIcon("kde");
    }

    view->pluginname_edit->setText( metadata->pluginName() );

    QString serviceType = metadata->serviceType();

    if ( serviceType == QString("Plasma/Applet") ) {
        view->type_combo->setCurrentIndex(0);
    }
    else if ( serviceType == QString("Plasma/DataEngine") ) {
        view->type_combo->setCurrentIndex(1);
    }
    else if ( serviceType == QString("Plasma/Theme") ) {
        view->type_combo->setCurrentIndex(2);
    }
    else if ( serviceType == QString("Plasma/Runner") ) {
        view->type_combo->setCurrentIndex(3);
    }
    else {
        kWarning() << "Unknown service type" << serviceType;
    }

    // Enforce the security restriction from package.cpp in the input field
    QRegExpValidator *pluginname_validator = new QRegExpValidator( view->pluginname_edit );
    QRegExp validatePluginName("^[\\w-\\.]+$"); // Only allow letters, numbers, underscore and period.
    pluginname_validator->setRegExp(validatePluginName);

    int idx = view->category_combo->findText(metadata->category());
    if ( idx != -1 ) {
        view->category_combo->setCurrentIndex( idx );
    }
    else {
        kWarning() << "Unknown category detected " << metadata->category() << "using miscellaneous instead";
        view->category_combo->setCurrentIndex( view->category_combo->count()-1 ); // misc is last
    }

    view->version_edit->setText( metadata->version() );
    view->website_edit->setText( metadata->website() );
    view->author_edit->setText( metadata->author() );
    view->email_edit->setText( metadata->email() );
    view->license_edit->setText( metadata->license() );
}

void MetaDataEditor::writeFile()
{
    metadata->setName( view->name_edit->text() );
    metadata->setDescription( view->comment_edit->text() );

    //TODO
    //desktopGroup.writeEntry( "Icon", view->icon_edit->text() );

    if ( view->type_combo->currentIndex() == 0 )
	metadata->setServiceType("Plasma/Applet");
    else if ( view->type_combo->currentIndex() == 1 )
	metadata->setServiceType("Plasma/DataEngine");
    else if ( view->type_combo->currentIndex() == 2 )
	metadata->setServiceType("Plasma/Theme");

    metadata->setCategory( view->category_combo->currentText() );
    metadata->setPluginName( view->pluginname_edit->text() );
    metadata->setVersion( view->version_edit->text() );
    metadata->setWebsite( view->website_edit->text() );
    metadata->setAuthor( view->author_edit->text() );
    metadata->setEmail( view->email_edit->text() );
    metadata->setLicense( view->license_edit->text() );

    metadata->write(filename);
}
