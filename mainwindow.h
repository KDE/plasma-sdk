/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KParts/MainWindow>
#include <KLibLoader>
#include <kservice.h>

class QModelIndex;

namespace Ui
{
}

class QDockWidget;
class QStringList;


class EditPage;
class PackageModel;
class StartPage;
class Sidebar;
class TimeLine;

// our own previewer
class Previewer;

class MainWindow : public KParts::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QStringList recentProjects();

public Q_SLOTS:
    void quit();

    void changeTab(const QModelIndex &);
    void loadProject(const QString &name, const QString &type);

    void loadRequiredEditor(const KService::List offers, KUrl target);
signals:
    void newSavePointClicked();

private:
    enum WorkflowTabs { StartPageTab = 0,
                        EditTab,
                        SavePoint,
                        PublishTab,
                        DocsTab,
                        PreviewTab
                      };

    void createMenus();
    void createDockWidgets();

    StartPage *m_startPage;
    QDockWidget *m_workflow;
    Sidebar *m_sidebar;
    QDockWidget *m_dockTimeLine;
    TimeLine    *m_timeLine;
    QDockWidget *m_previewerWidget;
    Previewer *m_previewer;

    EditPage *m_editPage;
    PackageModel *m_model;
    int m_oldTab;
    bool docksCreated;

    KParts::ReadOnlyPart *m_part;
};

#endif // MAINWINDOW_H
