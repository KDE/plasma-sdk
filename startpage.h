/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <QWidget>
#include <KUrl>

class QLabel;
class QComboBox;
class QListWidget;
class QHBoxLayout;
class QModelIndex;
class MainWindow;

class StartPage : public QWidget
{
    Q_OBJECT
    
    enum Roles { 
        FullPathRole = Qt::UserRole + 1
    };
    
    public:
        StartPage(MainWindow *parent);
    
    signals:
        void projectSelected(KUrl url);
        
    public Q_SLOTS:
        void emitProjectSelected(const QModelIndex &);
        
    private:
        void createWidgets();
        void populateRecentProjects();
        
        QLabel *m_createNewLabel, *m_openExistingLabel, *m_continueWorkingLabel;
        QComboBox *m_contentTypes;
        QListWidget *m_recentProjects;
        QHBoxLayout *m_layout;
        MainWindow *m_parent;
};

#endif // STARTPAGE_H
