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

class QModelIndex;

namespace Ui {
    class MainWindowClass;
};

class QDockWidget;
class QStringList;

class KListWidget;

class EditPage;
class PackageModel;
class StartPage;
class Sidebar;

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

        void changeTab(int tab);
        void loadProject(const QString &name, const QString &type);

    private:
        enum WorkflowTabs { StartPageTab = 0,
                            EditTab,
                            PublishTab,
                            DocsTab,
                            PreviewTab };

        void createMenus();
        void showEditor();
        void hideEditor();
        void createDockWidgets();

        StartPage *m_startPage;
        QDockWidget *m_workflow;
        Sidebar *m_sidebar;
        QDockWidget *m_previewerWidget;
        Previewer *m_previewer;

        KLibFactory *m_factory;
        EditPage *m_editPage;
        KParts::ReadWritePart *m_part;
        PackageModel *m_model;
        int m_oldTab;
};

#endif // MAINWINDOW_H
