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

class KUrl;

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

    static bool exportPackage(const KUrl &toExport, const KUrl &targetFile);
    static bool importPackage(const KUrl &toImport, const KUrl &targetLocation);
    static bool deleteProject(const KUrl &projectLocation);

signals:
    void projectSelected(const QString &name, const QString &type);
    void requestRefresh();

private slots:
   void emitProjectSelected();
   void confirmDeletion();

private:
//TODO: Implement:
//      search/filterbar,
//      project multi-export,
//      project multi-import.
    QListWidget *projectList;
    QPushButton *loadButton;
    QPushButton *deleteButton;
    static void removeDirectory(const QString&);
};

#endif // PROJECTMANAGER_H
