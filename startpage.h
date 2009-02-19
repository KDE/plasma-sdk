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

class QLabel;
class QComboBox;
class QListView;
class QHBoxLayout;

class StartPage : public QWidget
{
    public:
        StartPage(QWidget *parent);
        
    private:
        void createWidgets();
        
        QLabel *m_createNewLabel, *m_openExistingLabel, *m_continueWorkingLabel;
        QComboBox *m_contentTypes;
        QListView *m_recentProjects;
        QHBoxLayout *m_layout;
};

#endif // STARTPAGE_H
