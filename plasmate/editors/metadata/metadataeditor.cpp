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
#include <QDebug>
#include <QRegExpValidator>

#include <KIconButton>
#include <KDesktopFile>
#include <KConfigGroup>
#include <KMessageWidget>
#include <klocalizedstring.h>

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

void MetaDataEditor::setFilename(const QString &filePath)
{
    m_metadata->setFilePath(filePath);
}

const QString MetaDataEditor::filename()
{
    return m_metadata->filePath();
}

void MetaDataEditor::readFile()
{
    m_view->name_edit->setText(m_metadata->name());
    m_view->comment_edit->setText(m_metadata->description());
    m_view->icon_button->setIcon(m_metadata->icon());

    m_view->pluginname_edit->setText(m_metadata->pluginName());

    const QString serviceType = m_metadata->serviceTypes().at(0);

    initCatergories(serviceType);

    m_view->type_combo->clear();
    m_view->type_combo->setEnabled(false);
    if (serviceType == QStringLiteral("Plasma/DataEngine")) {
        m_view->type_combo->insertItem(0, i18n("Data Engine"));
        m_metadata->setServiceTypes(QStringList() << QStringLiteral("Plasma/DataEngine"));
    } else if (serviceType == QStringLiteral("Plasma/Theme")) {
        m_view->type_combo->insertItem(0, i18n("Theme"));
        m_metadata->setServiceTypes(QStringList() << QStringLiteral("Plasma/Theme"));
    } else if (serviceType == QStringLiteral("Plasma/Runner")) {
        m_view->type_combo->insertItem(0, i18n("Runner"));
        m_metadata->setServiceTypes(QStringList() << QStringLiteral("Plasma/Runner"));
    } else if (serviceType == QStringLiteral("KWin/WindowSwitcher")) {
        m_view->type_combo->insertItem(0,i18n("Window Switcher"));
        m_metadata->setServiceTypes(QStringList() << QStringLiteral("KWin/WindowSwitcher"));
    } else if (serviceType == QStringLiteral("KWin/Script")) {
        m_view->type_combo->insertItem(0, i18n("KWin Script"));
        m_metadata->setServiceTypes(QStringList() << QStringLiteral("KWin/Script"));
    } else if (serviceType == QStringLiteral("KWin/Effect")) {
        m_view->type_combo->insertItem(0, i18n("KWin Effect"));
        m_metadata->setServiceTypes(QStringList() << QStringLiteral("KWin/Effect"));
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
    m_view->api_combo->setEnabled(false); // disallow dynamic changing of api
    QStringList apis;
    apis << QStringLiteral("declarativeappletscript") << QStringLiteral("javascript");

    //add  api from the metadata.desktop inside the api
    m_view->label_16->setVisible(false);
    m_view->api_combo->clear();
    m_view->api_combo->insertItems(0, QStringList() << m_metadata->pluginApi());
    if (m_view->api_combo->currentIndex() == -1 || m_metadata->pluginApi().isEmpty()) {
        m_view->label_16->setVisible(true);
        m_view->api_combo->clear();
        m_view->api_combo->insertItems(0, apis);
        m_view->api_combo->setEnabled(true);
    }
}

void MetaDataEditor::writeFile()
{
    QString api = m_view->api_combo->currentText();
    m_metadata->setName(m_view->name_edit->text());
    m_metadata->setDescription(m_view->comment_edit->text());
    m_metadata->setIcon(m_view->icon_button->icon());

    m_metadata->setCategory(m_categories[m_view->category_combo->currentIndex()]);
    m_metadata->setPluginApi(api);
    m_metadata->setPluginName(m_view->pluginname_edit->text());
    m_metadata->setVersion(m_view->version_edit->text());
    m_metadata->setWebsite(m_view->website_edit->text());
    m_metadata->setAuthor(m_view->author_edit->text());
    m_metadata->setEmail(m_view->email_edit->text());

    m_metadata->setLicense(m_view->license_edit->text());
    emit apiChanged();
    m_metadata->writeFile();
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
