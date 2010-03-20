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

#include <KLineEdit>

#include <QListWidget>
#include <QVBoxLayout>
#include <iostream>
#include "runnerpreviewer.h"

RunnerPreviewer::RunnerPreviewer(const QString & title, QWidget * parent, Qt::WindowFlags flags)
        : Previewer(title, parent, flags)
{
    m_edit = new KLineEdit(this);
    connect(m_edit, SIGNAL(textEdited()), this, SIGNAL(doQuery()));
    m_matches = new QListWidget(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_edit);
    layout->addWidget(m_matches);
    QWidget *w = new QWidget(this);
    w->setLayout(layout);
    setWidget(w);
}

void RunnerPreviewer::showPreview(const QString &packagePath)
{
    Plasma::RunnerManager rmanager;
    m_runner = rmanager.runner("places");
}

void RunnerPreviewer::refreshPreview() {
    //emit refreshView();
}

void RunnerPreviewer::doQuery() {
}