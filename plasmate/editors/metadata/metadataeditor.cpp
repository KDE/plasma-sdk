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
#include <QDebug>
#include <KMessageWidget>

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
        m_metadata(0)
{
    m_view = new Ui::MetaDataEditor;
    m_view->setupUi(this);

    m_view->label_16->setText(i18n("Plasmate cannot detect the correct API. Please choose one from the above list"));
    m_view->label_16->setMessageType(KMessageWidget::Error);
    m_view->label_16->setCloseButtonVisible(false);

    connect(m_view->type_combo, SIGNAL(currentIndexChanged(int)), SLOT(serviceTypeChanged()));
}

MetaDataEditor::~MetaDataEditor()
{
    delete m_metadata;
}

void MetaDataEditor::setFilename(const QString &filename)
{
    m_filename = filename;
}

const QString MetaDataEditor::filename()
{
    return m_filename;
}

bool MetaDataEditor::isValidMetaData()
{
    qDebug() << "readFile file" << m_filename;

    delete m_metadata;
    m_metadata = new Plasma::PackageMetadata(m_filename);
    return m_metadata->isValid();

}


void MetaDataEditor::readFile()
{
    if (!this->isValidMetaData()) {
      qWarning() << "MetaData cannot read the file because the filename:" + m_filename + "is not valid.";
      return;
    }

    m_view->name_edit->setText(m_metadata->name());
    m_view->comment_edit->setText(m_metadata->description());
    m_view->icon_button->setIcon(m_metadata->icon());

    m_view->pluginname_edit->setText(m_metadata->pluginName());

    QString serviceType = m_metadata->serviceType();

    initCatergories(serviceType);

    m_view->type_combo->clear();
    m_view->type_combo->setEnabled(false);
    if (serviceType == "Plasma/DataEngine") {
        m_view->type_combo->insertItem(0, i18n("Data Engine"));
        m_metadata->setServiceType("Plasma/DataEngine");
    } else if (serviceType == "Plasma/Theme") {
        m_view->type_combo->insertItem(0, i18n("Theme"));
        m_metadata->setServiceType("Plasma/Theme");
    } else if (serviceType == "Plasma/Runner") {
        m_view->type_combo->insertItem(0, i18n("Runner"));
        m_metadata->setServiceType("Plasma/Runner");
    } else if (serviceType == "KWin/WindowSwitcher") {
        m_view->type_combo->insertItem(0,i18n("Window Switcher"));
        m_metadata->setServiceType("KWin/WindowSwitcher");
    } else if (serviceType == "KWin/Script") {
        m_view->type_combo->insertItem(0, i18n("KWin Script"));
        m_metadata->setServiceType("KWin/Script");
    } else if (serviceType == "KWin/Effect") {
        m_view->type_combo->insertItem(0, i18n("KWin Effect"));
        m_metadata->setServiceType("KWin/Effect");
    } else {
        m_view->type_combo->insertItem(0, i18n("Applet"));
        m_view->type_combo->insertItem(1, i18n("Popup Applet"));
        m_view->type_combo->setEnabled(true);
        if (serviceType == "Plasma/Applet") {
            m_view->type_combo->setCurrentIndex(0);
        } else {
            m_view->type_combo->setCurrentIndex(1);
        }
        serviceTypeChanged();
    }

    // Enforce the security restriction from package.cpp in the input field
    QRegExpValidator *pluginname_validator = new QRegExpValidator(m_view->pluginname_edit);
    QRegExp validatePluginName("^[\\w-\\.]+$"); // Only allow letters, numbers, underscore and period.
    pluginname_validator->setRegExp(validatePluginName);
    m_view->pluginname_edit->setValidator(pluginname_validator);

    int idx = m_view->category_combo->findText(m_metadata->category());
    if (idx != -1) {
        m_view->category_combo->setCurrentIndex(idx);
    } else {
        qWarning() << "Unknown category detected " << m_metadata->category() << "using miscellaneous instead";
        m_view->category_combo->setCurrentIndex(m_view->category_combo->count() - 1); // misc is last
    }

    m_view->version_edit->setText(m_metadata->version());
    m_view->website_edit->setText(m_metadata->website());
    m_view->author_edit->setText(m_metadata->author());
    m_view->email_edit->setText(m_metadata->email());
    m_view->license_edit->setText(m_metadata->license());
    m_view->api_combo->setCurrentIndex(0);
}

const QString MetaDataEditor::api()
{
    readFile();
    return m_view->api_combo->currentText();
}

void MetaDataEditor::serviceTypeChanged()
{
    Plasma::ComponentTypes currentType;

    switch (m_view->type_combo->currentIndex()) {
    case 0:
        m_metadata->setServiceType("Plasma/Applet");
        currentType = Plasma::AppletComponent;
        break;
    case 1:
        m_metadata->setServiceType("Plasma/Applet,Plasma/PopupApplet");
        currentType = Plasma::AppletComponent;
        break;
    /*case 2:   // only applet/popupapplet can be dynamically selected now
        m_metadata->setServiceType("Plasma/DataEngine");
        currentType = Plasma::DataEngineComponent;
        break;
    case 3:
        m_metadata->setServiceType("Plasma/Theme");
        m_view->api_combo->setEnabled(false);
        return;
        break;
    case 4:
        m_metadata->setServiceType("Plasma/Runner");
        currentType = Plasma::RunnerComponent;
        break;*/
    default:
        qWarning() << "Unknown service type" << currentType;
        return;
    }

    m_view->api_combo->setEnabled(false); // disallow dynamic changing of api
    //FIXME: shouldn't need to do most of the stuff below anymore
    m_apis = Plasma::knownLanguages(currentType);
    m_apis.append(QString());   // Add empty string for native

    qDebug() << "Got m_apis " << m_apis;
    // Map to friendly names (TODO: fix in library)

    //add  api from the metadata.desktop inside the api
    KConfig filenamePath(m_filename);
    KConfigGroup metaFilePreferences(&filenamePath, "Desktop Entry");
    QString api = metaFilePreferences.readEntry("X-Plasma-API");
    QStringList apiName;
    apiName << formatApi(api, MetaDataEditor::uiApi);
    m_view->label_16->setVisible(false);
    m_view->api_combo->clear();
    m_view->api_combo->insertItems(0, apiName);
    if (m_view->api_combo->currentIndex() == -1 || apiName.first().isEmpty()) {
        m_view->label_16->setVisible(true);
        m_view->api_combo->clear();
        m_view->api_combo->insertItems(0, m_apis);
        m_view->api_combo->setEnabled(true);
    }
}

QString MetaDataEditor::formatApi(const QString &api,  apiModes apiMode)
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
    qWarning() << "Unknown API " << api << apiMode;
    return QString(api);
}

void MetaDataEditor::writeFile()
{
    QString api = m_view->api_combo->currentText();
    m_metadata->setName(m_view->name_edit->text());
    m_metadata->setDescription(m_view->comment_edit->text());
    m_metadata->setIcon(m_view->icon_button->icon());

    m_metadata->setCategory(m_categories[m_view->category_combo->currentIndex()]);
    m_metadata->setImplementationApi(formatApi(api, MetaDataEditor::coreApi));
    m_metadata->setPluginName(m_view->pluginname_edit->text());
    m_metadata->setVersion(m_view->version_edit->text());
    m_metadata->setWebsite(m_view->website_edit->text());
    m_metadata->setAuthor(m_view->author_edit->text());
    m_metadata->setEmail(m_view->email_edit->text());

    m_metadata->setLicense(m_view->license_edit->text());
    emit apiChanged();
    m_metadata->write(m_filename);
    //TODO: alert the necessary components (eg. packagemodel) if plugin type/api is changed
}

void MetaDataEditor::initCatergories(const QString& serviceType)
{
    m_categories.clear();
    m_view->category_combo->clear();

    if (serviceType == "KWin/Effect") {
        m_categories << QString("Accessibility")
        << QString("Appearance")
        << QString("Candy")
        << QString("Focus")
        << QString("Tools")
        << QString("Window Management");
    } else {
        // This list contains the untranslated version of the m_categories so that we
        // write the correct version into the package metadata. The list of allowed m_categories can be
        // found at http://techbase.kde.org/Projects/Plasma/PIG#Category_Names
        m_categories << QString("Application Launchers")
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

    //now add them in the combobox
    m_view->category_combo->addItems(m_categories);
}
