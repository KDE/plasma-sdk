/*
 *  Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "remoteinstaller.h"

#include <KIO/NetAccess>
#include <KIO/CopyJob>
#include <KMessageBox>
#include <KLocalizedString>

#include <QWidget>
#include <QDir>

RemoteInstaller::RemoteInstaller(const QString& username, const QString& hostname,
                                 const QString& source, QWidget* parent)
        : QObject(parent),
        m_widget(parent)
{
    //like fish://username@192.123.23.1
    m_execUrl.setUrl("fish://" + hostname + ":22");
    m_execUrl.setUserName(username);

    //this will be out temp directory
    const QString temporaryDirectory("/var/tmp/plasmaremoteinstaller/");
    //we need a temporary directory
    //all users have access into /var/tmp
    KUrl tmpUrl = m_execUrl;
    tmpUrl.addPath(temporaryDirectory);
    if (!KIO::NetAccess::exists(tmpUrl, KIO::NetAccess::DestinationSide, m_widget)) {
        const bool ok = KIO::NetAccess::mkdir(tmpUrl, m_widget);
        if (!ok) {
            QString text = i18n("Plasma Remote Installer wasn't able to create a temp dir at"
                "%1, please check the rights into %1").arg(temporaryDirectory);
            KMessageBox::error(m_widget, text);
            return;
        }
    }

    //our destination path
    m_destinationPath = m_execUrl;
    m_destinationPath.setPath(temporaryDirectory);

    //we need our source path
    m_sourcePath = source;

    m_plasmaPkgUrl.append("plasmapkg -u " + temporaryDirectory + packageDir.dirName() + '/');

    //now do the installation
    doInstall();
}

void RemoteInstaller::doInstall()
{
    //copy the project in the target's plasmate data directory
    KIO::CopyJob *copy = KIO::copy(m_sourcePath, m_destinationPath);

    connect(copy, SIGNAL(result(KJob*)), this, SLOT(doPlasmaPkg(KJob*)));
}

void RemoteInstaller::doPlasmaPkg(KJob* job)
{
    if (job->error() != 0) {
        //an error has occured from the previous job.
        //show the error to the user.
        KMessageBox::error(m_widget, job->errorText());
    }

    KIO::NetAccess::fish_execute(m_execUrl, m_plasmaPkgUrl, m_widget);

    //an error has occured.
    if (KIO::NetAccess::lastError()) {
        //show the error
        KMessageBox::error(m_widget, KIO::NetAccess::lastErrorString());
    }
}


