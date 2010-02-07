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
#include <KZip>

#include "publisher.h"

Publisher::Publisher(QWidget *parent, const KUrl &path)
    :QWidget(parent),
    m_projectPath(path)
{
    // something simple and plasmoid-specific for now
    // These should probably be refined at some point
    QString exportLabel = i18n("Export plasmoid");
    QString exportText = i18n("Choose a target file to export the current plasmoid to an installable package file on your system.");
    QString installLabel = i18n("Install plasmoid");
    QString installText = i18n("Click to install the current plasmoid directly onto your computer.");
    QString publishLabel = i18n("Publish plasmoid");
    QString publishText = i18n("Click to publish the current plasmoid online, so that other people can find and install it using the Internet.");

    m_exporterUrl = new KUrlRequester(this);
    m_exporterUrl->setFilter("*.plasmoid");
    m_exporterUrl->setMode(KFile::File | KFile::LocalOnly);

    m_exporterButton = new QPushButton(i18n("Export current plasmoid"), this);
    m_installerButton = new QPushButton(i18n("Install current plasmoid"), this);
    m_publisherButton = new QPushButton(i18n("Publish current plasmoid"), this);

    // Disable publish button first since it isn't implemented.
    m_publisherButton->setEnabled(false);

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
    if (suffix != "plasmoid" && suffix != "zip") {
        m_exporterUrl->setUrl(selected + ".plasmoid");
    }
}

void Publisher::doExport()
{
    if (!m_exporterUrl->url().isLocalFile() ||
          QDir(m_exporterUrl->url().path()).exists()) {
        KMessageBox::error(this, i18n("The file you entered is invalid."));
        return;
    }
    bool ok = exportPackage(m_projectPath, m_exporterUrl->url());
    if (QFile::exists(m_exporterUrl->url().path()) && ok)
        KMessageBox::information(this, i18n("Plasmoid has been exported to %1.", m_exporterUrl->url().path()));
    else
        KMessageBox::error(this, i18n("An error has occurred during the export. Please check the write permissions in the target directory."));
}

void Publisher::doInstall()
{
    KUrl tempPackage(m_projectPath.path(KUrl::AddTrailingSlash) + "package.plasmoid");
    exportPackage(m_projectPath, tempPackage); // create temporary package

    // we do a plasmapkg -u in case the package was installed before
    // in which case it should be updated. -u also installs the
    // package if it hasn't been installed before, so it's all good.
    QStringList argv("plasmapkg");
    argv.append("-u");
    argv.append(tempPackage.path());
    KProcess::execute(argv);
    QFile::remove(tempPackage.path()); // delete temporary package
    // TODO: probably check for errors and stuff instead of announcing
    // succcess in a 'leap of faith'
    KMessageBox::information(this, i18n("Plasmoid has been installed"));
}

void Publisher::doPublish()
{
    KMessageBox::information(this, "Do funky stuff here");
}

bool Publisher::exportPackage(const KUrl &toExport, const KUrl &targetFile)
{
    bool ret = true;
    KZip plasmoid(targetFile.path());
    if (!plasmoid.open(QIODevice::WriteOnly)) {
        return false;
    }
    ret = plasmoid.addLocalDirectory(toExport.path(), ".");
    plasmoid.close();
    return ret;
}

bool Publisher::importPackage(const KUrl &toImport, const KUrl &targetLocation)
{
    bool ret = true;
    KZip plasmoid(toImport.path());
    if (!plasmoid.open(QIODevice::ReadOnly)) {
        return false;
    }
    plasmoid.directory()->copyTo(targetLocation.path());
    plasmoid.close();
    return ret;
}
