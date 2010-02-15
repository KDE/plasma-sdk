/*
  Copyright (c) 2009 Lim Yuen Hoe <yuenhoe@hotmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QDialog>

class QListWidget;
class QPushButton;
class QListWidgetItem;

namespace Ui
{
class ProjectManager;
}

class ProjectManager : public QDialog
{
    Q_OBJECT;
public:
    ProjectManager(QWidget* parent);
    void addProject(QListWidgetItem *item);
    void clearProjects();

//TODO: Should probably have all project-management related
//      code eg. import/export, import-all/export-all, delete
//      here. Move export/import code over from Publisher?

signals:
    void projectSelected(const QString &name, const QString &type);

private slots:
   void emitProjectSelected();

private:
//TODO: Implement:
//      search/filterbar,
//      project deletion,
//      project multi-export,
//      project multi-import.
    QListWidget *projectList;
    QPushButton *loadButton;
};

#endif // PROJECTMANAGER_H