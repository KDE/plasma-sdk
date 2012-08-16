/*
  Copyright 2009 Richard Moore, <rich@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
#include "../../packagemodel.h"

//
// TODO: Now we know what this does it needs rewriting to use the metadata object
// as its main store. And to handle updates in a clean per-change way rather than
// the hacks that are there right now.
//

MetaDataEditor::MetaDataEditor(QWidget *parent)
        : QWidget(parent),
        metadata(0)
{
    view = new Ui::MetaDataEditor;
    view->setupUi(this);

    connect(view->type_combo, SIGNAL(currentIndexChanged(int)), SLOT(serviceTypeChanged()));

    // This list contains the untranslated version of the categories so that we
    // write the correct version into the package metadata. The list MUST be kept
    // in sync with the one in metadata.ui. The list of allowed categories can be
    // found at http://techbase.kde.org/Projects/Plasma/PIG#Category_Names
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
}

MetaDataEditor::~MetaDataEditor()
{
    delete metadata;
}

void MetaDataEditor::setFilename(const QString &filename)
{
    this->m_filename = filename;
}

const QString MetaDataEditor::filename()
{
    return this->m_filename;
}

bool MetaDataEditor::isValidMetaData()
{
    kDebug() << "readFile file" << m_filename;

    delete metadata;
    metadata = new Plasma::PackageMetadata(m_filename);
    return metadata->isValid();

}


void MetaDataEditor::readFile()
{
    if (!this->isValidMetaData()) {
      kWarning() << "MetaData cannot read the file because the filename:" + m_filename + "is not valid.";
      return;
    }

    view->name_edit->setText(metadata->name());
    view->comment_edit->setText(metadata->description());
    view->icon_button->setIcon(metadata->icon());

    view->pluginname_edit->setText(metadata->pluginName());

    QString serviceType = metadata->serviceType();

    view->type_combo->clear();
    view->type_combo->setEnabled(false);
    if (serviceType == "Plasma/DataEngine") {
        view->type_combo->insertItem(0, i18n("Data Engine"));
        metadata->setServiceType("Plasma/DataEngine");
    } else if (serviceType == "Plasma/Theme") {
        view->type_combo->insertItem(0, i18n("Theme"));
        metadata->setServiceType("Plasma/Theme");
    } else if (serviceType == "Plasma/Runner") {
        view->type_combo->insertItem(0, i18n("Runner"));
        metadata->setServiceType("Plasma/Runner");
    } else if (serviceType == "KWin/WindowSwitcher") {
        view->type_combo->insertItem(0,i18n("Window Switcher"));
        metadata->setServiceType("KWin/WindowSwitcher");
    } else if (serviceType == "KWin/Script") {
        view->type_combo->insertItem(0, i18n("KWin Script"));
        metadata->setServiceType("KWin/Script");
    } else if (serviceType == "KWin/Effect") {
        view->type_combo->insertItem(0, i18n("KWin Effect"));
        metadata->setServiceType("KWin/Effect");
    } else {
        view->type_combo->insertItem(0, i18n("Applet"));
        view->type_combo->insertItem(1, i18n("Popup Applet"));
        view->type_combo->setEnabled(true);
        if (serviceType == "Plasma/Applet") {
            view->type_combo->setCurrentIndex(0);
        } else {
            view->type_combo->setCurrentIndex(1);
        }
        serviceTypeChanged();
    }

    // Enforce the security restriction from package.cpp in the input field
    QRegExpValidator *pluginname_validator = new QRegExpValidator(view->pluginname_edit);
    QRegExp validatePluginName("^[\\w-\\.]+$"); // Only allow letters, numbers, underscore and period.
    pluginname_validator->setRegExp(validatePluginName);
    view->pluginname_edit->setValidator(pluginname_validator);

    int idx = view->category_combo->findText(metadata->category());
    if (idx != -1) {
        view->category_combo->setCurrentIndex(idx);
    } else {
        kWarning() << "Unknown category detected " << metadata->category() << "using miscellaneous instead";
        view->category_combo->setCurrentIndex(view->category_combo->count() - 1); // misc is last
    }

    view->version_edit->setText(metadata->version());
    view->website_edit->setText(metadata->website());
    view->author_edit->setText(metadata->author());
    view->email_edit->setText(metadata->email());
    view->license_edit->setText(metadata->license());
    view->api_combo->setCurrentIndex(0);
}

const QString MetaDataEditor::api()
{
    readFile();
    return view->api_combo->currentText();
}

void MetaDataEditor::serviceTypeChanged()
{
    Plasma::ComponentTypes currentType;

    switch (view->type_combo->currentIndex()) {
    case 0:
        metadata->setServiceType("Plasma/Applet");
        currentType = Plasma::AppletComponent;
        break;
    case 1:
        metadata->setServiceType("Plasma/Applet,Plasma/PopupApplet");
        currentType = Plasma::AppletComponent;
        break;
    /*case 2:   // only applet/popupapplet can be dynamically selected now
        metadata->setServiceType("Plasma/DataEngine");
        currentType = Plasma::DataEngineComponent;
        break;
    case 3:
        metadata->setServiceType("Plasma/Theme");
        view->api_combo->setEnabled(false);
        return;
        break;
    case 4:
        metadata->setServiceType("Plasma/Runner");
        currentType = Plasma::RunnerComponent;
        break;*/
    default:
        kWarning() << "Unknown service type" << currentType;
        return;
    }

    view->api_combo->setEnabled(false); // disallow dynamic changing of api
    //FIXME: shouldn't need to do most of the stuff below anymore
    apis = Plasma::knownLanguages(currentType);
    apis.append(QString());   // Add empty string for native

    kDebug() << "Got apis " << apis;
    // Map to friendly names (TODO: fix in library)

    //add  api from the metadata.desktop inside the api
    KConfig filenamePath(m_filename);
    KConfigGroup metaFilePreferences(&filenamePath, "Desktop Entry");
    QString api = metaFilePreferences.readEntry("X-Plasma-API");
    QStringList apiName;
    apiName << formatApi(api, MetaDataEditor::uiApi);
    view->label_16->setVisible(false);
    view->api_combo->clear();
    view->api_combo->insertItems(0, apiName);
    if (view->api_combo->currentIndex() == -1 || apiName.first().isEmpty()) {
        view->label_16->setVisible(true);
	view->api_combo->clear();
	view->api_combo->insertItems(0, apis);
	view->api_combo->setEnabled(true);
    }
}

QString MetaDataEditor::formatApi(QString &api,  apiModes apiMode)
{
    if (apiMode == MetaDataEditor::uiApi) {
        if (api == QString("dashboard")) {
            return QString("Dashboard");
	} else if (api == QString("javascript")) {
            return QString("Javascript");
	} else if (api == QString("ruby-script")) {
            return QString("Ruby");
	} else if (api == QString("webkit")) {
            return QString("Webkit");
	} else if (api == QString("python")) {
            return QString("Python");
	} else if (api == QString("declarativeappletscript")) {
            return QString("declarativeappletscript");
        }
    } else if (apiMode == MetaDataEditor::coreApi) {
        if (api == QString("Dashboard")) {
            return QString("dashboard");
	} else if (api == QString("Javascript")) {
            return QString("javascript");
	} else if (api == QString("Ruby")) {
            return QString("ruby-script");
	} else if (api == QString("Webkit")) {
            return QString("webkit");
	} else if (api == QString("Python")) {
	    return QString("python");
	} else if (api == QString("declarativeappletscript")) {
            return QString("declarativeappletscript");
        }
    }
    kWarning() << "Unknown API " << api << apiMode;
    return QString(api);
}

void MetaDataEditor::writeFile()
{
    QString api = view->api_combo->currentText();
    metadata->setName(view->name_edit->text());
    metadata->setDescription(view->comment_edit->text());
    metadata->setIcon(view->icon_button->icon());

    metadata->setCategory(categories[view->category_combo->currentIndex()]);
    metadata->setImplementationApi(formatApi(api, MetaDataEditor::coreApi));
    metadata->setPluginName(view->pluginname_edit->text());
    metadata->setVersion(view->version_edit->text());
    metadata->setWebsite(view->website_edit->text());
    metadata->setAuthor(view->author_edit->text());
    metadata->setEmail(view->email_edit->text());

    metadata->setLicense(view->license_edit->text());
    emit apiChanged();
    metadata->write(m_filename);
    //TODO: alert the necessary components (eg. packagemodel) if plugin type/api is changed
}
