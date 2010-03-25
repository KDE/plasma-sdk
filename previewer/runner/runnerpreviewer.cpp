/*
 * Copyright 2010 Lim Yuen Hoe <yuenhoe@hotmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <Plasma/RunnerManager>
#include <Plasma/AbstractRunner>
#include <Plasma/RunnerContext>
#include <Plasma/QueryMatch>

#include <KLineEdit>

#include <QListWidget>
#include <QVBoxLayout>

#include "runnerpreviewer.h"

RunnerPreviewer::RunnerPreviewer(const QString & title, QWidget * parent, Qt::WindowFlags flags)
        : Previewer(title, parent, flags), m_runner(0)
{
    m_edit = new KLineEdit(this);
    connect(m_edit, SIGNAL(textEdited(const QString&)), this, SLOT(doQuery()));
    m_matches = new QListWidget(this);
    connect(m_matches, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(executeMatch(QListWidgetItem*)));
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_edit);
    layout->addWidget(m_matches);
    QWidget *w = new QWidget(this);
    w->setLayout(layout);
    setWidget(w);
}

RunnerPreviewer::~RunnerPreviewer()
{
    if (m_runner) {
        m_runner->matchSessionComplete();
        delete m_runner;
    }
    delete m_matches;
    delete m_edit;
}

void RunnerPreviewer::showPreview(const QString &packagePath)
{
    delete m_runner;
    m_runner = new Plasma::RunnerManager(this);
    connect(m_runner,SIGNAL(matchesChanged(const QList< Plasma::QueryMatch > &)),this,SLOT(showMatches()));
    QStringList runners;
    runners << "places";
    m_runner->setAllowedRunners(runners);
}

void RunnerPreviewer::refreshPreview()
{
    //emit refreshView();
}

void RunnerPreviewer::showMatches()
{
    m_matches->clear();
    foreach (Plasma::QueryMatch match, m_runner->matches()) {
        m_matches->addItem(match.text());
    }
}

void RunnerPreviewer::executeMatch(QListWidgetItem* item)
{
    m_matches->setCurrentItem(item);
    m_runner->run(m_runner->matches().at(m_matches->currentRow()));
}

void RunnerPreviewer::doQuery()
{
    if (m_runner) {
        m_runner->launchQuery(m_edit->text());
    }
}