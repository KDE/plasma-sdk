/*
  Copyright 2009 Lim Yuen Hoe <yuenhoe@hotmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QDBusInterface>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QCheckBox>

#include <KUrlRequester>
#include <KLocalizedString>
#include <KProcess>
#include <KService>
#include <KServiceTypeTrader>
#include <KMessageBox>
#include <knewstuff3/uploaddialog.h>
#include <KStandardDirs>

#include "publisher.h"
#include "signingwidget.h"
#include "../packagemodel.h"
#include "../projectmanager/projectmanager.h"

Publisher::Publisher(QWidget *parent, const KUrl &path, const QString& type)
        : QDialog(parent),
        m_projectPath(path),
        m_projectType(type)
{
    // These should probably be refined at some point
    QString exportLabel = i18n("Export project");
    QString exportText = i18n("Choose a target file to export the current project to an installable package file on your system.");
    QString installLabel = i18n("Install project");
    QString installText = i18n("Click to install the current project directly onto your computer.");
    QString publishLabel = i18n("Publish project");
    QString publishText = i18n("Click to publish the current project online, so that other people can find and install it using the Internet.");

    m_extension = (type == "Plasma/Applet" || type == "Plasma/PopupApplet") ? "plasmoid" : "zip";

    m_exporterUrl = new KUrlRequester(this);
    m_exporterUrl->setFilter(QString("*.") + m_extension);
    m_exporterUrl->setMode(KFile::File | KFile::LocalOnly);

    m_exporterButton = new QPushButton(i18n("Export current project"), this);
    m_installerButton = new QPushButton(i18n("Install current project"), this);
    m_publisherButton = new QPushButton(i18n("Publish current project"), this);

    connect(m_exporterUrl, SIGNAL(urlSelected(const KUrl&)), this, SLOT(addSuffix()));
    connect(m_exporterButton, SIGNAL(clicked()), this, SLOT(doExport()));
    connect(m_installerButton, SIGNAL(clicked()), this, SLOT(doInstall()));
    connect(m_publisherButton, SIGNAL(clicked()), this, SLOT(doPublish()));

    // Publish only works right for Plasmoid now afaik. Disabling for other project types.
    m_publisherButton->setEnabled(type == "Plasma/Applet" || type == "Plasma/PopupApplet");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(new QLabel("<h1>" + exportLabel + "</h1>", this));

    QFrame *separator = new QFrame();
    separator->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    layout->addWidget(separator);
    layout->addWidget(new QLabel(exportText, this));
    layout->addWidget(m_exporterUrl);
    layout->addWidget(m_exporterButton);

    layout->addWidget(new QLabel("<h1>" + installLabel + "</h1>", this));

    separator = new QFrame();
    separator->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    layout->addWidget(separator);
    layout->addWidget(new QLabel(installText, this));
    layout->addWidget(m_installerButton);

    layout->addWidget(new QLabel("<h1>" + publishLabel + "</h1>", this));

    separator = new QFrame();
    separator->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    layout->addWidget(separator);
    layout->addWidget(new QLabel(publishText, this));
    layout->addWidget(m_publisherButton);

    m_signingWidget = new SigningWidget();

    layout->addWidget(m_signingWidget);

    layout->addStretch();
    setLayout(layout);

}

void Publisher::addSuffix()
{
    QString selected = m_exporterUrl->url().path();
    QString suffix = QFileInfo(selected).suffix();
    if (suffix != m_extension && suffix != "zip") {
        m_exporterUrl->setUrl(selected + "." + m_extension);
    }
}

void Publisher::setProjectName(const QString &name)
{
    m_projectName = name;
}

void Publisher::doExport()
{
    if (QFile(m_exporterUrl->url().path()).exists()) {
        QString dialogText = i18n("A file already exists at %1. Do you want to overwrite it?", m_exporterUrl->url().path());
        int code = KMessageBox::warningYesNo(0,dialogText);
        if (code != KMessageBox::Yes) return;
    }
    bool ok = exportToFile(m_exporterUrl->url());

    // If signign is enabled, lets do that!
    if (m_signingWidget->signingEnabled()) {
        ok = ok && m_signingWidget->sign(m_exporterUrl->url());
    }
    if (QFile::exists(m_exporterUrl->url().path()) && ok)
        KMessageBox::information(this, i18n("Project has been exported to %1.", m_exporterUrl->url().path()));
    else
        KMessageBox::error(this, i18n("An error has occurred during the export. Please check the write permissions in the target directory."));
}

// Plasmoid specific, for now
void Publisher::doInstall()
{
    KUrl tempPackage(tempPackagePath());
    kDebug() << "tempPackagePath" << tempPackage.pathOrUrl();
    kDebug() << "m_projectPath" << m_projectPath.pathOrUrl();
    ProjectManager::exportPackage(m_projectPath, tempPackage); // create temporary package

    QStringList argv("plasmapkg");
    argv.append("-t");
    if (m_projectType == "Plasma/Runner") {
        argv.append("runner");
    } else if (m_projectType == "Plasma/DataEngine") {
        argv.append("dataengine");
    } else if (m_projectType == "Plasma/Theme") {
        argv.append("theme");
    } else {
        argv.append("plasmoid");
    }
    // we do a plasmapkg -u in case the package was installed before
    // in which case it should be updated. -u also installs the
    // package if it hasn't been installed before, so it's all good.
    argv.append("-u");
    argv.append(tempPackage.path());
    bool ok = (KProcess::execute(argv) >= 0 ? true: false);
    if(ok) {
        QDBusInterface dbi("org.kde.kded", "/kbuildsycoca", "org.kde.kbuildsycoca");
        dbi.call(QDBus::NoBlock, "recreate");
    } else {
        KMessageBox::error(this, i18n("Project has not been installed"));
        return;
    }


    if (m_signingWidget->signingEnabled()) {
        ok = ok && m_signingWidget->sign(tempPackage);

        QString signatureDestPath = KStandardDirs::locateLocal("data", "plasma/plasmoids/");
        signatureDestPath.append(m_projectName).append(".plasmoid.asc");

        QString signatureOrigPath(tempPackage.pathOrUrl().append(".asc"));

        QFile signatureDest(signatureDestPath);
        if(signatureDest.open(QIODevice::ReadWrite)) {
            signatureDest.remove(signatureDestPath);
            signatureDest.close();
        }

        QFile signatureOrig(signatureOrigPath);
        if(signatureOrig.open(QIODevice::ReadWrite)) {
            ok = signatureOrig.copy(signatureOrigPath,signatureDestPath);
        } else {
            ok = false;
        }

    }
    QFile::remove(tempPackage.path()); // delete temporary package
    // TODO: probably check for errors and stuff instead of announcing
    // succcess in a 'leap of faith'
    if(ok) {
        KMessageBox::information(this, i18n("Project has been installed"));
    } else {
        KMessageBox::error(this, i18n("Project has not been installed"));
    }
}

const QString Publisher::tempPackagePath()
{
    QDir d(m_projectPath.pathOrUrl());
    if (d.cdUp()) {
        return d.path() + "/" + m_projectName + "." + m_extension;
    }
    return m_projectPath.path(KUrl::AddTrailingSlash) + m_projectName + "." + m_extension;
}

void Publisher::doPublish()
{
    // TODO: make sure this works with non-plasmoids too?
    kDebug() << "projectPath:" << m_projectPath.path();

    kDebug() << "Exportando no tmp: file://" + tempPackagePath();
    KUrl url(tempPackagePath());

    bool ok = exportToFile(url);
    if (m_signingWidget->signingEnabled()) {
        ok = ok && m_signingWidget->sign(url);
    }
    if (ok) {
        KNS3::UploadDialog *mNewStuffDialog = new KNS3::UploadDialog("plasmate.knsrc", this);
        mNewStuffDialog->setUploadFile(url);
        mNewStuffDialog->exec();
        QFile::remove(url.path()); // delete temporary package
    } else {
        // should probably eventually use a better error message
        KMessageBox::error(this, i18n("An error has occurred during the export. Please check the write permissions in the target directory."));
    }
}

bool Publisher::exportToFile(const KUrl& url)
{
    if (!url.isLocalFile() ||
            QDir(url.path()).exists()) {
        KMessageBox::error(this, i18n("The file you entered is invalid."));
        return false;
    }
    return ProjectManager::exportPackage(m_projectPath, url); // will overwrite if exists!
}
