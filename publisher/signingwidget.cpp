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
#include <QFile>
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
#include <KLineEdit>
#include <KPushButton>

#define _FILE_OFFSET_BITS 64

#include <gpgme.h>
#include <qgpgme/dataprovider.h>
#include <gpgme++/data.h>
#include <gpgme++/engineinfo.h>
#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>
#include <gpgme++/keygenerationresult.h>
#include <gpgme++/context.h>
#include <gpgme++/interfaces/passphraseprovider.h>
#include <gpgme++/signingresult.h>

#include <cstdio> // FILE

#include "signingwidget.h"
#include "signingdialog.h"

namespace GpgME
{
class PasswordAsker : public PassphraseProvider, QDialog
{
public:
    PasswordAsker(const QWidget *parent)
            : QDialog(0) {
        setModal(true);
        QVBoxLayout *main = new QVBoxLayout(this);
        QHBoxLayout *child = new QHBoxLayout(this);

        m_infoLabel = new QLabel(this);
        main->addWidget(m_infoLabel);

        m_pwdLine = new KLineEdit(this);
        main->addWidget(m_pwdLine);

        m_submitButton = new KPushButton("Submit", this);
        m_cancelButton = new KPushButton("Cancel", this);
        child->addWidget(m_submitButton);
        child->addWidget(m_cancelButton);

        m_pwdLine->setEchoMode(QLineEdit::Password);
        m_submitButton->setIcon(KIcon("dialog-ok"));
        m_cancelButton->setIcon(KIcon("dialog-cancel"));

        main->addLayout(child);

        this->hide();

        connect(m_submitButton, SIGNAL(clicked()),
                this, SLOT(close()));
        connect(m_cancelButton, SIGNAL(clicked()),
                this, SLOT(close()));
    }

    void clear() {
        m_pwdLine->clear();
    }

    char * getPassphrase(const char * useridHint, const char * description, bool previousWasBad, bool & canceled) {
        m_infoLabel->setText("Set Password for:\n" + QString(useridHint));
        this->exec();
        return strdup(m_pwdLine->text().append("\n").toAscii().data());
    }

private:
    QLabel *m_infoLabel;
    KLineEdit *m_pwdLine;
    KPushButton *m_submitButton;
    KPushButton *m_cancelButton;
};
}



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
    delete m_gpgContext;
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
    m_createKeyButton->setEnabled(false);

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

    m_pwdAsker = new GpgME::PasswordAsker(this);
    m_gpgContext->setKeyListMode(GPGME_KEYLIST_MODE_LOCAL | GPGME_KEYLIST_MODE_SIGS);
    m_gpgContext->setPassphraseProvider(m_pwdAsker);
    m_gpgContext->setArmor(true);
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
    GpgME::KeyListResult lRes = m_gpgContext->endKeyListing();
    if (lRes.error()) {
        kDebug() << "Error while ending the keyListing operation: " << lRes.error().asString();
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
    //m_createKeyButton->setEnabled(m_signingEnabled);
    m_deleteKeyButton->setEnabled(m_signingEnabled);
}

bool SigningWidget::signingEnabled() const
{
    return m_signingEnabled;
}

bool SigningWidget::sign(const KUrl &path)
{
    // Ensure we have a key set
    if (m_currentKey.isEmpty() ||  m_currentKey.isNull())
        return false;

    m_gpgContext->clearSigningKeys();

    QString hash = m_currentKey.split(" ",QString::SkipEmptyParts).last();

    // Lets start looking for the key
    GpgME::Error error = m_gpgContext->startKeyListing("", true);
    while (!error) {
        GpgME::Key k = m_gpgContext->nextKey(error);
        if (error)
            break;

        QString fingerprint(k.subkey(0).fingerprint());
        if (fingerprint.contains(hash)) {
            m_gpgContext->addSigningKey(k);
            kDebug() << "Added signer: " << k.subkey(0).fingerprint();
            break;
        }
    }
    GpgME::KeyListResult lRes = m_gpgContext->endKeyListing();
    if (lRes.error()) {
        kDebug() << "Error while ending the keyListing operation: " << lRes.error().asString();
    }


    FILE *fp;
    fp = fopen(QFile::encodeName(path.pathOrUrl()).data(), "r");
    FILE *fp1;
    fp1 = fopen(QFile::encodeName(path.pathOrUrl().append(".asc")).data(), "w");
    GpgME::Data plasmoidata(fp);
    GpgME::Data signature(fp1);

    kDebug() << "Ready to sign: " << path.pathOrUrl();

    GpgME::SigningResult sRes = m_gpgContext->sign(plasmoidata, signature, GpgME::Detached);
    error = m_gpgContext->startSigning(plasmoidata, signature, GpgME::Detached);
    kDebug() <<"Signing result: " << m_gpgContext->signingResult().createdSignature(0).fingerprint();

    m_pwdAsker->clear();
    fclose(fp1);
    fclose(fp);

    if (!error) {
        return true;
    }
    return false;
}

void SigningWidget::showCreateKeyDialog()
{
    SigningDialog *dialog = new SigningDialog(this);
    connect(dialog, SIGNAL(emitCreateKey(const QString&)),
            this, SLOT(createKey(const QString&)));
    dialog->exec();
}

void SigningWidget::createKey(const QString &param)
{
    kDebug() << "READY TO CREATE:" << param;
    GpgME::Data *data = new GpgME::Data();
//    GpgME::KeyGenerationResult result = m_gpgContext->generateKey(param.toAscii().data(), data);
//    if(result.primaryKeyGenerated() && result.subkeyGenerated()) {
//        kDebug() << "Generated new key with fingerprint: " << result.fingerprint();
//    }
//    kDebug() << "Generated new key with fingerprint: " << result.fingerprint();

    GpgME::KeyGenerationResult e  = m_gpgContext->generateKey(param.toUtf8().data(), *data);
    GpgME::Error er  = m_gpgContext->startKeyGeneration(param.toUtf8().data(), *data);
    kDebug() << "Error:" << e.error().source() << "::" << e.error().asString();
    kDebug() << "Error:" << er.source() << "::" << er.asString();
}

void SigningWidget::deleteKey()
{
    if (m_currentKey.isEmpty() || m_currentKey.isNull())
        return;

    GpgME::Error error = m_gpgContext->startKeyListing("",true);
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
            GpgME::KeyListResult lRes = m_gpgContext->endKeyListing();
            if (lRes.error()) {
                kDebug() << "Error while ending the keyListing operation: " << lRes.error().asString();
            }
            return;
        }
    }
    GpgME::KeyListResult lRes = m_gpgContext->endKeyListing();
    if (lRes.error()) {
        kDebug() << "Error while ending the keyListing operation: " << lRes.error().asString();
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
