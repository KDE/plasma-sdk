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
#include <QLabel>

#include <KPushButton>
#include <KLineEdit>
#include <KLocalizedString>
#include <KUrl>

#include "docbrowser.h"

DocBrowser::DocBrowser(QWidget *parent)
    :QWidget(parent)
{
    m_view = new QWebView(this);
    connect(m_view, SIGNAL(loadFinished(bool)), this, SLOT(focusSearchField()));

    QHBoxLayout *topbar = new QHBoxLayout();
    QHBoxLayout *btmbar = new QHBoxLayout();

    KPushButton *linkButton = new KPushButton(KIcon("favorites"), i18n("Tutorials"), this);
    connect(linkButton, SIGNAL(clicked()), this, SLOT(showTutorial()));
    topbar->addWidget(linkButton);

    linkButton = new KPushButton(KIcon("favorites"), i18n("API Reference"), this);
    connect(linkButton, SIGNAL(clicked()), this, SLOT(showApi()));
    topbar->addWidget(linkButton);

    searchLabel = new QLabel(i18n("Find : "));
    btmbar->addWidget(searchLabel);

    // TODO: should probably respond to the common 'Ctrl-F' by
    //       highlight-focusing the search field
    searchField = new KLineEdit(this);
    connect(searchField, SIGNAL(textChanged(const QString&)), this, SLOT(findText(const QString&)));
    connect(searchField, SIGNAL(returnPressed()), this, SLOT(findNext()));
    btmbar->addWidget(searchField);

    KPushButton *searchButton = new KPushButton(i18n("Next"), this);
    connect(searchButton, SIGNAL(clicked()), this, SLOT(findNext()));
    btmbar->addWidget(searchButton);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(topbar);
    layout->addWidget(m_view);
    layout->addLayout(btmbar);
    setLayout(layout);
    showTutorial();
}

void DocBrowser::findText(const QString& toFind)
{
    if (!m_view->findText(toFind, 
              QWebPage::FindWrapsAroundDocument))
        searchLabel->setText(i18n("(Text not found!)"));
    else
        searchLabel->setText(i18n("Find : "));
}

void DocBrowser::findNext()
{
    m_view->findText(searchField->text(), QWebPage::FindWrapsAroundDocument);
}

KUrl DocBrowser::currentPage() const
{
    return KUrl(m_view->url());
}

void DocBrowser::load(KUrl page)
{
    m_view->load(page);
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

void DocBrowser::focusSearchField()
{
    searchField->setFocus(Qt::OtherFocusReason);
    searchField->selectAll();
}