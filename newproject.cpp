/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QByteArray>

#include <QFile>
#include <KDebug>

#include "newproject.h"
#include "ui_newproject.h"
 
NewProjectWizard::NewProjectWizard()
{
    w = new Ui::Wizard;
    w->setupUi(this);
}

void NewProjectWizard::setProjectType(StartPage::ProjectType type)
{
    m_type = type;
}

KUrl NewProjectWizard::projectFile()
{
    return m_metadataUrl;
}

void NewProjectWizard::accept()
{
    QByteArray m; // metadata.desktop
    QByteArray c; // code file
    
    QString projectName = w->projectName->text();
    //TODO: remove all "weird" characters from safeProjectName. we should have only a-z, and screw the rest.
    QString safeProjectName = projectName;
    QString author = "";
    
    
    m += "[Desktop Entry]\n"
         "Name=" + projectName + "\n"
         "Comment=\n"
         "Icon=\n"
         "Type=Service\n";
         
    if (!m_type == StartPage::Theme) { // so it's a javascript thingie
        m += "X-Plasma-API=javascript\n"
             "X-Plasma-MainScript=main.js\n";
             "X-Plasma-DefaultSize=200,100\n";
    }

    m += "X-KDE-ServiceTypes=Plasma/";
    if (m_type == StartPage::Plasmoid) { //TODO add the rest of the cases
        m += "Applet";
    } else if (m_type == StartPage::DataEngine) {
        m += "DataEngine";
    } else if (m_type == StartPage::Theme) {
        m += "Theme";
    }
    m += "\n";
        
    m += "X-KDE-PluginInfo-Author=" + author + "\n"
         "X-KDE-PluginInfo-Email=\n"
         "X-KDE-PluginInfo-Name=" + safeProjectName + "\n"
         "X-KDE-PluginInfo-Version=0.1\n"
         "X-KDE-PluginInfo-Website=\n"
         "X-KDE-PluginInfo-Category=\n"
         "X-KDE-PluginInfo-Depends=\n"
         "X-KDE-PluginInfo-License=GPL\n"
         "X-KDE-PluginInfo-EnabledByDefault=true";
    
    m_metadataUrl = w->outputDir->url().path() + "/metadata.desktop";
         
    QFile metadataFile(m_metadataUrl.path());
    kDebug() << "Writing metadata.desktop to: " << m_metadataUrl;
    
    if (!metadataFile.open(QFile::WriteOnly | QFile::Text)) {
//         TODO: Handle errors
    }
    
    metadataFile.write(m);
    
    QWizard::accept();
}

