/*
 * Copyright 2012 Giorgos Tsiapaliokas <terietor@gmail.com>
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
#include <QCoreApplication>
#include <QMutexLocker>

#include <KAction>
#include <KFileDialog>
#include <KMessageBox>
#include <KStandardAction>
#include <KStandardDirs>
#include <KTextEdit>
#include <KToolBar>
#include <KUrl>
#include <KUser>

#include <iostream>

QWeakPointer<KonsolePreviewer> KonsolePreviewer::msgHandler;

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

    m_textEdit = new KTextEdit(this);
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

    KonsolePreviewer::msgHandler = this;
}

KonsolePreviewer::~KonsolePreviewer()
{
}

void KonsolePreviewer::clearOutput()
{
    m_textEdit->clear();
}

void KonsolePreviewer::saveOutput()
{
    KUser user;
    KUrl destination = KFileDialog::getSaveUrl(KUrl(user.homeDir()), ".*", this);

    if (destination.isEmpty()) {
        return;
    }

    m_textEdit->toPlainText();
    QFile f(destination.pathOrUrl());
    f.open(QIODevice::ReadWrite);
    f.write(m_textEdit->toPlainText().toAscii());
}

void KonsolePreviewer::customMessageHandler(QtMsgType type, const char* msg)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    if (KonsolePreviewer::msgHandler) {
        KonsolePreviewer::msgHandler.data()->debugMessage(type, msg);
    }
}

void KonsolePreviewer::debugMessage(QtMsgType type, const char *msg)
{
    QString txt;

    if (QString(msg).startsWith("plasmate") || //don't include the plasmate specific output
        QString(msg).startsWith("Object::") || // don't include QObject warnings
        QString(msg).startsWith("QGraphicsScene::") || //don't include QGraphicsScene warnings
        QString(msg).startsWith(" X Error")) //don't include silly X errors
    {
       std::cout << msg << std::endl;
    } else {
        switch (type) {
            case QtDebugMsg:
                txt = QString("Debug: %1").arg(msg);
                break;
            case QtWarningMsg:
                txt = QString("Warning: %1").arg(msg);
                break;
            case QtCriticalMsg:
                txt = QString("Critical: %1").arg(msg);
                break;
            case QtFatalMsg:
                txt = QString("Fatal: %1").arg(msg);
                abort();
        }
    }
    if (KonsolePreviewer::msgHandler) {
        KonsolePreviewerDebugEvent *e = new KonsolePreviewerDebugEvent(txt);
        QCoreApplication::postEvent(this, e);
    }
}

void KonsolePreviewer::customEvent(QEvent *event)
{
    if (static_cast<KonsolePreviewer::EventType>(event->type()) == KonsolePreviewer::MessageEventType) {
        if (!m_textEdit) {
            return;
        }
        m_textEdit->append(dynamic_cast<KonsolePreviewerDebugEvent *>(event)->debugOutput);
    }
}

KonsolePreviewerDebugEvent::KonsolePreviewerDebugEvent(const QString& debugOutput)
        :QEvent(static_cast<QEvent::Type>(KonsolePreviewer::MessageEventType))
{
    this->debugOutput = debugOutput;
}

KonsolePreviewerDebugEvent::~KonsolePreviewerDebugEvent()
{
}
