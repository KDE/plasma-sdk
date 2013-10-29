/*
 *   Copyright 2012 Giorgos Tsiapaliokas <terietor@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KONSOLEPREVIEWER_H
#define KONSOLEPREVIEWER_H

#include <QDockWidget>
#include <QEvent>
#include <QWeakPointer>

class QTextEdit;

class KonsolePreviewer : public QDockWidget {

    Q_OBJECT

public:
    KonsolePreviewer(const QString & title, QWidget *parent = 0);
    ~KonsolePreviewer();

    enum EventType {
        MessageEventType = QEvent::User + 1
    };

    static void customMessageHandler(QtMsgType type, const char *msg);
    static QWeakPointer<KonsolePreviewer> msgHandler;

    void debugMessage(QtMsgType type, const char *msg);

protected:
    virtual void customEvent(QEvent *event);
public Q_SLOTS:
    void clearOutput();
    void saveOutput();

private:
    QTextEdit *m_textEdit;
};

class KonsolePreviewerDebugEvent: public QEvent
{
public:
    KonsolePreviewerDebugEvent(const QString& debugOutput);
    ~KonsolePreviewerDebugEvent();
    QString debugOutput;
};
#endif // KONSOLEPREVIEWER_H
