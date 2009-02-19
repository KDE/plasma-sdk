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
#include <KPushButton>

#include "startpage.h"
#include "mainwindow.h"
#include "ui_newproject.h"

StartPage::StartPage(MainWindow *parent)
    : QWidget(parent),
    m_parent(parent)
{
    createWidgets();
    populateRecentProjects();
}

void StartPage::createWidgets() // Make this a QGV? Use Plasma::Theme?
{
    m_layout = new QHBoxLayout(this);
    
    QVBoxLayout *continueWorkingLayout = new QVBoxLayout;
    m_continueWorkingLabel = new QLabel(i18n("Continue working on.."), this);
    m_recentProjects = new QListWidget(this);
    continueWorkingLayout->addWidget(m_continueWorkingLabel);
    continueWorkingLayout->addWidget(m_recentProjects);
    connect(m_recentProjects, SIGNAL(clicked(const QModelIndex)),
            this, SLOT(emitProjectSelected(const QModelIndex)));
    
    QVBoxLayout *createNewLayout = new QVBoxLayout;
    m_createNewLabel = new QLabel(i18n("Create new..."), this);
    m_contentTypes = new QComboBox(this);
    m_contentTypes->addItem("");
    m_contentTypes->addItem(i18n("Plasmoid"));
    m_contentTypes->addItem(i18n("Data engine"));
    m_contentTypes->addItem(i18n("Theme"));
    m_newProjectWizardButton = new KPushButton(i18n("Create!"), this);
    createNewLayout->addWidget(m_createNewLabel);
    createNewLayout->addWidget(m_contentTypes);
    createNewLayout->addWidget(m_newProjectWizardButton);
    connect(m_newProjectWizardButton, SIGNAL(clicked()), this, SLOT(launchNewProjectWizard()));
    
    m_layout->addItem(continueWorkingLayout);
    m_layout->addItem(createNewLayout);
    
    setLayout(m_layout);
}

void StartPage::launchNewProjectWizard()
{
    Ui::Wizard w;
    QWizard d;
    w.setupUi(&d);
    d.exec();
}

void StartPage::populateRecentProjects()
{
    QList<KUrl> recentFiles = m_parent->recentProjects();
    
    for (int i = 0; i < recentFiles.size(); i++) {
        const KUrl u = recentFiles.at(i);
        QListWidgetItem *item = new QListWidgetItem(u.fileName());
        item->setData(FullPathRole, u);
        m_recentProjects->addItem(item);
    }
}

void StartPage::emitProjectSelected(const QModelIndex &index)
{
    QAbstractItemModel *m = m_recentProjects->model();
    KUrl url = m->data(index, FullPathRole).value<KUrl>();
    kDebug() << "Loading project file:" << m->data(index, FullPathRole);
    emit projectSelected(url);
}
