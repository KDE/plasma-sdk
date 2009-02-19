/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QLabel>
#include <QComboBox>
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <KLocalizedString>

#include "startpage.h"

StartPage::StartPage(QWidget *parent = 0)
    : QWidget(parent)
{
    createWidgets();
}

void StartPage::createWidgets() // Make this a QGV? Use Plasma::Theme?
{
    m_layout = new QHBoxLayout(this);
    
    QVBoxLayout *continueWorkingLayout = new QVBoxLayout;
    m_continueWorkingLabel = new QLabel(i18n("Continue working on.."), this);
    m_recentProjects = new QListView(this);
    continueWorkingLayout->addWidget(m_continueWorkingLabel);
    continueWorkingLayout->addWidget(m_recentProjects);
    
    QVBoxLayout *createNewLayout = new QVBoxLayout;
    m_createNewLabel = new QLabel(i18n("Create new..."), this);
    m_contentTypes = new QComboBox(this);
    createNewLayout->addWidget(m_createNewLabel);
    createNewLayout->addWidget(m_contentTypes);
    
    m_layout->addItem(continueWorkingLayout);
    m_layout->addItem(createNewLayout);
    
    setLayout(m_layout);
}



