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

#include "startpage.h"
#include "mainwindow.h"

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
    createNewLayout->addWidget(m_createNewLabel);
    createNewLayout->addWidget(m_contentTypes);
    
    m_layout->addItem(continueWorkingLayout);
    m_layout->addItem(createNewLayout);
    
    setLayout(m_layout);
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
