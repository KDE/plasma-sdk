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
#include "Commit.h"

class CommitsMatcher;

class CommitsCursor : public InterviewCursor
{
public:
    enum SelectionType {
        SelectOnePatch, ///< gives user choice of 'yes'/'no'
        SelectRange     ///< gives user navigating abilities and ability to say yes to multiple patches
    };

    CommitsCursor(SelectionType selectionType);
    virtual ~CommitsCursor();

    virtual int forward(Scope scope = ItemScope, bool skipAnswered = true);
    virtual int back(Scope scope = ItemScope);
    virtual void setResponse(bool response, Scope scope = ItemScope);

    virtual int count();
    virtual void forceCount();
    virtual QString currentText() const;
    virtual QString helpMessage() const;
    virtual bool isValid() const;
    virtual QString allowedOptions() const;

    Commit head();

    /// returns the index into the list of patches that is the oldest patch the user adjusted.
    int oldestCommitAltered() const { return m_oldestIndex; }

    /**
     * make the cursor use the matcher to match only commits that match the command line arguments.
     * @param use if true the next call to forward() or back() will use the CommitsMatcher, otherwise it won't.
     * The default is to use no matcher.
     */
    void setUseMatcher(bool use);

private:
    int m_currentIndex, m_oldestIndex;
    Commit m_head;
    Commit m_current;
    Commit m_firstInBranch;
    bool m_done;
    CommitsMatcher *m_matcher;
    SelectionType m_selectionType;
};

#endif
