#include <QDir>
#include <QRegExpValidator>
#include <KDesktopFile>
#include <KConfigGroup>
#include <KDebug>

#include <plasma/packagemetadata.h>
#include <plasma/plasma.h>
#include <plasma/scripting/scriptengine.h>

#include "ui_metadata.h"
#include "metadataeditor.h"

//
// TODO: Now we know what this does it needs rewriting to use the metadata object
// as its main store. And to handle updates in a clean per-change way rather than
// the hacks that are there right now.
//

MetaDataEditor::MetaDataEditor( QWidget *parent )
    : QWidget( parent ),
      metadata( 0 )
{
    view = new Ui::MetaDataEditor;
    view->setupUi(this);

    connect( view->type_combo, SIGNAL(currentIndexChanged(int)), SLOT(serviceTypeChanged()) );

    categories << QString("Application Launchers")                                 
	       << QString("Astronomy")                                             
	       << QString("Date and Time")                                         
	       << QString("Development Tools")                            
	       << QString("Education")                                             
	       << QString("Environment and Weather")                               
	       << QString("Examples")                                              
	       << QString("File System")                                           
	       << QString("Fun and Games")
	       << QString("Graphics")
	       << QString("Language")
	       << QString("Mapping")
	       << QString("Online Services")
	       << QString("System Information")
	       << QString("Utilities")
	       << QString("Windows and Tasks")
	       << QString("Miscellaneous");
    QString category;
    foreach( const QString &category, categories ) {
	// TODO: i18n
	view->category_combo->addItem( category );
    }
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

    delete metadata;
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
    serviceTypeChanged();

    // Enforce the security restriction from package.cpp in the input field
    QRegExpValidator *pluginname_validator = new QRegExpValidator( view->pluginname_edit );
    QRegExp validatePluginName("^[\\w-\\.]+$"); // Only allow letters, numbers, underscore and period.
    pluginname_validator->setRegExp(validatePluginName);
    view->pluginname_edit->setValidator(pluginname_validator);

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

void MetaDataEditor::serviceTypeChanged()
{
    Plasma::ComponentTypes currentType;

    switch( view->type_combo->currentIndex() ) {
	case 0:
	    metadata->setServiceType("Plasma/Applet");
	    currentType = Plasma::AppletComponent;
	    break;
	case 1:
	    metadata->setServiceType("Plasma/DataEngine");
	    currentType = Plasma::DataEngineComponent;
	    break;
	case 2:
	    metadata->setServiceType("Plasma/Theme");
	    view->api_combo->setEnabled(false);
	    return;
	    break;
	case 3:
	    metadata->setServiceType("Plasma/Runner");
	    view->api_combo->setEnabled(false);
	    currentType = Plasma::RunnerComponent;
	    break;
	default:
	    kWarning() << "Unknown service type" << currentType;
	    return;
    }

    view->api_combo->setEnabled( true );
    apis = Plasma::knownLanguages( currentType );
    apis.append( QString() ); // Add empty string for native

    kDebug() << "Got apis " << apis;
    // Map to friendly names (TODO: fix in library)
    QString api;
    QStringList apiNames;

    foreach( const QString &api, apis ) {
	if ( api == QString("dashboard") )
	    apiNames.append("Dashboard");
	else if ( api == QString("javascript") )
	    apiNames.append("Javascript");
	else if ( api == QString("ruby-script") )
	    apiNames.append("Ruby");
	else if ( api == QString("webkit") )
	    apiNames.append("Webkit");
	else if ( api == QString() )
	    apiNames.append("C++");
	else {
	    kWarning() << "Unknown API " << api;
	    apiNames.append( api );
	}
    }

    view->api_combo->clear();
    view->api_combo->insertItems( 0, apiNames );

    int idx = view->api_combo->findText(metadata->implementationApi());
    if ( idx != -1 ) {
        view->api_combo->setCurrentIndex( idx );
    }
    else if ( metadata->implementationApi().isEmpty() ) {
        view->api_combo->setCurrentIndex( view->api_combo->count()-1 ); // Native
    }
    else {
        kWarning() << "Unknown api detected " << metadata->implementationApi();
        view->api_combo->setCurrentIndex( view->api_combo->count()-1 ); // C++ is last
    }

}

void MetaDataEditor::writeFile()
{
    metadata->setName( view->name_edit->text() );
    metadata->setDescription( view->comment_edit->text() );

    //TODO
    //desktopGroup.writeEntry( "Icon", view->icon_edit->text() );

    metadata->setCategory( categories[view->category_combo->currentIndex()] );
    metadata->setImplementationApi( apis[view->api_combo->currentIndex()] );
    metadata->setPluginName( view->pluginname_edit->text() );
    metadata->setVersion( view->version_edit->text() );
    metadata->setWebsite( view->website_edit->text() );
    metadata->setAuthor( view->author_edit->text() );
    metadata->setEmail( view->email_edit->text() );
    metadata->setLicense( view->license_edit->text() );

    metadata->write(filename);
}
