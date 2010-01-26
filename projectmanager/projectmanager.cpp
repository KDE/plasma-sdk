/*
  Copyright (c) 2009 Lim Yuen Hoe <yuenhoe@hotmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>

#include <KMessageBox>
#include <KLocalizedString>

#include "projectmanager.h"
#include "startpage.h"

ProjectManager::ProjectManager(QWidget* parent) : QDialog(parent)
{
    projectList = new QListWidget();
    connect(projectList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(emitProjectSelected()));

    loadButton = new QPushButton("Load Project");
    connect(loadButton, SIGNAL(clicked()), this, SLOT(emitProjectSelected()));

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(projectList);
    lay->addWidget(loadButton);
    setLayout(lay);
}

void ProjectManager::addProject(QListWidgetItem *item)
{
    projectList->addItem(item);
}

void ProjectManager::clearProjects()
{
    projectList->clear();
}

void ProjectManager::emitProjectSelected()
{
    QList<QListWidgetItem*> l = projectList->selectedItems();
    if (l.size() != 1) {
      KMessageBox::information(this, i18n("Please select exactly one project to load."));
      return;
    }
    QString url = l[0]->data(StartPage::FullPathRole).value<QString>();

    emit projectSelected(url, QString());
    done(QDialog::Accepted);
}
