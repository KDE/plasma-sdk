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

#include <QListWidget>
#include <QListWidgetItem>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QValidator>
#include <QFile>
#include <QScopedPointer>
#include <QStandardPaths>

#include <KUser>
#include <KLocalizedString>
#include <QDebug>
#include <QIcon>
#include <KLineEdit>
#include <KPluginInfo>
#include <QPushButton>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KShell>
#include <KUrlRequester>
#include <KMessageBox>
#include <KMessageWidget>
#include <KNS3/DownloadDialog>

#include "editors/metadata/metadatahandler.h"
#include "packagemodel.h"
#include "startpage.h"
#include "mainwindow.h"
#include "packagehandler.h"
#include "projectmanager/projectmanager.h"
#include "projecthandler.h"


StartPage::StartPage(PackageHandler *packageHandler, MainWindow *parent) // TODO set a palette so it will look identical with any color scheme.
        : QWidget(parent),
         m_parent(parent),
         m_projectHandler(new ProjectHandler()),
         m_packageHandler(packageHandler)
{
    m_mainWindow = parent;
    setupWidgets();
    refreshRecentProjectsList();
}

StartPage::~StartPage()
{
}

void StartPage::setupWidgets()
{
    m_ui.setupUi(this);

    m_ui.invalidPlasmagikLabelEmpty->setVisible(false);
    m_ui.invalidPlasmagikLabelNoMetadataDesktop->setVisible(false);

    // Set some default parameters, like username/email and preferred scripting language
    KConfigGroup cg(KSharedConfig::openConfig(), QStringLiteral("ProjectDefaultPreferences"));
    KUser user = KUser(KUser::UseRealUserID);

    QString userName = cg.readEntry("Username", user.loginName());
    QString userEmail = cg.readEntry("Email", userName);

    // If username or email are empty string, i.e. in the previous project the
    // developer deleted it, restore the default values
    if (userName.isEmpty()) {
        userName = user.loginName();
    }

    if (userEmail.isEmpty()) {
        userEmail.append(user.loginName()+"@none.org");
    } else if (userEmail == userName) {
        //the first time that we run plasmate, if our config doesn't exists
        //then the userEmail==userName, so se append the correct email prefix
        userEmail.append(QStringLiteral("@none.org"));
    }

    m_ui.authorTextField->setText(userName);
    m_ui.emailTextField->setText(userEmail);

    m_ui.cancelNewProjectButton->setIcon(QIcon::fromTheme("draw-arrow-back"));
    m_ui.newProjectButton->setIcon(QIcon::fromTheme("dialog-ok"));
    m_ui.loadLocalProject->setEnabled(false);
    m_ui.importPackageButton->setEnabled(false);

    // Enforce the security restriction from package.cpp in the input field
    connect(m_ui.projectName, &KLineEdit::textEdited, [&](const QString &name) {
        QRegExp validatePluginName("[a-zA-Z0-9_.]*");
        if (!validatePluginName.exactMatch(name)) {
            int pos = 0;
            for (int i = 0; i < name.size(); i++) {
                if (validatePluginName.indexIn(name, pos, QRegExp::CaretAtZero) == -1)
                    break;
                pos += validatePluginName.matchedLength();
            }
            m_ui.projectName->setText(QString(name).remove(pos, 1));
        }

        m_ui.newProjectButton->setEnabled(!m_ui.projectName->text().isEmpty());
    });

    // Enforce the security restriction from package.cpp in the input field
    connect(m_ui.localProject, &KUrlRequester::textChanged, this, &StartPage::checkLocalProjectPath);

    connect(m_ui.loadLocalProject, &QPushButton::clicked, [&]() {
        const QString path = KShell::tildeExpand(m_ui.localProject->text());
        qDebug() << "loading local project from" << path;
        if (!QFile::exists(path)) {
            return;
        }

        m_projectHandler->recentProject(path);
        resetStatus();

        //load our project
        QString metadataDesktop = path;
        const QString projectPath = metadataDesktop.replace(QStringLiteral("metadata.desktop"), "");
        m_packageHandler->setPackagePath(projectPath);
        m_packageHandler->loadPackage();
        m_mainWindow->loadProject(findMainScript(projectPath));
        emit projectSelected(path);
    });

    connect(m_ui.importPackageButton, &QPushButton::clicked, [&]() {
            const QUrl target = m_ui.importPackage->url();
            selectProject(target);

            m_projectHandler->recentProject(target.toLocalFile());
            resetStatus();
    });

    connect(m_ui.contentTypes, &QListWidget::clicked, [&](const QModelIndex &sender) {
        m_ui.languageLabel->show();
        m_ui.frame->show();
        m_ui.radioButtonJs->setEnabled(true);
        m_ui.radioButtonDe->setEnabled(true);
        m_ui.radioButtonJs->hide();
        m_ui.radioButtonDe->hide();

        if (sender.row() == ThemeRow) {
            m_ui.languageLabel->hide();
            m_ui.frame->hide();
        } else if (sender.row() == PlasmoidRow) {
            m_ui.radioButtonDe->setVisible(true);
        } else if (sender.row() == WindowSwitcherRow) {
            m_ui.radioButtonDe->setVisible(true);
            m_ui.radioButtonJs->setVisible(true);
        } else if (sender.row() == KWinScriptRow) {
            m_ui.radioButtonJs->setVisible(true);
        } else if (sender.row() == KWinEffectRow) {
            m_ui.radioButtonJs->setVisible(true);
        }

        m_ui.newProjectButton->setEnabled(!m_ui.projectName->text().isEmpty());
        m_ui.layoutHackStackedWidget->setCurrentIndex(1);
        m_ui.projectName->setFocus();
    });

    connect(m_ui.newProjectButton, &QPushButton::clicked, this, &StartPage::createNewProject);

    connect(m_ui.cancelNewProjectButton, &QPushButton::clicked, [&]() {
        m_ui.projectName->clear();
        resetStatus();
    });

    connect(m_ui.importGHNSButton, &QPushButton::clicked, [&]() {
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
                    QUrl target(file);

                    selectProject(target);
                }
            }
        }
    });

    connect(m_ui.recentProjects, &QListWidget::clicked, [=](const QModelIndex &index) {
        QAbstractItemModel *m = m_ui.recentProjects->model();
        QString url = m->data(index, FullPathRole).value<QString>();
        if (url.isEmpty()) {
            QScopedPointer<ProjectManager> projectManager(new ProjectManager(m_projectHandler, m_packageHandler, m_mainWindow));
            if (projectManager->exec() == QDialog::Accepted) {
                resetStatus();
            }

            return;
        } else {
            m_packageHandler->setPackagePath(url);
            m_packageHandler->loadPackage();
            m_mainWindow->loadProject(findMainScript(url));
        }

        qDebug() << "Loading project file:" << m->data(index, FullPathRole);

        emit projectSelected(url);
    });

    new QListWidgetItem(QIcon::fromTheme("application-x-plasma"), i18n("Plasma Widget"), m_ui.contentTypes);
    new QListWidgetItem(QIcon::fromTheme("inkscape"), i18n("Theme"), m_ui.contentTypes);
    new QListWidgetItem(QIcon::fromTheme("window-duplicate"), i18n("Window Switcher"), m_ui.contentTypes);
    new QListWidgetItem(QIcon::fromTheme("preferences-system-windows-actions"), i18n("KWin Script"), m_ui.contentTypes);
    new QListWidgetItem(QIcon::fromTheme("preferences-system-windows-effect"), i18n("KWin Effect"), m_ui.contentTypes);
}

// Convert FooBar to foo_bar
QString StartPage::camelToSnakeCase(const QString& name)
{
    QString result(name);
    return result.replace(QRegExp("([A-Z])"), "_\\1").toLower().replace(QRegExp("^_"), "");
}

QString StartPage::userName()
{
    return m_ui.authorTextField->text();
}

QString StartPage::userEmail()
{
    return m_ui.emailTextField->text();
}

void StartPage::resetStatus()
{
    qDebug() << "Reset status!";
    updateProjectPreferences();
    m_ui.layoutHackStackedWidget->setCurrentIndex(0);
    refreshRecentProjectsList();
}

void StartPage::updateProjectPreferences()
{
    KConfigGroup cg(KSharedConfig::openConfig(), QStringLiteral("ProjectDefaultPreferences"));
    cg.writeEntry(QStringLiteral("Username"), userName());
    cg.writeEntry(QStringLiteral("EMail"), userEmail());
    cg.sync();
}

void StartPage::refreshRecentProjectsList()
{
    m_ui.recentProjects->clear();

    int counter = 0;
    for (const QString &file : m_projectHandler->loadProjectsList()) {
        // Specify path + filename as well to avoid mistaking .gitignore
        // as being the metadata file.
        QDir pDir(file);
        QString pPath =  file + "/metadata.desktop";
        qDebug() << "RECENT FILE::: " << file;

        if (!QFile::exists(pPath)) {
            continue;
        }

        KPluginInfo metadata(pPath);
        // Do not expose the idea of a 'folder name' to the user -
        // we keep the folder hidden and name it whatever we want as
        // long as it's unique. Only the plasmoid name, which the user
        // sets in the metadata editor, will ever be visible in the UI.
        const QString projectName = metadata.name();

        qDebug() << "adding" << projectName << "to the list of recent projects...";
        QListWidgetItem *item = new QListWidgetItem(projectName); // show the user-set plasmoid name in the UI
        counter++;
        // the loading code uses this to find the project to load.
        // since folder name and plasmoid name can be different, this
        // should be set to the folder name, which the loading code expects.
        item->setData(FullPathRole, file);

        // set a tooltip for extra info and to help differentiating similar projects
        QString tooltip = i18n("Project: %1", projectName) + "\n\n";
        if (!metadata.comment().isEmpty()) {
            tooltip += "\"" + metadata.comment() + "\"\n---\n";
        }

        if (!metadata.author().isEmpty()) {
            tooltip += i18n("Author: %1", metadata.author());
            if (!metadata.email().isEmpty()) {
                tooltip += " <" + metadata.email() + '>';
            }

            tooltip += "\n";
        }

        if (!metadata.version().isEmpty()) {
            tooltip += i18n("Version: %1", metadata.version()) + "\n";
        }


        KService service(pPath);
        tooltip += i18n("API: %1", service.property("X-Plasma-API").toString()) + "\n";
        item->setToolTip(tooltip);

        const QStringList serviceTypes = service.serviceTypes();

        QString defaultIconName;

       if (serviceTypes.contains("KWin/WindowSwitcher")) {
            defaultIconName = "window-duplicate";
            tooltip += i18n("Project Type: Window Switcher");
        } else if (serviceTypes.contains("Plasma/Applet")) {
            defaultIconName = "plasma";
            tooltip += i18n("Project type: Plasmoid");
        } else if (serviceTypes.contains("Plasma/Theme")) {
            defaultIconName = "preferences-desktop-theme";
            tooltip += i18n("Project type: Theme");
        } else if (serviceTypes.contains("KWin/Script")) {
            defaultIconName = "preferences-system-windows-actions";
            tooltip += i18n("Project type: KWin Script");
        } else if (serviceTypes.contains("KWin/Effect")) {
            defaultIconName = "preferences-system-windows-effect";
            tooltip += i18n("Project type: KWin Script");
        } else {
            qWarning() << "Unknown service type" << serviceTypes;
        }

        if (metadata.icon().isEmpty()) {
            item->setIcon(QIcon::fromTheme(defaultIconName));
        } else {
            item->setIcon(QIcon::fromTheme(metadata.icon()));
        }

        // limit to 5 projects to display up front
        if (m_ui.recentProjects->count() < 5) {
            m_ui.recentProjects->addItem(new QListWidgetItem(*item));
        }
    }

    QListWidgetItem *more;

    if (counter >= 0 && counter <= 4) {
        more = new QListWidgetItem(i18n("Manage Projects..."));
    } else {
        more = new QListWidgetItem(i18n("More projects..."));
    }
    more->setIcon(QIcon::fromTheme("window-new"));
    m_ui.recentProjects->addItem(more);
}

void StartPage::createNewProject()
{
    // packagePath -> projectPath
    const QString projectName = m_ui.projectName->text();
    if (projectName.isEmpty()) {
        return;
    }

    qDebug() << "Creating simple folder structure for the project " << projectName;
    const QString projectNameLowerCase = projectName.toLower();
    QString projectFileExtension;

    // type -> serviceTypes
    QString serviceTypes;
    if (m_ui.contentTypes->currentRow() == 0) {
        serviceTypes = "Plasma/Applet";
    } else if (m_ui.contentTypes->currentRow() == 1) {
        serviceTypes = "Plasma/Theme";
    } else if (m_ui.contentTypes->currentRow() == 2) {
        serviceTypes = "KWin/WindowSwitcher";
    } else if (m_ui.contentTypes->currentRow() == 3) {
        serviceTypes = "KWin/Script";
    } else if (m_ui.contentTypes->currentRow() == 4) {
        serviceTypes = "KWin/Effect";
    }

    QString projectFolderName;
    QString mainScriptName;
    QString api;

    // Append the desired extension
    if (m_ui.radioButtonDe->isChecked()) {
        api = "declarativeappletscript";
        projectFolderName = generateProjectFolderName(projectNameLowerCase);
        projectFileExtension = ".qml";
        mainScriptName = projectNameLowerCase + projectFileExtension;
    } else {
        api = "javascript";
        projectFolderName = generateProjectFolderName(projectNameLowerCase);
        projectFileExtension = ".js";
        mainScriptName = projectNameLowerCase + projectFileExtension;
    }

    // Creating the corresponding folder

    //  The correct directory structure is,
    //              <plasmateProjDir>/
    //                               NOTES
    //                               .git/
    //                               .gitignore
    //                               contents/...

    const QString projectPath = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0) + QLatin1Char('/') + projectFolderName;
    m_packageHandler->setPackagePath(projectPath);
    m_packageHandler->createPackage(m_ui.authorTextField->text(), m_ui.emailTextField->text(), serviceTypes, projectNameLowerCase, mainScriptName, api, projectFileExtension);
    m_packageHandler->loadPackage();

    // create the metadata.desktop file
    // TODO: missing but possible entries that could be added:
    // * Icon
    // * Comment
    // * Keywords
    // * X-KDE-PluginInfo-Website
    // * X-KDE-PluginInfo-Category
    // * X-KDE-ParentApp

    emit projectSelected(projectPath);

    // need to clear the project name field here too because startpage is still
    // accessible after project loads.
    m_ui.projectName->clear();
    resetStatus();

    //load our new project
    m_mainWindow->loadProject(findMainScript(projectPath));
}

void StartPage::checkLocalProjectPath(const QString& name)
{
    m_ui.invalidPlasmagikLabelEmpty->setText(i18n("Your directory is empty"));

    m_ui.invalidPlasmagikLabelEmpty->setMessageType(KMessageWidget::Error);
    m_ui.invalidPlasmagikLabelNoMetadataDesktop->setMessageType(KMessageWidget::Error);

    m_ui.invalidPlasmagikLabelEmpty->setCloseButtonVisible(false);
    m_ui.invalidPlasmagikLabelNoMetadataDesktop->setCloseButtonVisible(false);

    m_ui.invalidPlasmagikLabelEmpty->setVisible(false);
    m_ui.invalidPlasmagikLabelNoMetadataDesktop->setVisible(false);

    QFile metadataDesktop(name);
    qDebug() << "checking: " << name;
    m_ui.loadLocalProject->setEnabled(metadataDesktop.exists());

    if (name.isEmpty()) {
        m_ui.invalidPlasmagikLabelEmpty->setVisible(true);
        m_ui.invalidPlasmagikLabelNoMetadataDesktop->setVisible(false);
    } else if (!metadataDesktop.exists() && !metadataDesktop.fileName().endsWith(QStringLiteral("metadata.desktop"))) {
        m_ui.invalidPlasmagikLabelEmpty->setVisible(false);
        m_ui.invalidPlasmagikLabelNoMetadataDesktop->setVisible(true);
        m_ui.invalidPlasmagikLabelNoMetadataDesktop->setText(i18n("metadata.desktop does not exist in %1", name));
    } else if (!name.isEmpty()) {
        m_ui.invalidPlasmagikLabelEmpty->setVisible(false);
        m_ui.invalidPlasmagikLabelNoMetadataDesktop->setVisible(false);
    }
}

void StartPage::selectProject(const QUrl &target)
{
    if (!target.isLocalFile() || !QFile::exists(target.path()) || QDir(target.path()).exists()) {
        KMessageBox::error(this, i18n("The file you entered is invalid."));
        return;
    }

    // we can do this because this is 'under the hood'
    // the user should never be aware of any 'folder names'
    // This nicely eliminates the need for a 'conflict resolution' dialog :)
    QString suggested = QFileInfo(target.path()).completeBaseName();
    QString projectFolder = generateProjectFolderName(suggested);

    QString projectPath = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0) + projectFolder + QStringLiteral("/");
    const QUrl projectUrl(projectPath);

    if (!ProjectManager::importPackage(target, projectUrl)) {
        KMessageBox::information(this, i18n("A problem has occurred during import."));
    }
    emit projectSelected(projectFolder);
}

/**
 * Brain-dead way of generating a unique folder name.
 */
const QString StartPage::generateProjectFolderName(const QString& suggestion)
{
    QString projectFolder = suggestion;
    int suffix = 1;
    while (!QStandardPaths::locateAll(QStandardPaths::DataLocation, projectFolder).isEmpty()) {
        projectFolder = suggestion + QString::number(suffix);
        suffix++;
    }
    return projectFolder;
}

QString StartPage::findMainScript(const QString &projectPath) const
{
    QString mainScriptPath(projectPath);

    MetadataHandler metadataHandler;
    metadataHandler.setFilePath(mainScriptPath + QLatin1Char('/') + QStringLiteral("metadata.desktop"));
    mainScriptPath += QStringLiteral("/contents/") + metadataHandler.mainScript();
    return mainScriptPath;
}

