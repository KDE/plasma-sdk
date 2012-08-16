/*
 * Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include "konsolepreviewer.h"

#include <QVBoxLayout>
#include <QFile>
#include <KAction>
#include <KFileDialog>
#include <KMessageBox>
#include <KStandardAction>
#include <KTextEdit>
#include <KToolBar>
#include <KUrl>
#include <KUser>
#include <KIO/CopyJob>

KonsolePreviewer::KonsolePreviewer(const QString & title, QWidget *parent)
        : QDockWidget(title, parent),
        m_textEdit(0)
{
    QVBoxLayout *layout = new QVBoxLayout();
    KToolBar *toolBar = new KToolBar(this, true, true);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    KAction *clear = KStandardAction::clear(this, SLOT(clearOutput()), this);
    KAction *save = KStandardAction::save(this, SLOT(saveOutput()), this);
    toolBar->addAction(clear);
    toolBar->addAction(save);

    m_textEdit = new KTextEdit();
    //don't let the user modify the output
    m_textEdit->setReadOnly(true);
    //make the background white
    QPalette p = m_textEdit->palette();
    p.setColor(QPalette::Base, Qt::white);
    m_textEdit->setPalette(p);

    layout->addWidget(toolBar);
    layout->addWidget(m_textEdit);

    QWidget *tmpWidget = new QWidget(this);
    tmpWidget->setLayout(layout);
    setWidget(tmpWidget);
}

void KonsolePreviewer::clearOutput()
{
    m_textEdit->clear();
}

void KonsolePreviewer::clearTmpFile()
{
    QFile f("/var/tmp/plasmatepreviewerlog.txt");
    f.resize(0);
    f.close();
}


void KonsolePreviewer::populateKonsole()
{
    m_textEdit->setText(takeOutput());


    //move the scrollbar automatically,
    //in the end of the output
    QTextCursor c =  m_textEdit->textCursor();
    c.movePosition(QTextCursor::End);
    m_textEdit->setTextCursor(c);
}

QString KonsolePreviewer::takeOutput() const
{
    QFile file("/var/tmp/plasmatepreviewerlog.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QString content = file.readAll();
        file.close();
        return content;
    }

    return QString();
}


void KonsolePreviewer::saveOutput()
{
    KUser user;
    KUrl destination = KFileDialog::getSaveUrl(KUrl(user.homeDir()), ".*", this);

    if (destination.isEmpty()) {
        return;
    }

    KIO::copy(KUrl("/var/tmp/plasmatepreviewerlog.txt"), destination, KIO::Overwrite);
}

