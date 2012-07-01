/*
 *  Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "remoteinstallerdialog.h"
#include "remoteinstaller.h"

#include <QFile>
#include <QPointer>
#include <QVBoxLayout>
#include <QLabel>

#include <KMessageBox>

RemoteInstallerDialog::RemoteInstallerDialog(QWidget* parent)
        : QDialog(parent),
        m_installer(0)
{
    QWidget *widget = new QWidget();

    m_ui.setupUi(widget);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(widget);
    setLayout(layout);

    //we don't want the ok button to be enabled
    //before we get all the necessary informations
    m_ui.installButton->setEnabled(false);

    connect(m_ui.usernameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkInformations()));
    connect(m_ui.hostnameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkInformations()));
    connect(m_ui.installButton, SIGNAL(clicked()), this, SLOT(install()));

    //we don't want the source relative ui to be visible.
    //We will get this information from plasmate.
    m_ui.srcLabel1->setVisible(false);
    m_ui.srcLabel2->setVisible(false);
    m_ui.srcDirUrl->setVisible(false);
    m_ui.srcSeparator->setVisible(false);
}

void RemoteInstallerDialog::checkInformations()
{
    QString username = m_ui.usernameLineEdit->text();

    //we don't want spaces
    username = username.replace(' ', "");

    QString hostname = m_ui.hostnameLineEdit->text();

    //we don't want spaces
    hostname = hostname.replace(' ', "");

    QFile metadateDesktopFile(m_packagePath);
    if (!metadateDesktopFile.exists()) {
        return;
    }

    if (!username.isEmpty() && !hostname.isEmpty()) {
        m_username = username;
        m_hostname = hostname;

        //now enable the install button
        m_ui.installButton->setEnabled(true);
    }
}


void RemoteInstallerDialog::install()
{
    m_installer = new RemoteInstaller(m_username, m_hostname, m_packagePath, this);
}

QString RemoteInstallerDialog::packagePath() const
{
    return m_packagePath;
}

void RemoteInstallerDialog::setPackagePath(const QString& path)
{
    m_packagePath = path;
}
