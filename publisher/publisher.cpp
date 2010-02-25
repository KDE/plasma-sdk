/*
  Copyright (c) 2009 Lim Yuen Hoe <yuenhoe@hotmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

#include <KUrlRequester>
#include <KLocalizedString>
#include <KProcess>
#include <KMessageBox>
#include <knewstuff3/uploaddialog.h>
#include <KStandardDirs>

#include "publisher.h"
#include "../packagemodel.h"
#include "../projectmanager/projectmanager.h"

Publisher::Publisher(QWidget *parent, const KUrl &path, const QString& type)
    :QWidget(parent),
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
    
    m_extension = type == PackageModel::plasmoidType ? "plasmoid" : "zip";

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

    QWidget *spaceSoaker = new QWidget(this);
    layout->addWidget(spaceSoaker);
    layout->setStretchFactor(spaceSoaker, 100);

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

void Publisher::doExport()
{
    bool ok = exportToFile(m_exporterUrl->url());
    if (QFile::exists(m_exporterUrl->url().path()) && ok)
        KMessageBox::information(this, i18n("Project has been exported to %1.", m_exporterUrl->url().path()));
    else
        KMessageBox::error(this, i18n("An error has occurred during the export. Please check the write permissions in the target directory."));
}

// Plasmoid specific, for now
void Publisher::doInstall()
{
    KUrl tempPackage(m_projectPath.path(KUrl::AddTrailingSlash) + "package." + m_extension);
    ProjectManager::exportPackage(m_projectPath, tempPackage); // create temporary package

    QStringList argv("plasmapkg");
    argv.append("-t");
    if (m_projectType == PackageModel::runnerType) {
        argv.append("runner");
    } else if (m_projectType == PackageModel::dataengineType) {
        argv.append("dataengine");
    } else if (m_projectType == PackageModel::themeType) {
        argv.append("theme");
    } else {
        argv.append("plasmoid");
    }
    // we do a plasmapkg -u in case the package was installed before
    // in which case it should be updated. -u also installs the
    // package if it hasn't been installed before, so it's all good.
    argv.append("-u");
    argv.append(tempPackage.path());
    KProcess::execute(argv);
    QFile::remove(tempPackage.path()); // delete temporary package
    // TODO: probably check for errors and stuff instead of announcing
    // succcess in a 'leap of faith'
    KMessageBox::information(this, i18n("Project has been installed"));
}

void Publisher::doPublish()
{
    //FIXME: should not use appdata/tmp/ because a user project may be called 'tmp'
    //       creating a name conflict. Probably just /tmp?
    kDebug() << "projectPath:" << m_projectPath.path();
    kDebug() << "Exportando no tmp: file://" + KStandardDirs::locateLocal("appdata", "tmp/" + QFileInfo(m_projectPath.path()).baseName() + "package." + m_extension);
    KUrl url("file://" + KStandardDirs::locateLocal("appdata", "tmp/" + QFileInfo(m_projectPath.path()).baseName() + "package." + m_extension));
    // export to tmp folder
    bool ok = exportToFile(url);
    if (ok) {
        KNS3::UploadDialog *mNewStuffDialog = new KNS3::UploadDialog("plasmate.knsrc", this);
        mNewStuffDialog->setUploadFile(url);
        mNewStuffDialog->exec();
        //TODO: Delete temporary package after publishing is done
    } else {
        // should probably eventually use a better error message
        KMessageBox::error(this, i18n("An error has occurred during the export. Please check the write permissions in the target directory."));
    }
}

bool Publisher::exportToFile(const KUrl& url)
{
    //TODO: Handle existing file/overwrite etc.
    if (!url.isLocalFile() ||
          QDir(url.path()).exists()) {
        KMessageBox::error(this, i18n("The file you entered is invalid."));
        return false;
    }
    return ProjectManager::exportPackage(m_projectPath, url);
}
