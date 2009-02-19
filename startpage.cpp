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
        QString project = recentFiles.at(i).directory();
        kDebug() << project;
        QListWidgetItem *item = new QListWidgetItem(project);
        item->setData(FullPathRole, project);
        ui->recentProjects->addItem(item);
    }
}

void StartPage::createNewProject()
{
    
}

void StartPage::emitProjectSelected(const QModelIndex &index)
{
    QAbstractItemModel *m = ui->recentProjects->model();
    KUrl url = m->data(index, FullPathRole).value<KUrl>();
    kDebug() << "Loading project file:" << m->data(index, FullPathRole);
    emit projectSelected(url);
}
