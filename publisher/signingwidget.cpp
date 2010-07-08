/*
 *  Copyright (C) 2010 by Diego '[Po]lentino' Casella <polentino911@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <QCheckBox>
#include <QRadioButton>
#include <QDialog>
#include <QLabel>
#include <QTreeWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <KGlobal>
#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <KIcon>

#define _FILE_OFFSET_BITS 64

#include <gpgme.h>
#include <qgpgme/dataprovider.h>
#include <gpgme++/data.h>
#include <gpgme++/engineinfo.h>
#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>
#include <gpgme++/context.h>

#include "signingwidget.h"
#include "signingdialog.h"



SigningWidget::SigningWidget()
        : QWidget(0),
        m_treeWidget(0)
{
    loadConfig();
    initUI();
    initGpgContext();
    loadKeys();
}

SigningWidget::~SigningWidget()
{
    //release context
}

void SigningWidget::loadConfig()
{
    KConfigGroup cg = KGlobal::config()->group("Signing Options");
    m_signingEnabled = cg.readEntry("signingEnabled", false);
    m_currentKey = cg.readEntry("currentSignerKey", QString());
}

void SigningWidget::initUI()
{
    QVBoxLayout *mainlLayout = new QVBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderLabel("Select one key from the list below:");

    m_createKeyButton = new QPushButton(this);
    m_createKeyButton->setText("Create new Key ...");
    m_createKeyButton->setIcon(KIcon("dialog-password"));

    m_deleteKeyButton = new QPushButton(this);
    m_deleteKeyButton->setText("Delete selected key");
    m_deleteKeyButton->setIcon(KIcon("edit-delete"));

    m_signCheckBox = new QCheckBox("Enable plasmoid signing.", this);
    m_signCheckBox->setChecked(m_signingEnabled);

    buttonLayout->addWidget(m_createKeyButton);
    buttonLayout->addWidget(m_deleteKeyButton);

    mainlLayout->addWidget(m_signCheckBox);
    mainlLayout->addWidget(m_treeWidget);
    mainlLayout->addLayout(buttonLayout);
    setLayout(mainlLayout);

    setEnabled(m_signingEnabled);

    connect(m_createKeyButton, SIGNAL(clicked()),
            this, SLOT(showCreateKeyDialog()));
    connect(m_deleteKeyButton, SIGNAL(clicked()),
            this, SLOT(deleteKey()));
    connect(m_signCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(setEnabled(bool)));
}

void SigningWidget::initGpgContext()
{
    GpgME::initializeLibrary();
    GpgME::Error error = GpgME::checkEngine(GpgME::OpenPGP);
    if (error) {
        kDebug() << "OpenPGP not supported!";
        m_contextInitialized = false;
        return;
    }

    m_gpgContext = GpgME::Context::createForProtocol(GpgME::OpenPGP);
    if (!m_gpgContext) {
        m_contextInitialized = true;
    }
}

QList< QMap<QString, QVariant> > SigningWidget::gpgEntryList(const bool privateKeysOnly) const
{
    GpgME::Error error = m_gpgContext->startKeyListing("", privateKeysOnly ? 1 : 0);
    QList< QMap<QString, QVariant> > result;
    while (!error) {
        GpgME::Key k = m_gpgContext->nextKey(error);
        if (error)
            break;
        QMap<QString, QVariant> tmp;
        tmp.insert("name", k.userID(0).name());
        tmp.insert("comment", k.userID(0).comment());
        tmp.insert("email", k.userID(0).email());
        tmp.insert("id", k.keyID());
        result << tmp;
    }
    return result;
}

QString SigningWidget::mapKeyToString(const QMap<QString, QVariant> &map,  const bool appendKeyId) const
{
    QString result;
    result.append(map["name"].toString() + " ");
    if (!map["comment"].toString().isEmpty())
        result.append("(" + map["comment"].toString() + ") ");
    result.append(map["email"].toString());
    if (appendKeyId)
        result.append(" , " + map["id"].toString());
    return result;
}

void SigningWidget::setEnabled(const bool enabled)
{
    m_signingEnabled = enabled;
    KConfigGroup cg = KGlobal::config()->group("Signing Options");
    cg.writeEntry("signingEnabled", m_signingEnabled);
    cg.sync();

    m_treeWidget->setEnabled(m_signingEnabled);
    m_createKeyButton->setEnabled(m_signingEnabled);
    m_deleteKeyButton->setEnabled(m_signingEnabled);
}

bool SigningWidget::signingEnabled() const
{
    return m_signingEnabled;
}

bool SigningWidget::sign(const KUrl &path) const
{
    return false;
}

void SigningWidget::showCreateKeyDialog()
{
    SigningDialog *dialog = new SigningDialog(this);
    dialog->exec();
    connect(dialog, SIGNAL(emitCreateKey(const QString&)),
            this, SLOT(createKey(const QString&)));
}

void SigningWidget::createKey(const QString &param)
{
    kDebug() << "READY TO CREATE ;)";
}

void SigningWidget::deleteKey()
{
    if (m_currentKey.isEmpty() || m_currentKey.isNull())
        return;

    GpgME::Error error = m_gpgContext->startKeyListing("", 1);
    QList< QMap<QString, QVariant> > result;
    while (!error) {
        GpgME::Key k = m_gpgContext->nextKey(error);
        if (error)
            break;
        if (m_currentKey.contains(k.keyID())) {
            error = m_gpgContext->deleteKey(k, true);
            if (!error) {
                m_currentKey.clear();
                KConfigGroup cg = KGlobal::config()->group("Signing Options");
                cg.writeEntry("currentSignerKey", m_currentKey);
                cg.sync();
                loadKeys();
            }
            return;
        }
    }
    return;
}

void SigningWidget::loadKeys()
{
    m_treeWidget->clear();
    QList< QMap<QString, QVariant> > entries = gpgEntryList(true);
    for (int i = 0; i < entries.count(); ++i) {
        QMap<QString, QVariant> entry = entries.at(i);
        QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget);
        QRadioButton *button = new QRadioButton(mapKeyToString(entry, false), this);
        button->setObjectName(mapKeyToString(entry));
        if (!m_currentKey.isNull() || m_currentKey.isEmpty()) {
            if (m_currentKey == mapKeyToString(entry))
                button->setChecked(true);
        }
        m_treeWidget->setItemWidget(item, 0, button);

        connect(button, SIGNAL(clicked(bool)),
                this, SLOT(updateCurrentKey()));
    }
}

void SigningWidget::updateCurrentKey()
{
    QRadioButton *sender = static_cast<QRadioButton *>(QObject::sender());

    m_currentKey = sender->objectName();
    kDebug() << m_currentKey;
    KConfigGroup cg = KGlobal::config()->group("Signing Options");
    cg.writeEntry("currentSignerKey", m_currentKey);
    cg.sync();
}


#include "moc_signingwidget.cpp"
