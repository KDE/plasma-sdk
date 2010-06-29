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

#include <QRadioButton>
#include <KDebug>


#include "signingwidget.h"



SigningWidget::SigningWidget()
        : QTreeWidget(0)
{
    QCA::scanForPlugins();
    QCA::ProviderList qcaProviders = QCA::providers();
    bool hasGPGPlugin = false;
    for (int i = 0; i < qcaProviders.size(); ++i) {
        if (qcaProviders[i]->name().contains("qca-gnupg")) {
            hasGPGPlugin = true;
            break;
        }
    }

    if (!hasGPGPlugin) {
        kDebug() << "qca-gnupg plugin for QCA not foud: can't sign the plasmoids.";
        return;
    }


    // Ensure QCA supports the public key and keystorelist plugin
    if (!QCA::isSupported("pkey")) {
        kDebug() << "QCA has no pkey support: can't sign the plasmoids.";
        return;
    }
    if (!QCA::isSupported("keystorelist")) {
        kDebug() << "QCA has no keystorelist support: can't retrieve the pgp keys in the keystore.";
        return;
    }

    // Start the keystore manager, and get access to the user keystore
    m_keyStoreManager.start();
    m_keyStoreManager.waitForBusyFinished(); // TODO: better connect to finished() signal?
    const QString id("qca-gnupg");
    m_userKeyStore = new QCA::KeyStore(id, &m_keyStoreManager);


    loadKeys();

    connect(m_userKeyStore, SIGNAL(updated()),
            this, SLOT(loadKeys()));
}

void SigningWidget::loadKeys()
{
    kDebug() << "loading keys ...";
    clear();
    QList< QCA::KeyStoreEntry > entries = m_userKeyStore->entryList();
    foreach(QCA::KeyStoreEntry entry, entries) {
        if (!entry.pgpSecretKey().isNull()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(this);
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            QRadioButton *button = new QRadioButton(entry.pgpSecretKey().primaryUserId(), this);
            setItemWidget(item, 0, button);
        }
    }
}

#include "moc_signingwidget.cpp"
