/*
  Copyright (c) 2009 Lim Yuen Hoe <yuenhoe@hotmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QWebView>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <KPushButton>
#include <KLocalizedString>

#include "docbrowser.h"

DocBrowser::DocBrowser(QWidget *parent)
    :QWidget(parent)
{
    m_view = new QWebView(this);
    QHBoxLayout *bar = new QHBoxLayout();

    KPushButton *linkButton = new KPushButton(KIcon("favorites"), i18n("Tutorials"), this);
    connect(linkButton, SIGNAL(clicked()), this, SLOT(showTutorial()));
    bar->addWidget(linkButton);

    linkButton = new KPushButton(KIcon("favorites"), i18n("API Reference"), this);
    connect(linkButton, SIGNAL(clicked()), this, SLOT(showApi()));
    bar->addWidget(linkButton);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(bar);
    layout->addWidget(m_view);
    setLayout(layout);
    showTutorial();
}

void DocBrowser::showApi()
{
    m_view->setHtml("<center><h3>" + 
                      i18n("Loading API reference...") + 
                      "</h3></center>");
    m_view->load(QUrl("http://api.kde.org/4.x-api/kdelibs-apidocs/plasma/html/annotated.html"));
}

void DocBrowser::showTutorial()
{
    m_view->setHtml("<center><h3>" + 
                      i18n("Loading tutorials...") + 
                      "</h3></center>");
    m_view->load(QUrl("http://techbase.kde.org/Development/Tutorials/Plasma"));
}