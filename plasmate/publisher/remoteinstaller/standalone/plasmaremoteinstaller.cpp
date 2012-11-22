/*
 *  Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "plasmaremoteinstaller.h"

PlasmaRemoteInstaller::PlasmaRemoteInstaller(QWidget* parent)
        : RemoteInstallerDialog(parent)
{
    connect(m_ui.sourceDirectory, SIGNAL(textChanged(QString)), this, SLOT(checkProjectPath(QString)));


    //disable the buttons. The user hasn't give a path yet.
    m_ui.usernameLineEdit->setEnabled(false);
    m_ui.hostnameLineEdit->setEnabled(false);

    //we want the source relative ui to be visible.
    m_ui.installPackageLabel->setVisible(true);
    m_ui.sourceLabel->setVisible(true);
    m_ui.sourceDirectory->setVisible(true);
    m_ui.srcSeparator->setVisible(true);
}

void PlasmaRemoteInstaller::checkProjectPath(const QString& path)
{
    if (path.isEmpty()) {
        return;
    }

    //check if projectName/metadata.desktop exists
    if(path.endsWith("metadata.desktop")) {
        setPackagePath(path);
        m_ui.usernameLineEdit->setEnabled(true);
        m_ui.hostnameLineEdit->setEnabled(true);
    } else {
        m_ui.usernameLineEdit->setEnabled(false);
        m_ui.hostnameLineEdit->setEnabled(false);
    }
}

#include "moc_plasmaremoteinstaller.cpp"