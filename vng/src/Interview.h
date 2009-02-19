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

#ifndef INTERVIEW_H
#define INTERVIEW_H

#include <QString>

class InterviewCursor;

class Interview
{
public:
    Interview(InterviewCursor &cursor, const QString &command);

    /// returns false if the user cancelled the interaction.
    bool start();

    static QChar askSingleChar(const QString &prompt, const QString &allowedCharacters = QString());
    static QString ask(const QString &prompt);

    void setUsePager(bool on) { m_usePager = on; }
    bool usePager() const { return m_usePager; }

private:
    InterviewCursor &m_cursor;
    QString m_command;
    bool m_usePager;
};

#endif

