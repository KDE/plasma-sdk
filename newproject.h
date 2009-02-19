/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/
 
#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H
 
#include <QWizard>
#include "startpage.h" // for the enum.

class KUrl;

namespace Ui {
    class Wizard;
}

class NewProjectWizard : public QWizard
{
    public:
        NewProjectWizard();
        
        KUrl projectFile();
        void setProjectType(StartPage::ProjectType);

        void accept();

    private:
        Ui::Wizard *w;
        StartPage::ProjectType m_type;
        KUrl m_metadataUrl;
};
 
#endif // NEWPROJECT_H