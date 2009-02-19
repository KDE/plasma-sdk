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

#include <KLocalizedString>
#include <KDebug>
#include <KLineEdit>
#include <KPushButton>
#include <KSeparator>
#include <KUrlRequester>
#include <KStandardDirs>

#include <Plasma/PackageMetadata>

#include "startpage.h"
#include "mainwindow.h"
#include "ui_startpage.h"

StartPage::StartPage(MainWindow *parent) // TODO set a palette so it will look identical with any color scheme.
    : QWidget(parent),
    m_parent(parent)
{
    setupWidgets();
    refreshRecentProjectsList();
}

void StartPage::setupWidgets()
{
    ui = new Ui::StartPage;
    ui->setupUi(this);
    
    // Enforce the security restriction from package.cpp in the input field
    QRegExpValidator *pluginname_validator = new QRegExpValidator(ui->projectName);
    QRegExp validatePluginName("^[\\w-\\.]+$"); // Only allow letters, numbers, underscore and period.
    pluginname_validator->setRegExp(validatePluginName);
    
    connect(ui->recentProjects, SIGNAL(clicked(const QModelIndex)),
            this, SLOT(emitProjectSelected(const QModelIndex)));
    connect(ui->contentTypes, SIGNAL(clicked(const QModelIndex)),
            this, SLOT(changeStackedWidgetPage()));
    connect(ui->newProjectButton, SIGNAL(clicked()),
            this, SLOT(createNewProject()));
    
    new QListWidgetItem(KIcon("application-x-plasma"), i18n("Plasmoid"), ui->contentTypes);
    new QListWidgetItem(KIcon("kexi"), i18n("Data Engine"), ui->contentTypes);
    new QListWidgetItem(KIcon("system-run"), i18n("Runner"), ui->contentTypes);
    new QListWidgetItem(KIcon("inkscape"), i18n("Theme"), ui->contentTypes);

//     connect(ui->newProjectButton, SIGNAL(clicked()), this, SLOT(launchNewProjectWizard()));
}

void StartPage::changeStackedWidgetPage()
{
    ui->layoutHackStackedWidget->setCurrentIndex(1);
}

void StartPage::refreshRecentProjectsList()
{
    ui->recentProjects->clear();
    QList<KUrl> recentFiles = m_parent->recentProjects();
    
    for (int i = 0; i < recentFiles.size(); i++) {
        Plasma::PackageMetadata metadata(recentFiles.at(i).path());
        QString projectName = metadata.name();
        
        if (projectName.isEmpty()) {
            continue;
        }
        
        kDebug() << projectName;
        QListWidgetItem *item = new QListWidgetItem(projectName);
        item->setData(FullPathRole, projectName);
        
        QString serviceType = metadata.serviceType();
        
        if ( serviceType == QString("Plasma/Applet") ) {
            item->setIcon(KIcon("application-x-plasma"));
        } else if ( serviceType == QString("Plasma/DataEngine") ) {
            item->setIcon(KIcon("kexi"));
        } else if ( serviceType == QString("Plasma/Theme") ) {
            item->setIcon(KIcon("inkscape"));
        } else if ( serviceType == QString("Plasma/Runner") ) {
            item->setIcon(KIcon("system-run"));
        } else {
            kWarning() << "Unknown service type" << serviceType;
        }
        
        ui->recentProjects->addItem(item);
    }
}

void StartPage::createNewProject()
{
    Plasma::PackageMetadata *metadata = new Plasma::PackageMetadata;
    
    QString filename = KStandardDirs::locateLocal("appdata", ui->projectName->text().toLower() + "/metadata.desktop");

    metadata->setName(ui->projectName->text());

    if (ui->contentTypes->currentRow() == 0) {
        metadata->setServiceType("Plasma/Applet");
    } else if (ui->contentTypes->currentRow() == 1) {
        metadata->setServiceType("Plasma/DataEngine");
    } else if (ui->contentTypes->currentRow() == 2) {
        metadata->setServiceType("Plasma/Theme");
    } else if (ui->contentTypes->currentRow() == 3) {
        metadata->setServiceType("Plasma/Runner");
    }

// TODO
//     metadata->setPluginName( view->pluginname_edit->text() );

// TODO: Fill with KUser, default to GPL
//     metadata->setAuthor( view->author_edit->text() );
//     metadata->setEmail( view->email_edit->text() );
//     metadata->setLicense( view->license_edit->text() );

    metadata->write(filename);
    
    KUrl url = filename;
    
    emit projectSelected(url);
}

void StartPage::emitProjectSelected(const QModelIndex &index)
{
    QAbstractItemModel *m = ui->recentProjects->model();
    KUrl url = m->data(index, FullPathRole).value<KUrl>();
    kDebug() << "Loading project file:" << m->data(index, FullPathRole);
    emit projectSelected(url);
}
