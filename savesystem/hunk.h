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

#ifndef HUNK_H
#define HUNK_H

#include "vng.h"

#include <QByteArray>

class Hunk
{
public:
    Hunk();
    Hunk(const Hunk &other);
    ~Hunk();

    void addLine(const QByteArray &line);
    bool isEmpty() const;

    int lineNumber() const;
    int lineNumber(int subHunk) const;
    int subHunkCount() const;
    int linesAdded() const;
    int linesRemoved() const;

    Vng::Acceptance acceptance() const;
    Vng::Acceptance acceptance(int subHunk) const;
    void setAcceptance(Vng::Acceptance accepted);
    void setAcceptance(int subHunk, Vng::Acceptance accepted);
    QByteArray header() const;

    QByteArray subHunk(int index) const;
    QByteArray patch() const;
    QByteArray acceptedPatch() const;
    QByteArray rejectedPatch() const;

    Hunk &operator=(const Hunk &other);
private:
    class Private;
    Private *d;
};

#endif
