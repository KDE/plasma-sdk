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

#ifndef HUNKSCURSOR_H
#define HUNKSCURSOR_H

#include "../InterviewCursor.h"

class ChangeSet;

class HunksCursor : public InterviewCursor
{
public:
    HunksCursor(ChangeSet &changeSet);
    virtual ~HunksCursor();

    /// returns the new index
    virtual int forward(Scope scope = ItemScope, bool skipAnswered = true);
    /// returns the new index
    virtual int back(Scope scope = ItemScope);
    virtual void setResponse(bool response, Scope scope = ItemScope);
    virtual QString allowedOptions() const;

    virtual int count();
    virtual void forceCount();
    virtual QString currentText() const;
    virtual QString helpMessage() const;
    virtual bool isValid() const;

private:
    int currentSubHunkCount();

    ChangeSet &m_changeSet;
    int m_totalHunks;

protected: // unit test.
    int currentIndex() const;
    int m_fileIndex;
    int m_hunkIndex;
    int m_subHunkIndex;
};

#endif
