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
    ui = new Ui::StartPage;
    ui->setupUi(this);

    // Set some default parameters, like username/email and preferred scripting language
    KConfigGroup cg = KGlobal::config()->group("NewProjectDefaultPreferences");
    KUser user = KUser(KUser::UseRealUserID);

    QString userName = cg.readEntry("Username", user.loginName());
    QString userEmail = cg.readEntry("Email", userName+"@none.org");

    // If username or email are empty string, i.e. in the previous project the
    // developer deleted it, restore the default values
    if(userName.isEmpty()) {
        userName = user.loginName();
    }
    if(userEmail.isEmpty()) {
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
    connect(ui->contentTypes, SIGNAL(clicked(const QModelIndex)),
            this, SLOT(changeStackedWidgetPage()));
    connect(ui->contentTypes, SIGNAL(clicked(const QModelIndex)),
            ui->projectName, SLOT(setFocus()));
    connect(ui->newProjectButton, SIGNAL(clicked()),
            this, SLOT(createNewProject()));
    connect(ui->cancelNewProjectButton, SIGNAL(clicked()),
            this, SLOT(cancelNewProject()));
    connect(ui->importButton, SIGNAL(clicked()),
            this, SLOT(doImport()));

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

void StartPage::changeStackedWidgetPage()
{
    ui->layoutHackStackedWidget->setCurrentIndex(1);
    if (ui->contentTypes->currentRow() == 3) {
        ui->frame->setEnabled(false);
        //ui->radioButtonJs->setEnabled( false );
        //ui->radioButtonPy->setEnabled( false );
        //ui->radioButtonRb->setEnabled( false );
    }
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
    QStringList recentFiles = m_parent->recentProjects();

    for (int i = 0; i < recentFiles.size(); i++) {
        Plasma::PackageMetadata metadata(KStandardDirs::locateLocal("appdata", recentFiles.at(i) + '/'));

        // The loading code expects this to be the folder name, not the plasmoid name.
        // Note : Here we're effectively allowing plasmoid name and project folder name
        //        to be decoupled - so that the user is able to change the plasmoid name via
        //        the metadata editor, while the project folder name remains fixed.
        //        Alternatives are:
        //        1) Always rename the project folder when user changes plasmoid name
        //           - difficult to do and can get messy
        //        2) Don't allow the user to change plasmoid name once it is set
        //           - pretty inflexibly imo
        // QString projectName = metadata.name();
        QString projectName = recentFiles.at(i);

//         if (projectName.isEmpty()) {
//             continue;
//         }

        kDebug() << "adding" << projectName << "to the list of recent projects...";
        QListWidgetItem *item = new QListWidgetItem(projectName); // TODO make me the user "nice" name
        item->setData(FullPathRole, projectName.toLower());

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

        ui->recentProjects->addItem(item);
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
    QString projectPath = KStandardDirs::locateLocal("appdata", projectNameLowerCase + '/');

    Plasma::PackageMetadata metadata;

    // type -> serviceTypes
    QString serviceTypes;
    if (ui->contentTypes->currentRow() == 0) {
        serviceTypes = "Plasma/Applet";
        templateFilePath.append("mainPlasmoid");
    } else if (ui->contentTypes->currentRow() == 1) {
        serviceTypes = "Plasma/DataEngine";
        templateFilePath.append("mainDataEngine");
    } else if (ui->contentTypes->currentRow() == 2) {
        serviceTypes = "Plasma/Theme";
    } else if (ui->contentTypes->currentRow() == 3) {
        serviceTypes = "Plasma/Runner";
        templateFilePath.append("mainRunner");
    }

    // Append the desired extension
    if (ui->radioButtonJs->isChecked()) {
        metadata.setImplementationApi("javascript");
        projectFileExtension.append(".js");
    } else if (ui->radioButtonPy->isChecked()) {
        metadata.setImplementationApi("python");
        projectFileExtension.append(".py");
    } else if (ui->radioButtonRb->isChecked()) {
        metadata.setImplementationApi("ruby-script");
        projectFileExtension.append(".rb");
    }

    // Creating the corresponding folder
    QDir packageSubDirs(projectPath);
    packageSubDirs.mkpath("contents/code/");

    // Create a QFile object that points to the template we need to copy
    QFile sourceFile(templateFilePath + projectFileExtension);
    QFile destinationFile(projectPath +
                          "contents/code/" +
                          projectNameLowerCase +
                          projectFileExtension);

    // Now open these files, and substitute the main class, author, email and date fields
    sourceFile.open(QIODevice::ReadOnly);
    destinationFile.open(QIODevice::ReadWrite);

    QByteArray rawData = sourceFile.readAll();

    QByteArray replacedString("$PLASMOID_NAME");
    if(rawData.contains(replacedString)) {
        rawData.replace(replacedString, projectName.toAscii());
    }
    replacedString.clear();

    replacedString.append("$DATAENGINE_NAME");
    if(rawData.contains(replacedString)) {
        rawData.replace(replacedString, projectName.toAscii());
    }
    replacedString.clear();

    replacedString.append("$AUTHOR");
    if(rawData.contains(replacedString)) {
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
    QFile metaFile(projectPath + "metadata.desktop");
    metaFile.open(QIODevice::Append);
    QByteArray entry = "X-Plasma-MainScript=code/" +
                       projectNameLowerCase.toUtf8() +
                       projectFileExtension.toUtf8() +
                       "\n";
    metaFile.write(entry);
    entry.clear();
    entry.append("X-Plasma-DefaultSize=200,100\n");
    metaFile.write(entry);
    metaFile.close();

    emit projectSelected(projectNameLowerCase, serviceTypes);
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

    emit projectSelected(url, QString());
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

    // TODO: Should check for existing folder before importing.
    // In the short term show a simple overwrite-or-cancel dialog.
    // In the long term a more advanced conflict resolution dialog
    // that allows renaming should be offered here.
    QString projectName = QFileInfo(target.path()).baseName();
    QString projectPath = KStandardDirs::locateLocal("appdata", projectName + '/');
    if (!Publisher::importPackage(target, projectPath)) {
        KMessageBox::information(this, i18n("A problem has occured during import."));
    }
    emit projectSelected(projectName, QString());
}
