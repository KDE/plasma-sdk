/*
 * This file is part of the vng project
 * Copyright (C) 2008 Thomas Zander <tzander@trolltech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Interview.h"
#include "InterviewCursor.h"
#include "Logger.h"

#include <QDebug>
#include <QTextStream>

#ifndef Q_OS_WIN
#include <termios.h>
#endif

Interview::Interview(InterviewCursor &cursor, const QString &command)
    : m_cursor(cursor),
    m_command(command),
    m_usePager(true)
{
}

bool Interview::start()
{
    if (! m_usePager)
        Logger::stopPager();
    const QString prompt = "Shall I "+ m_command +" this change?";
    int currentIndex = 1;
    if (m_cursor.count() == 0)
        return true;
    QTextStream out(stdout);
    while(true) {
        if (m_usePager)
            Logger::startPager();
        QTextStream &patchOut = Logger::standardOut();
        patchOut << m_cursor.currentText();
        patchOut.flush();
        Logger::stopPager();

        bool askAgain = false;
        do {
            int count = m_cursor.count();
            QString allowed = m_cursor.allowedOptions();
            QString question = prompt +" ("+ QString::number(currentIndex) +"/"+
                (count == -1 ? "?" : QString::number(count)) +")  ["+ allowed + "], or ? for help: ";

            QChar reply = askSingleChar(question);
            if (reply.unicode() != '?' && reply.unicode() != 'h' && allowed.indexOf(reply) < 0)
                reply = 0x00DF; // to generate an 'invalid response' message.
            askAgain = false;
            switch (reply.toAscii()) {
                case 'j': currentIndex = m_cursor.forward(InterviewCursor::ItemScope, false); break;
                case 'k':
                    if (currentIndex == 1) // at start.
                        askAgain = true;
                    else
                        currentIndex = m_cursor.back();
                    break;
                case 'y': m_cursor.setResponse(true);
                          currentIndex = m_cursor.forward(); break;
                case 'n': m_cursor.setResponse(false);
                          currentIndex = m_cursor.forward(); break;
                case 'f': m_cursor.setResponse(true, InterviewCursor::BlockScope);
                          currentIndex = m_cursor.forward(InterviewCursor::BlockScope); break;
                case 's': m_cursor.setResponse(false, InterviewCursor::BlockScope);
                          currentIndex = m_cursor.forward(InterviewCursor::BlockScope); break;
                case 'a': m_cursor.setResponse(true, InterviewCursor::FullRange);
                          return true;
                case 'd': return true;
                case 'q': return false;
                case 'h': // fall through
                case '?':
                    out << m_cursor.helpMessage();
                    out.flush();
                    askAgain = true;
                    break;
                case 'c':
                    m_cursor.forceCount();
                    askAgain = true;
                    break;
                default:
                    out << "Invalid response, try again please." << endl;
                    askAgain = true;
                    break;
            }
        } while(askAgain);
        if (! m_cursor.isValid())
            return true;
    }
}

QChar Interview::askSingleChar(const QString &prompt, const QString &allowedCharacters)
{
    QTextStream out(stdout);
    QTextStream in(stdin);
#ifndef Q_OS_WIN
    struct termios oldT, newT;
    tcgetattr(0, &oldT);
    newT = oldT;
    newT.c_lflag &= ~ECHO; // echo off
    newT.c_lflag &= ~ICANON; //one char at a time
#endif

    out << prompt;
    out.flush();

#ifndef Q_OS_WIN
    tcsetattr(0, TCSANOW, &newT);
#endif
    QString answer;
    do {
        answer = in.read(1);
        if (answer[0] == 93) // aka esc, used for things like 'arrow up'
            in.read(1);     // after an esc there always is a second character.
    } while(answer.length() == 0 || (!allowedCharacters.isEmpty() && !allowedCharacters.contains(answer[0])));
#ifndef Q_OS_WIN
    tcsetattr(0, TCSANOW, &oldT);
    out << answer << endl;
#endif
    return answer[0];
}

QString Interview::ask(const QString &prompt)
{
    QTextStream out(stdout);
    QTextStream in(stdin);
    out << prompt;
    out.flush();
    QString answer = in.readLine();
    return answer;
}

