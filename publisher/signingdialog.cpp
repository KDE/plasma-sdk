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
    m_nameLabel = new QLabel("Name:", this);
    m_emailLabel = new QLabel("Email:", this);
    m_commentLabel = new QLabel("Comment (optional):", this);
    m_expirationLabel = new QLabel("Set Expiration Date:", this);
    m_pwdLabel = new QLabel("Password:", this);
    m_repeatPwdLabel = new QLabel("Confirm Password:", this);

    QFrame *separator = new QFrame();
    separator->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    KConfigGroup preferences = KGlobal::config()->group("NewProjectDefaultPreferences");

    m_nameLine = new KLineEdit(preferences.readEntry("Username", QString("foo")), this);
    m_emailLine = new KLineEdit(preferences.readEntry("Email", QString("foo@foo.org")), this);
    m_commentLine = new KLineEdit(this);
    m_expirationLineInput = new KIntNumInput(0, this);
    m_expirationLineInput->setEnabled(false);
    m_expirationComboBox = new KComboBox(this);
    m_expirationComboBox->addItem("Never");
    m_expirationComboBox->addItem("Days");
    m_expirationComboBox->addItem("Weeks");
    m_expirationComboBox->addItem("Months");
    m_expirationComboBox->addItem("Years");

    m_pwdLine = new KLineEdit(this);
    m_pwdLine->setEchoMode(QLineEdit::Password);
    m_repeatPwdLine = new KLineEdit(this);
    m_repeatPwdLine->setEchoMode(QLineEdit::Password);


    QHBoxLayout *buttonLay = new QHBoxLayout(this);
    m_createButton = new QPushButton("Create!", this);
    m_cancelButton = new QPushButton("Cancel", this);
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
    mainLay->addWidget(m_expirationLabel);
    mainLay->addWidget(m_expirationLineInput);
    mainLay->addWidget(m_expirationComboBox);
    mainLay->addWidget(separator);
    mainLay->addWidget(m_pwdLabel);
    mainLay->addWidget(m_pwdLine);
    mainLay->addWidget(m_repeatPwdLabel);
    mainLay->addWidget(m_repeatPwdLine);
    mainLay->addLayout(buttonLay);

    connect(m_emailLine, SIGNAL(textChanged(const QString&)),
            this, SLOT(validateParams()));
    connect(m_nameLine, SIGNAL(textChanged(const QString&)),
            this, SLOT(validateParams()));
    connect(m_expirationLineInput, SIGNAL(valueChanged(int)),
            this, SLOT(validateParams()));
    connect(m_pwdLine, SIGNAL(textChanged(const QString&)),
            this, SLOT(validateParams()));
    connect(m_repeatPwdLine, SIGNAL(textChanged(const QString&)),
            this, SLOT(validateParams()));


    connect(m_cancelButton, SIGNAL(clicked()),
            this, SLOT(close()));
    connect(m_createButton, SIGNAL(clicked()),
            this, SIGNAL(createKey(const QString&)));
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

#include "moc_signingdialog.cpp"
