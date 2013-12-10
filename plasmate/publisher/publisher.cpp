/*
  Copyright 2009 Lim Yuen Hoe <yuenhoe@hotmail.com>
  Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QDBusInterface>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QScopedPointer>

#include <KConfigGroup>
#include <KIO/DeleteJob>
#include <KSharedConfig>
#include <QUrlRequester>
#include <KLocalizedString>
#include <KService>
#include <KServiceTypeTrader>
#include <KMessageBox>
#include <knewstuff3/uploaddialog.h>
#include <KStandardDirs>

#include "publisher.h"
#include "signingwidget.h"
#include "../packagemodel.h"
#include "../projectmanager/projectmanager.h"
#include "remoteinstaller/remoteinstallerdialog.h"

Publisher::Publisher(QWidget *parent, const QUrl &path, const QString& type)
        : KDialog(parent),
        m_signingWidget(0),
        m_projectPath(path),
        m_projectType(type),
        m_comboBoxIndex(0)
{
    QWidget *uiWidget = new QWidget();
    m_ui.setupUi(uiWidget);
    m_signingWidget = new SigningWidget();

    //merge the ui file with the SigningWidget
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(uiWidget);
    layout->addWidget(m_signingWidget);

    QWidget *tmpWidget = new QWidget();
    QHBoxLayout *tmpLayout = new QHBoxLayout();
    tmpLayout->addLayout(layout);
    setButtons(KDialog::None);
    tmpWidget->setLayout(tmpLayout);
    setMainWidget(tmpWidget);

    m_extension = (type == "Plasma/Applet" || type == "Plasma/PopupApplet") ? "plasmoid" : "zip";

    m_ui.exporterUrl->setFilter(QString("*.") + m_extension);
    m_ui.exporterUrl->setMode(KFile::File | KFile::LocalOnly);

    //we want the installButton to be enabled only when the comboBox's current index is valid.
    m_ui.installButton->setEnabled(false);

    //populate the comboBox
    m_ui.installerButton->addItem("");
    m_ui.installerButton->addItem("Use PlasmaPkg");
    m_ui.installerButton->addItem("Remote Install");

    connect(m_ui.exporterUrl, SIGNAL(urlSelected(const QUrl&)), this, SLOT(addSuffix()));
    connect(m_ui.exporterButton, SIGNAL(clicked()), this, SLOT(doExport()));
    connect(m_ui.installerButton, SIGNAL(currentIndexChanged(int)), this, SLOT(checkInstallButtonState(int)));
    connect(m_ui.installButton, SIGNAL(clicked()), this, SLOT(doInstall()));
    connect(m_ui.publisherButton, SIGNAL(clicked()), this, SLOT(doPublish()));
    // Publish only works right for Plasmoid now afaik. Disabling for other project types.
    m_ui.publisherButton->setEnabled(type == "Plasma/Applet" || type == "Plasma/PopupApplet");

    setLayout(layout);
}

void Publisher::addSuffix()
{
    QString selected = m_ui.exporterUrl->url().path();
    QString suffix = QFileInfo(selected).suffix();
    if (suffix != m_extension && suffix != "zip") {
        m_ui.exporterUrl->setUrl(selected + "." + m_extension);
    }
}

void Publisher::setProjectName(const QString &name)
{
    m_projectName = name;
}

void Publisher::doExport()
{
    if (QFile(m_ui.exporterUrl->url().path()).exists()) {
        QString dialogText = i18n("A file already exists at %1. Do you want to overwrite it?",
                                  m_ui.exporterUrl->url().path());
        int code = KMessageBox::warningYesNo(0,dialogText);
        if (code != KMessageBox::Yes) return;
    }
    bool ok = exportToFile(m_ui.exporterUrl->url());

    // If signing is enabled, lets do that!
    if (m_signingWidget->signingEnabled()) {
        ok = ok && m_signingWidget->sign(m_ui.exporterUrl->url());
    }
    if (QFile::exists(m_ui.exporterUrl->url().path()) && ok) {
        KMessageBox::information(this, i18n("Project has been exported to %1.", m_ui.exporterUrl->url().path()));
    } else {
        KMessageBox::error(this, i18n("An error has occurred during the export. Please check the write permissions "
        "in the target directory."));
    }
}

//we want the installButton to be enabled only when comboBox's index is valid.
void Publisher::checkInstallButtonState(int comboBoxCurrentIndex)
{
    if (comboBoxCurrentIndex == 1) {
        m_ui.installButton->setEnabled(true);
        m_comboBoxIndex = 1;
    } else if (comboBoxCurrentIndex == 2) {
        m_ui.installButton->setEnabled(true);
        m_comboBoxIndex = 2;
    } else if (comboBoxCurrentIndex == 3) {
        m_ui.installButton->setEnabled(true);
        m_comboBoxIndex = 3;
    } else {
        m_ui.installButton->setEnabled(false);
        m_comboBoxIndex = 0;
    }
}

//choose the method which which we will install the project
//according to comboBox's index
void Publisher::doInstall()
{
    if (m_comboBoxIndex == 1) {
        doPlasmaPkg();
    } else if (m_comboBoxIndex == 2) {
        doRemoteInstall();
    }
}

void Publisher::doPlasmaPkg()
{
    const QUrl tempPackage(tempPackagePath());
    qDebug() << "tempPackagePath" << tempPackage.pathOrUrl();
    qDebug() << "m_projectPath" << m_projectPath.pathOrUrl();
    ProjectManager::exportPackage(m_projectPath, tempPackage); // create temporary package

    QStringList argv("plasmapkg");
    argv.append("-t");
    if (m_projectType == "Plasma/Runner") {
        argv.append("runner");
    } else if (m_projectType == "Plasma/DataEngine") {
        argv.append("dataengine");
    } else if (m_projectType == "Plasma/Theme") {
        argv.append("theme");
    } else if (m_projectType == "Plasma/Applet") {
        argv.append("plasmoid");
    } else if (m_projectType == "KWin/WindowSwitcher") {
        argv.append("windowswitcher");
    } else if (m_projectType == "KWin/Script") {
        argv.append("kwinscript");
    } else if (m_projectType == "KWin/Effect") {
        argv.append("kwineffect");
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

void Publisher::doRemoteInstall()
{
    QScopedPointer<RemoteInstallerDialog> dialog(new RemoteInstallerDialog());

    //get the source directory from plasmaterc
    QString path = currentPackagePath();
    dialog->setPackagePath(path);

    dialog->exec();
}

const QString Publisher::tempPackagePath()
{
    QDir d(m_projectPath.pathOrUrl());
    if (d.cdUp()) {
        return d.path() + "/" + m_projectName + "." + m_extension;
    }
    return m_projectPath.path(QUrl::AddTrailingSlash) + m_projectName + "." + m_extension;
}

void Publisher::doPublish()
{
    // TODO: make sure this works with non-plasmoids too?
    qDebug() << "projectPath:" << m_projectPath.path();

    qDebug() << "Exportando no tmp: file://" + tempPackagePath();
    QUrl url(tempPackagePath());

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

bool Publisher::exportToFile(const QUrl& url)
{
    if (!url.isLocalFile() ||
            QDir(url.path()).exists()) {
        KMessageBox::error(this, i18n("The file you entered is invalid."));
        return false;
    }
    return ProjectManager::exportPackage(m_projectPath, url); // will overwrite if exists!
}

QString Publisher::currentPackagePath() const
{
    KConfigGroup cg(KGlobal::config(), "PackageModel::package");
    return cg.readEntry("lastLoadedPackage", QString());
}
