/*
Copyright 2009 Riccardo Iaconelli <riccardo@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#include <knewstuff3/downloaddialog.h>

#include "packagemodel.h"
#include "startpage.h"
#include "mainwindow.h"
#include "ui_startpage.h"
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

    ui->radioButtonJs->setChecked(cg.readEntry("radioButtonJsChecked", false));
    ui->radioButtonPy->setChecked(cg.readEntry("radioButtonPyChecked", false));
    ui->radioButtonRb->setChecked(cg.readEntry("radioButtonRbChecked", false));
    ui->radioButtonDe->setChecked(cg.readEntry("radioButtonDeChecked", true));

    ui->cancelNewProjectButton->setIcon(KIcon("dialog-cancel"));
    ui->newProjectButton->setIcon(KIcon("dialog-ok"));
    ui->titleWidget->setPixmap(KIcon("plasmagik").pixmap(48, 48), KTitleWidget::ImageRight);

    // Enforce the security restriction from package.cpp in the input field
    connect(ui->projectName, SIGNAL(textEdited(const QString&)),
            this, SLOT(processProjectName(const QString&)));

    connect(ui->projectName, SIGNAL(returnPressed()),
            this, SLOT(createNewProject()));
    connect(ui->recentProjects, SIGNAL(clicked(const QModelIndex)),
            this, SLOT(emitProjectSelected(const QModelIndex)));

    // Enforce the security restriction from package.cpp in the input field
    connect(ui->localProject, SIGNAL(textEdited(const QString&)),
            this, SLOT(processLocalProject(const QString&)));

    connect(ui->localProject, SIGNAL(returnPressed()),
            this, SLOT(loadLocalProject()));
    connect(ui->loadLocalProject, SIGNAL(clicked()),
            this, SLOT(loadLocalProject()));

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
    connect(ui->importGHNSButton, SIGNAL(clicked()),
            this, SLOT(doGHNSImport()));
    connect(ui->moreButton, SIGNAL(clicked()),
            this, SLOT(showMoreDialog()));

    connect(projectManager, SIGNAL(projectSelected(QString, QString)),
            this, SLOT(emitProjectSelected(QString, QString)));
    connect(projectManager, SIGNAL(requestRefresh()),
            this, SLOT(refreshRecentProjectsList()));

    new QListWidgetItem(KIcon("application-x-plasma"), i18n("Plasma Widget"), ui->contentTypes);
    new QListWidgetItem(KIcon("kexi"), i18n("Data Engine"), ui->contentTypes);
    new QListWidgetItem(KIcon("system-run"), i18n("Runner"), ui->contentTypes);
    new QListWidgetItem(KIcon("inkscape"), i18n("Theme"), ui->contentTypes);

//     connect(ui->newProjectButton, SIGNAL(clicked()), this, SLOT(launchNewProjectWizard()));
}

// Convert FooBar to foo_bar
QString StartPage::camelToSnakeCase(const QString& name)
{
    QString result(name);
    return result.replace(QRegExp("([A-Z])"), "_\\1").toLower().replace(QRegExp("^_"), "");
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

    ui->newProjectButton->setEnabled(!ui->projectName->text().isEmpty());
}

void StartPage::validateProjectType(const QModelIndex &sender)
{
    if (sender.row() == 1) {
        ui->languageLabel->setEnabled(true);
        ui->radioButtonJs->setEnabled(true);
        // gotta explicitly setEnabled to true in case it
        // was falsed before!
        ui->radioButtonPy->setEnabled(true);
        // also check this radio to prevent any disabled radios from
        // being checked due to previous setting!
        ui->radioButtonRb->setEnabled(true);
        ui->radioButtonDe->setEnabled(true);
        ui->newProjectButton->setEnabled(!ui->projectName->text().isEmpty()); // in case previously falsed
    } else if (sender.row() == 2) {
        ui->languageLabel->setEnabled(true);
        ui->radioButtonJs->setEnabled(true);
        ui->radioButtonJs->setChecked(true);
        ui->radioButtonPy->setEnabled(true);
        ui->radioButtonRb->setEnabled(false);
        ui->radioButtonDe->setEnabled(true);
        ui->newProjectButton->setEnabled(!ui->projectName->text().isEmpty()); // in case previously falsed

    } else if (sender.row() == 3) {
        ui->languageLabel->setEnabled(false);
        ui->radioButtonJs->setEnabled(false);
        ui->radioButtonPy->setEnabled(false);
        ui->radioButtonRb->setEnabled(false);
        ui->radioButtonDe->setEnabled(false);
        ui->newProjectButton->setEnabled(false); // disable the create button too.
    } else /* if (sender.row() == 0) */ {
        ui->radioButtonJs->setEnabled(true);
        ui->radioButtonJs->setChecked(true);
        ui->radioButtonDe->setEnabled(true);
        ui->radioButtonPy->setEnabled(true);
        ui->radioButtonRb->setEnabled(true);
        ui->newProjectButton->setEnabled(!ui->projectName->text().isEmpty()); // in case previously falsed
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

bool StartPage::selectedDeRadioButton()
{
    return ui->radioButtonDe->isChecked();
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

    foreach(const QString file, recentFiles) {
        // Specify path + filename as well to avoid mistaking .gitignore
        // as being the metadata file.
        QString f = file;
        QDir pDir(file);
        QString pPath =  file + "/metadata.desktop";
        kDebug() << "RECENT FILE::: " << file;
        if (pDir.isRelative()) {
            kDebug() << "NOT LOCAL";
            pPath = KStandardDirs::locateLocal("appdata", file + "/metadata.desktop");
        }
        Plasma::PackageMetadata metadata(pPath);

        // Changed this back on second thought. It's better that we
        // do not expose the idea of a 'folder name' to the user -
        // we keep the folder hidden and name it whatever we want as
        // long as it's unique. Only the plasmoid name, which the user
        // sets in the metadata editor, will ever be visible in the UI.
        QString projectName = metadata.name();

        kDebug() << "adding" << projectName << "to the list of recent projects...";
        QListWidgetItem *item = new QListWidgetItem(projectName); // show the user-set plasmoid name in the UI

        // the loading code uses this to find the project to load.
        // since folder name and plasmoid name can be different, this
        // should be set to the folder name, which the loading code expects.
        item->setData(FullPathRole, file);

        // set a tooltip for extra info and to help differentiating similar projects
        QString tooltip = "Project : " + projectName + "\n\n";
        tooltip += "\"" + metadata.description() + "\"\n---\n";
        tooltip += i18n("Author") + " : " + metadata.author() + " <" + metadata.email() + ">\n";
        tooltip += i18n("Version") + " : " + metadata.version() + "\n";
        tooltip += i18n("API") + " : " + metadata.implementationApi() + "\n";

        QString serviceType = metadata.serviceType();

        if (serviceType == "Plasma/Applet" ||
            serviceType == "Plasma/PopupApplet") {
            item->setIcon(KIcon("application-x-plasma"));
            tooltip += i18n("Project type") + " : " + i18n("Plasmoid");
        } else if (serviceType == "Plasma/DataEngine") {
            item->setIcon(KIcon("kexi"));
            tooltip += i18n("Project type") + " : " + i18n("Data Engine");
        } else if (serviceType == "Plasma/Theme") {
            item->setIcon(KIcon("inkscape"));
            tooltip += i18n("Project type") + " : " + i18n("Theme");
        } else if (serviceType == "Plasma/Runner") {
            item->setIcon(KIcon("system-run"));
            tooltip += i18n("Project type") + " : " + i18n("Runner");
        } else {
            kWarning() << "Unknown service type" << serviceType;
        }
        item->setToolTip(tooltip);

        projectManager->addProject(item);
        // limit to 5 projects to display up front
        if (ui->recentProjects->count() < 5) {
            ui->recentProjects->addItem(new QListWidgetItem(*item));
        }
    }
}

void StartPage::createNewProject()
{
    // packagePath -> projectPath
    const QString projectName = ui->projectName->text();
    if (projectName.isEmpty()) {
        return;
    }

    kDebug() << "Creating simple folder structure for the project " << projectName;
    QString projectNameLowerCase = projectName.toLower();
    QString projectNameSnakeCase = camelToSnakeCase(projectName);
    QString projectFileExtension;

    QString templateFilePath = KStandardDirs::locate("appdata", "templates/");

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

    QString projectFolderName;
    QString mainScriptName;

    // Append the desired extension
    if (ui->radioButtonPy->isChecked()) {
        metadata.setImplementationApi("python");
        projectFolderName = generateProjectFolderName(projectNameLowerCase);
        projectFileExtension = ".py";
        mainScriptName = projectNameLowerCase + projectFileExtension;
    } else if (ui->radioButtonRb->isChecked()) {
        metadata.setImplementationApi("ruby-script");
        projectFolderName = generateProjectFolderName(projectNameSnakeCase);
        projectFileExtension = ".rb";
        mainScriptName = QString("main_") + projectNameSnakeCase + projectFileExtension;
    } else if (ui->radioButtonDe->isChecked()) {
        metadata.setImplementationApi("declarativeappletscript");
        projectFolderName = generateProjectFolderName(projectNameLowerCase);
        projectFileExtension = ".qml";
        mainScriptName = projectNameLowerCase + projectFileExtension;
    } else {
        metadata.setImplementationApi("javascript");
        projectFolderName = generateProjectFolderName(projectNameLowerCase);
        projectFileExtension = ".js";
        mainScriptName = projectNameLowerCase + projectFileExtension;
    }
    
    // Creating the corresponding folder

    //  From this commit, the directory is changed int the following way. Old directory structure:
    //  <plasmateProjDir>/projectname/metadata.desktop
    //                               /NOTES
    //                               /.git/
    //                               /.gitignore
    //                               /contents/...
    //  New directory structure:
    //  <plasmateProjDir>/projectname/NOTES
    //                               /.git/..
    //                               /.gitignore
    //                               /projectname/metadata.desktop
    //                                           /contents/..
    //  The reason of this change is simple: with the old directory structure, packaging the project
    //  implied also packaging the whole git history, and the NOTES file as well (which are plasmate-only
    //  related files). With the new structure, this won't happen again :)
    //  Besides, this change will allow us to start implementing the per-project config file :)
    //
    QString projectPath = KStandardDirs::locateLocal("appdata",projectFolderName + '/');
    QDir packageSubDirs(projectPath);
    packageSubDirs.mkpath(projectFolderName + "/contents/code/");

    // Create a QFile object that points to the template we need to copy
    QFile sourceFile(templateFilePath + projectFileExtension);
    QFile destinationFile(projectPath + projectFolderName +  "/contents/code/" + mainScriptName);

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
    //FIXME: the plugin name needs to be globally unique, so should use more than just the project
    //       name
    metadata.setPluginName(projectNameLowerCase);
    metadata.setServiceType(serviceTypes);
    metadata.setAuthor(ui->authorTextField->text());
    metadata.setEmail(ui->emailTextField->text());
    metadata.setLicense("GPL");
    metadata.write(projectPath + projectFolderName + "/metadata.desktop");

    // Note: since PackageMetadata lacks of a good api to add X-Plasma-* entries,
    // we add it manually until a patch is released.
    KDesktopFile metaFile(projectPath + projectFolderName + "/metadata.desktop");
    KConfigGroup metaDataGroup = metaFile.desktopGroup();
    metaDataGroup.writeEntry("X-Plasma-MainScript", "code/" + mainScriptName);
    metaDataGroup.writeEntry("X-Plasma-DefaultSize", QSize(200, 100));
    metaFile.sync();

    // Create the notes file
    QFile notesFile(projectPath + "/NOTES");
    notesFile.open(QIODevice::ReadWrite);
    notesFile.close();

    // the loading code expects the FOLDER NAME
    emit projectSelected(projectFolderName + "/" + projectFolderName, serviceTypes);

    // need to clear the project name field here too because startpage is still 
    // accessible after project loads.
    ui->projectName->clear();
}

void StartPage::cancelNewProject()
{
    ui->projectName->clear();
    resetStatus();
}

void StartPage::processLocalProject(const QString& name)
{
    ui->loadLocalProject->setEnabled(name.isEmpty());
}

void StartPage::loadLocalProject()
{
    QString p = ui->localProject->text();
    kDebug() << "loading local project from" << p;
    emit projectSelected(p, QString());
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

    selectProject(target);
}

void StartPage::doGHNSImport()
{
    KNS3::DownloadDialog *mNewStuffDialog = new KNS3::DownloadDialog("plasmate.knsrc", this);
    if (mNewStuffDialog->exec() == QDialog::Accepted)
    {
        KNS3::Entry::List installed = mNewStuffDialog->installedEntries();

        if (!installed.empty())
        {
            KNS3::Entry entry = installed.at(0);
            QStringList installedFiles = entry.installedFiles();

            if (!installedFiles.empty())
            {
                QString file = installedFiles.at(0);
                KUrl target(file);

                selectProject(target);
            }
        }
    }
}

void StartPage::selectProject(const KUrl &target)
{
    if (!target.isLocalFile() ||
          !QFile::exists(target.path()) ||
          QDir(target.path()).exists()) {
        KMessageBox::error(this, i18n("The file you entered is invalid."));
        return;
    }

    // we can do this because this is 'under the hood'
    // the user should never be aware of any 'folder names'
    // This nicely eliminates the need for a 'conflict resolution' dialog :)
    QString suggested = QFileInfo(target.path()).completeBaseName();
    QString projectFolder = generateProjectFolderName(suggested);

    QString projectPath = KStandardDirs::locateLocal("appdata", projectFolder + '/');

    if (!ProjectManager::importPackage(target, projectPath)) {
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
