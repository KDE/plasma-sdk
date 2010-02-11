/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QLabel>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QValidator>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

#include <KUser>
// #include <KLocalizedString>
#include <KDebug>
#include <KDesktopFile>
#include <KLineEdit>
#include <KPushButton>
#include <KSeparator>
#include <KUrlRequester>
#include <KStandardDirs>
#include <KUser>
#include <KMessageBox>
#include <Plasma/PackageMetadata>

#include "packagemodel.h"
#include "startpage.h"
#include "mainwindow.h"
#include "ui_startpage.h"
#include "publisher/publisher.h"
#include "projectmanager/projectmanager.h"

StartPage::StartPage(MainWindow *parent) // TODO set a palette so it will look identical with any color scheme.
        : QWidget(parent),
        m_parent(parent)
{
    setupWidgets();
    refreshRecentProjectsList();
}

StartPage::~StartPage()
{
    delete ui;
}

void StartPage::setupWidgets()
{
    projectManager = new ProjectManager(this);
    ui = new Ui::StartPage;
    ui->setupUi(this);

    // Set some default parameters, like username/email and preferred scripting language
    KConfigGroup cg = KGlobal::config()->group("NewProjectDefaultPreferences");
    KUser user = KUser(KUser::UseRealUserID);

    QString userName = cg.readEntry("Username", user.loginName());
    QString userEmail = cg.readEntry("Email", userName+"@none.org");

    // If username or email are empty string, i.e. in the previous project the
    // developer deleted it, restore the default values
    if (userName.isEmpty()) {
        userName = user.loginName();
    }
    if (userEmail.isEmpty()) {
        userEmail.append(user.loginName()+"@none.org");
    }

    ui->authorTextField->setText(userName);
    ui->emailTextField->setText(userEmail);

    ui->radioButtonJs->setChecked(cg.readEntry("radioButtonJsChecked", true));
    ui->radioButtonPy->setChecked(cg.readEntry("radioButtonPyChecked", false));
    ui->radioButtonRb->setChecked(cg.readEntry("radioButtonRbChecked", false));

    // Enforce the security restriction from package.cpp in the input field
    connect(ui->projectName, SIGNAL(textEdited(const QString&)),
            this, SLOT(processProjectName(const QString&)));

    connect(ui->projectName, SIGNAL(returnPressed()),
            this, SLOT(createNewProject()));
    connect(ui->recentProjects, SIGNAL(clicked(const QModelIndex)),
            this, SLOT(emitProjectSelected(const QModelIndex)));

    // When there will be a good API for js and rb dataengines and runners, remove the
    // first connect() statement and uncomment the one below :)
    connect(ui->contentTypes, SIGNAL(clicked(const QModelIndex)),
            this, SLOT(validateProjectType(const QModelIndex)));
    /*connect(ui->contentTypes, SIGNAL(clicked(const QModelIndex)),
            ui->projectName, SLOT(setFocus()));*/


    connect(ui->newProjectButton, SIGNAL(clicked()),
            this, SLOT(createNewProject()));
    connect(ui->cancelNewProjectButton, SIGNAL(clicked()),
            this, SLOT(cancelNewProject()));
    connect(ui->importButton, SIGNAL(clicked()),
            this, SLOT(doImport()));
    connect(ui->moreButton, SIGNAL(clicked()),
            this, SLOT(showMoreDialog()));

    connect(projectManager, SIGNAL(projectSelected(QString, QString)),
            this, SLOT(emitProjectSelected(QString, QString)));

    new QListWidgetItem(KIcon("application-x-plasma"), i18n("Plasmoid"), ui->contentTypes);
    new QListWidgetItem(KIcon("kexi"), i18n("Data Engine"), ui->contentTypes);
    new QListWidgetItem(KIcon("system-run"), i18n("Runner"), ui->contentTypes);
    new QListWidgetItem(KIcon("inkscape"), i18n("Theme"), ui->contentTypes);

//     connect(ui->newProjectButton, SIGNAL(clicked()), this, SLOT(launchNewProjectWizard()));
}

void StartPage::processProjectName(const QString& name)
{
    QRegExp validatePluginName("[a-zA-Z0-9_.]*");
    if (!validatePluginName.exactMatch(name)) {
        int pos = 0;
        for (int i = 0; i < name.size(); i++) {
            if (validatePluginName.indexIn(name, pos, QRegExp::CaretAtZero) == -1)
                break;
            pos += validatePluginName.matchedLength();
        }
        ui->projectName->setText(QString(name).remove(pos, 1));
    }
}

void StartPage::validateProjectType(const QModelIndex &sender)
{

    if ((sender.row() == 1) ||
        (sender.row() == 2)) {
        ui->radioButtonJs->setEnabled(false);
        // gotta explicitly setEnabled to true in case it
        // was falsed before!
        ui->radioButtonPy->setEnabled(true);
        // also check this radio to prevent any disabled radios from
        // being checked due to previous setting!
        ui->radioButtonPy->setChecked(true);
        ui->radioButtonRb->setEnabled(false);
        ui->newProjectButton->setEnabled(true); // in case previously falsed

    } else if (sender.row() == 3) {
        ui->radioButtonJs->setEnabled(false);
        ui->radioButtonPy->setEnabled(false);
        ui->radioButtonRb->setEnabled(false);
        ui->newProjectButton->setEnabled(false); // disable the create button too.

    } else /* if (sender.row() == 0) */ {
        ui->radioButtonJs->setEnabled(true);
        ui->radioButtonPy->setEnabled(true);
        ui->radioButtonRb->setEnabled(true);
        ui->newProjectButton->setEnabled(true); // in case previously falsed
    }

    ui->layoutHackStackedWidget->setCurrentIndex(1);
    ui->projectName->setFocus();
}

QString StartPage::userName()
{
    return ui->authorTextField->text();
}

QString StartPage::userEmail()
{
    return ui->emailTextField->text();
}

bool StartPage::selectedJsRadioButton()
{
    return ui->radioButtonJs->isChecked();
}

bool StartPage::selectedRbRadioButton()
{
    return ui->radioButtonRb->isChecked();
}

bool StartPage::selectedPyRadioButton()
{
    return ui->radioButtonPy->isChecked();
}

void StartPage::resetStatus()
{
    kDebug() << "Reset status!";
    ui->layoutHackStackedWidget->setCurrentIndex(0);
    refreshRecentProjectsList();
}

void StartPage::refreshRecentProjectsList()
{
    ui->recentProjects->clear();
    projectManager->clearProjects();
    QStringList recentFiles = m_parent->recentProjects();

    for (int i = 0; i < recentFiles.size(); i++) {
        // Specify path + filename as well to avoid mistaking .gitignore
        // as being the metadata file.
        Plasma::PackageMetadata metadata(KStandardDirs::locateLocal("appdata", recentFiles.at(i) + "/metadata.desktop"));

        // Changed this back on second thought. It's better that we
        // do not expose the idea of a 'folder name' to the user -
        // we keep the folder hidden and name it whatever we want as
        // long as it's unique. Only the plasmoid name, which the user
        // sets in the metadata editor, will ever be visible in the UI.
        QString projectName = metadata.name();
        // QString projectName = recentFiles.at(i);

//         if (projectName.isEmpty()) {
//             continue;
//         }

        kDebug() << "adding" << projectName << "to the list of recent projects...";
        QListWidgetItem *item = new QListWidgetItem(projectName); // show the user-set plasmoid name in the UI

        // the loading code uses this to find the project to load.
        // since folder name and plasmoid name can be different, this
        // should be set to the folder name, which the loading code expects.
        item->setData(FullPathRole, recentFiles.at(i));

        QString serviceType = metadata.serviceType();

        if (serviceType == QString("Plasma/Applet")) {
            item->setIcon(KIcon("application-x-plasma"));
        } else if (serviceType == QString("Plasma/DataEngine")) {
            item->setIcon(KIcon("kexi"));
        } else if (serviceType == QString("Plasma/Theme")) {
            item->setIcon(KIcon("inkscape"));
        } else if (serviceType == QString("Plasma/Runner")) {
            item->setIcon(KIcon("system-run"));
        } else {
            kWarning() << "Unknown service type" << serviceType;
        }

        projectManager->addProject(item);
        // limit to 5 projects to display up front
        if (i < 5) {
            ui->recentProjects->addItem(new QListWidgetItem(*item));
        }
    }
}

void StartPage::createNewProject()
{
// TODO
//     metadata->setPluginName( view->pluginname_edit->text() );
    kDebug() << "Creating simple folder structure for the project ";

    // packagePath -> projectPath
    QString projectNameLowerCase = ui->projectName->text().toLower();
    QString projectName = ui->projectName->text();
    QString projectFileExtension;

    QString templateFilePath = KStandardDirs::locate("appdata", "templates/");
    QString projectFolderName = generateProjectFolderName(projectNameLowerCase);
    QString projectPath = KStandardDirs::locateLocal("appdata", projectFolderName + '/');

    Plasma::PackageMetadata metadata;

    // type -> serviceTypes
    QString serviceTypes;
    if (ui->contentTypes->currentRow() == 1) {
        serviceTypes = "Plasma/DataEngine";
        templateFilePath.append("mainDataEngine");
    } else if (ui->contentTypes->currentRow() == 2) {
        serviceTypes = "Plasma/Runner";
        templateFilePath.append("mainRunner");
    } else if (ui->contentTypes->currentRow() == 3) {
        serviceTypes = "Plasma/Theme";
    } else {
        serviceTypes = "Plasma/Applet";
        templateFilePath.append("mainPlasmoid");
    }

    // Append the desired extension
    if (ui->radioButtonPy->isChecked()) {
        metadata.setImplementationApi("python");
        projectFileExtension = ".py";
    } else if (ui->radioButtonRb->isChecked()) {
        metadata.setImplementationApi("ruby-script");
        projectFileExtension = ".rb";
    } else {
        metadata.setImplementationApi("javascript");
        projectFileExtension = ".js";
    }

    // Creating the corresponding folder
    QDir packageSubDirs(projectPath);
    packageSubDirs.mkpath("contents/code/");

    // Create a QFile object that points to the template we need to copy
    QFile sourceFile(templateFilePath + projectFileExtension);
    QFile destinationFile(projectPath + "contents/code/" + projectNameLowerCase + projectFileExtension);

    // Now open these files, and substitute the main class, author, email and date fields
    sourceFile.open(QIODevice::ReadOnly);
    destinationFile.open(QIODevice::ReadWrite);

    QByteArray rawData = sourceFile.readAll();

    QByteArray replacedString("$PLASMOID_NAME");
    if (rawData.contains(replacedString)) {
        rawData.replace(replacedString, projectName.toAscii());
    }
    replacedString.clear();


    // @schmidt-domine : thanks for committing the lines I forgot adding,
    // BUT please comply with the TAGS already assigned :)
    replacedString.append("$PLASMOID_NAME");
    if (rawData.contains(replacedString)) {
        rawData.replace(replacedString, projectName.toAscii());
    }
    replacedString.clear();

    replacedString.append("$DATAENGINE_NAME");
    if (rawData.contains(replacedString)) {
        rawData.replace(replacedString, projectName.toAscii());
    }
    replacedString.clear();

    replacedString.append("$RUNNER_NAME");
    if (rawData.contains(replacedString)) {
        rawData.replace(replacedString, projectName.toAscii());
    }
    replacedString.clear();

    replacedString.append("$AUTHOR");
    if (rawData.contains(replacedString)) {
        rawData.replace(replacedString, ui->authorTextField->text().toAscii());
    }
    replacedString.clear();

    replacedString.append("$EMAIL");
    if (rawData.contains(replacedString)) {
        rawData.replace(replacedString, ui->emailTextField->text().toAscii());
    }
    replacedString.clear();

    replacedString.append("$DATE");
    QDate date = QDate::currentDate();
    QByteArray datetime(date.toString().toUtf8());
    QTime time = QTime::currentTime();
    datetime.append(", " + time.toString().toUtf8());
    if (rawData.contains(replacedString)) {
        rawData.replace(replacedString, datetime);
    }

    destinationFile.write(rawData);
    destinationFile.close();

    // Write the .desktop file
    metadata.setName(projectName);
    metadata.setPluginName(projectNameLowerCase);
    metadata.setServiceType(serviceTypes);
    metadata.setAuthor(ui->authorTextField->text());
    metadata.setEmail(ui->emailTextField->text());
    metadata.setLicense("GPL");
    metadata.write(projectPath + "metadata.desktop");

    // Note: since PackageMetadata lacks of a good api to add X-Plasma-* entries,
    // we add it manually until a patch is released.
    KDesktopFile metaFile(projectPath + "metadata.desktop");
    KConfigGroup metaDataGroup = metaFile.desktopGroup();
    metaDataGroup.writeEntry("X-Plasma-MainScript", "code/" + projectNameLowerCase + projectFileExtension);
    metaDataGroup.writeEntry("X-Plasma-DefaultSize", QSize(200, 100));
    metaFile.sync();

    // the loading code expects the FOLDER NAME
    emit projectSelected(projectFolderName, serviceTypes);
}

void StartPage::cancelNewProject()
{
    resetStatus();
}

void StartPage::emitProjectSelected(const QModelIndex &index)
{
    QAbstractItemModel *m = ui->recentProjects->model();
    QString url = m->data(index, FullPathRole).value<QString>();
    kDebug() << "Loading project file:" << m->data(index, FullPathRole);

    emitProjectSelected(url, QString());
}

void StartPage::emitProjectSelected(const QString &name, const QString &type)
{
    emit projectSelected(name, type);
}

void StartPage::doImport()
{
    KUrl target = ui->importUrl->url();

    if (!target.isLocalFile() ||
          !QFile::exists(target.path()) ||
          QDir(target.path()).exists()) {
        KMessageBox::error(this, i18n("The file you entered is invalid."));
        return;
    }

    // we can do this because this is 'under the hood'
    // the user should never be aware of any 'folder names'
    // This nicely eliminates the need for a 'conflict resolution' dialog :)
    QString suggested = QFileInfo(target.path()).baseName();
    QString projectFolder = generateProjectFolderName(suggested);

    QString projectPath = KStandardDirs::locateLocal("appdata", projectFolder + '/');

    if (!Publisher::importPackage(target, projectPath)) {
        KMessageBox::information(this, i18n("A problem has occurred during import."));
    }
    emit projectSelected(projectFolder, QString());
}

void StartPage::showMoreDialog()
{
    projectManager->exec();
}

/**
 * Brain-dead way of generating a unique folder name.
 */
const QString StartPage::generateProjectFolderName(const QString& suggestion)
{
    QString projectFolder = suggestion;
    int suffix = 1;
    while (!KStandardDirs::locate("appdata", projectFolder + '/').isEmpty()) {
        projectFolder = suggestion + QString::number(suffix);
        suffix++;
    }
    return projectFolder;
}
