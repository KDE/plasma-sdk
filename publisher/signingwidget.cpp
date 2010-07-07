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
#include <QTreeWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <KGlobal>
#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <KIcon>

#include <gpgme++/context.h>

#include "signingwidget.h"



SigningWidget::SigningWidget()
        : QWidget(0),
        m_treeWidget(0)
{
    loadConfig();
    initUI();
    initGpgContext();
    loadKeys();
}

bool SigningWidget::signingEnabled() const
{
    return m_signingEnabled;
}

bool SigningWidget::sign(const KUrl &path) const
{
    return false;
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
            this, SLOT(createKey()));
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
    if(!m_gpgContext) {
        m_contextInitialized = true;
    }
}

QStringList SigningWidget::gpgEntryList(const bool privateKeysOnly) const
{
    c->startKeyListing("", privateKeysOnly ? 1 : 0);
    QStringList result;
    while(!error) {
        GpgME::Key k = c->nextKey(error);
        if(error)
            break;
        result << k.keyID();
    }
    return result;
}

void SigningWidget::setEnabled(const bool enabled)
{
    m_signingEnabled = enabled;
    KConfigGroup cg = KGlobal::config()->group("Signing Options");
    cg.writeEntry("signingEnabled", m_signingEnabled);
    cg.sync();

    m_treeWidget->setEnabled(m_signingEnabled);
    m_createKeyButton->setEnabled(false/*m_signingEnabled*/);
    m_deleteKeyButton->setEnabled(false/*m_signingEnabled*/); // Waiting for the damn qca patch again ...
}

void SigningWidget::createKey()
{

}

void SigningWidget::deleteKey()
{
    if (m_currentKey.isEmpty() || m_currentKey.isNull())
        return;

    QList<QCA::KeyStoreEntry> entries = m_userKeyStore->entryList();
    foreach(QCA::KeyStoreEntry entry, entries) {
        if (!entry.pgpSecretKey().isNull()) {
            kDebug() << "the key is not null.";
            if (m_currentKey.contains(entry.pgpSecretKey().primaryUserId())) {
                kDebug() << "Removing : " << entry.id();
                if (m_userKeyStore->removeEntry(entry.pgpSecretKey().keyId())) {
                    kDebug() << "Successfully removed key " << m_currentKey;
                    m_currentKey.clear();
                    return;
                }
                kDebug() << "Failed removal of key " << m_currentKey  << "with fingerprint " << entry.pgpSecretKey().fingerprint();
                return;
            }
        }
    }
}

void SigningWidget::loadKeys()
{
    kDebug() << "loading keys ...";
    m_treeWidget->clear();
    m_strings.clear();
    QList< QCA::KeyStoreEntry > entries = m_userKeyStore->entryList();
    foreach(QCA::KeyStoreEntry entry, entries) {
        if (!entry.pgpSecretKey().isNull()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget);
            QRadioButton *button = new QRadioButton(entry.pgpSecretKey().primaryUserId(), this);
            if (!m_currentKey.isNull() || m_currentKey.isEmpty()) {
                if (m_currentKey == entry.pgpSecretKey().primaryUserId())
                    button->setChecked(true);
            }
            m_treeWidget->setItemWidget(item, 0, button);

            m_strings << entry.pgpSecretKey().primaryUserId();

            connect(button, SIGNAL(clicked(bool)),
                    this, SLOT(updateCurrentKey()));
        }
    }
}

void SigningWidget::updateCurrentKey()
{
    QRadioButton *sender = static_cast<QRadioButton *>(QObject::sender());
    for (int i = 0; i < sender->text().size(); ++i) {
        int index = sender->text().indexOf('&', i);
        QString keyInfo;
        if (index > 0) {
            keyInfo.append(sender->text().left(index));
            keyInfo.append(sender->text().right(index));
        } else if (index == 0) {
            keyInfo.append(sender->text().mid(1));
        } else {
            break;
        }
        i = index;
        m_currentKey = keyInfo;
        KConfigGroup cg = KGlobal::config()->group("Signing Options");
        cg.writeEntry("currentSignerKey", m_currentKey);
        cg.sync();
        kDebug() << "current key = " << m_currentKey;
        break;
    }
}


#include "moc_signingwidget.cpp"
