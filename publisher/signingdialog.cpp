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


#include <KComboBox>
#include <KConfigGroup>
#include <KGlobal>
#include <KIcon>
#include <KIntNumInput>
#include <KLineEdit>

#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>


#include "signingdialog.h"

SigningDialog::SigningDialog(QWidget *parent) :
        QDialog(parent)
{
    setModal(true);

    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QFrame *separator = new QFrame(this);
    QFrame *separator2 = new QFrame(this);
    QHBoxLayout *buttonLay = new QHBoxLayout(this);
    //QHBoxLayout *expirationLay = new QHBoxLayout(this);
    KConfigGroup preferences = KGlobal::config()->group("NewProjectDefaultPreferences");

    m_nameLabel = new QLabel("Name:", this);
    m_emailLabel = new QLabel("Email:", this);
    m_commentLabel = new QLabel("Comment (optional):", this);
    //m_expirationLabel = new QLabel("Set Expiration Date:", this);
    m_pwdLabel = new QLabel("Password:", this);
    m_repeatPwdLabel = new QLabel("Confirm Password:", this);
    m_showPassword = new QCheckBox("Show password", this);
    m_createButton = new QPushButton("Create!", this);
    m_cancelButton = new QPushButton("Cancel", this);

    m_nameLine = new KLineEdit(preferences.readEntry("Username", QString("foo")), this);
    m_emailLine = new KLineEdit(preferences.readEntry("Email", QString("foo@foo.org")), this);
    m_commentLine = new KLineEdit(this);
    //m_expirationLineInput = new KIntNumInput(0, this);
    //m_expirationLineInput->setEnabled(false);
    //m_expirationComboBox = new KComboBox(this);
    m_pwdLine = new KLineEdit(this);
    m_repeatPwdLine = new KLineEdit(this);

    //m_expirationComboBox->addItem("Never");
    //m_expirationComboBox->addItem("Days");
    //m_expirationComboBox->addItem("Weeks");
    //m_expirationComboBox->addItem("Months");
    //m_expirationComboBox->addItem("Years");
    //expirationLay->addWidget(m_expirationLineInput);
    //expirationLay->addWidget(m_expirationComboBox);

    separator->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    separator2->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    m_pwdLine->setEchoMode(QLineEdit::Password);
    m_repeatPwdLine->setEchoMode(QLineEdit::Password);

    m_createButton->setIcon(KIcon("dialog-ok"));
    m_createButton->setEnabled(false);
    m_cancelButton->setIcon(KIcon("dialog-cancel"));
    m_cancelButton->setEnabled(true);
    buttonLay->addWidget(m_createButton);
    buttonLay->addWidget(m_cancelButton);


    mainLay->addWidget(m_nameLabel);
    mainLay->addWidget(m_nameLine);
    mainLay->addWidget(m_emailLabel);
    mainLay->addWidget(m_emailLine);
    mainLay->addWidget(m_commentLabel);
    mainLay->addWidget(m_commentLine);
    //mainLay->addWidget(m_expirationLabel);
    //mainLay->addLayout(expirationLay);
    mainLay->addWidget(separator);
    mainLay->addWidget(m_pwdLabel);
    mainLay->addWidget(m_pwdLine);
    mainLay->addWidget(m_repeatPwdLabel);
    mainLay->addWidget(m_repeatPwdLine);
    mainLay->addWidget(m_showPassword);
    mainLay->addWidget(separator2);
    mainLay->addLayout(buttonLay);

    connect(m_emailLine, SIGNAL(textChanged(const QString&)),
            this, SLOT(validateParams()));
    connect(m_nameLine, SIGNAL(textChanged(const QString&)),
            this, SLOT(validateParams()));
    //connect(m_expirationLineInput, SIGNAL(valueChanged(int)),
    //        this, SLOT(validateParams()));
    //connect(m_expirationComboBox, SIGNAL(currentIndexChanged(int)),
    //        this, SLOT(toggleExpirationLine()));
    connect(m_pwdLine, SIGNAL(textChanged(const QString&)),
            this, SLOT(validateParams()));
    connect(m_repeatPwdLine, SIGNAL(textChanged(const QString&)),
            this, SLOT(validateParams()));
    connect(m_showPassword, SIGNAL(toggled(bool)),
            this, SLOT(toggleShowPassword(bool)));


    connect(m_cancelButton, SIGNAL(clicked()),
            this, SLOT(close()));
    connect(m_createButton, SIGNAL(clicked()),
            this, SLOT(fetchParameters()));
}

void SigningDialog::validateParams()
{
    // Check for a valid email address.
    QRegExp rx("^[a-zA-Z0-9_.]*(@)[a-zA-Z0-9_.]*$");
    if (!rx.exactMatch(m_emailLine->text())) {
        m_createButton->setEnabled(false);
        return;
    }

    // Check for valid name, and expiration date, if any
    if (m_nameLine->text().isEmpty()) {
        m_createButton->setEnabled(false);
        return;
    }
    //if (m_expirationLineInput->isEnabled() && m_expirationLineInput->value() == 0) {
    //    m_createButton->setEnabled(false);
    //    return;
    //}

    // Check for a valid password
    if (m_pwdLine->text().isEmpty() && m_repeatPwdLine->text().isEmpty()) {
        m_createButton->setEnabled(false);
        return;
    }

    if (m_pwdLine->text() != m_repeatPwdLine->text()) {
        m_createButton->setEnabled(false);
        return;
    }

    // We are ready now :)
    m_createButton->setEnabled(true);
}

void SigningDialog::toggleShowPassword(const bool hide)
{
    m_pwdLine->setEchoMode(hide ? QLineEdit::Normal : QLineEdit::Password);
    m_repeatPwdLine->setEchoMode(hide ? QLineEdit::Normal : QLineEdit::Password);
}

//void SigningDialog::toggleExpirationLine()
//{
//    if (m_expirationComboBox->currentIndex() != 0) {
//        m_expirationLineInput->setEnabled(true);
//    } else {
//        m_expirationLineInput->setEnabled(false);
//    }
//    validateParams();
//}

void SigningDialog::fetchParameters()
{
    /* Structure of the param to send back to gpgme:
          <GnupgKeyParms format="internal">
          Key-Type: DSA
          Key-Length: 1024
          Subkey-Type: ELG-E
          Subkey-Length: 1024
          Name-Real: Joe Tester
          Name-Comment: with stupid passphrase
          Name-Email: joe@foo.bar
          Expire-Date: 0
          Passphrase: abc
          </GnupgKeyParms>
    */
    QString param;
    param.append("<GnupgKeyParms format=\"internal\">\n");
    param.append("Key-Type: DSA\n");
    param.append("Key-Length: 2048\n");
    param.append("Subkey-Type: ELG-E\n");
    param.append("Subkey-Length: 20148\n");
    param.append("Name-Real: " + m_nameLine->text() + "\n");
    param.append("Name-Comment: " + m_commentLine->text() + "\n");
    param.append("Name-Email: " + m_emailLine->text() + "\n");
    param.append("Expire-Date: 0");
    param.append("Passphrase: " + m_pwdLine->text() + "\n");
    param.append("</GnupgKeyParms>");

    close();
    emit(emitCreateKey(param));

}

#include "moc_signingdialog.cpp"
