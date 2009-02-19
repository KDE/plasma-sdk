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

#include "HunksCursor.h"
#include "ChangeSet.h"

#include <QDebug>

HunksCursor::HunksCursor(ChangeSet &changeSet)
    : m_changeSet(changeSet),
    m_totalHunks(-1),
    m_fileIndex(0),
    m_hunkIndex(0),
    m_subHunkIndex(0)
{
    changeSet.waitForFinishFirstFile();
    if (changeSet.count()) {
        // make sure the first hunk is an actual change
        File file = changeSet.file(0);
        if (file.fileName() == file.oldFileName())
            forward(ItemScope, true);
    }
    else {
        m_totalHunks = 0;
    }
}

HunksCursor::~HunksCursor()
{
}

int HunksCursor::forward(Scope scope, bool skipAnswered)
{
    if (m_changeSet.count() == 0 || m_fileIndex >= m_changeSet.count())
        return currentIndex();

    struct Last {
        Last(const ChangeSet &changeSet)
            : hunk(0), subHunk(0)
            {
            file = changeSet.count() - 1;
            if (! changeSet.hasAllHunks())
                return;
            File f = changeSet.file(file);
            hunk = f.count() - 1;
            if (hunk >= 0) {
                Hunk h = f.hunks()[hunk];
                hunk += 2;
                subHunk = h.subHunkCount() - 1;
            }
        }
        int file, hunk, subHunk;
    };
    Last last(m_changeSet);
    if (m_fileIndex > last.file) {
        m_fileIndex = last.file +1;
        m_hunkIndex = 0;
        m_subHunkIndex = 0;
        return currentIndex();  // already at end.
    }

    switch(scope) {
    case ItemScope: {
        if (++m_subHunkIndex < currentSubHunkCount())
            break;

        --m_subHunkIndex;
        File file = m_changeSet.file(m_fileIndex);
        if (m_hunkIndex + 1 - 2 >= file.count() && !(m_hunkIndex == 1 && file.isBinary()))
            return forward(BlockScope, skipAnswered);
        else {
            m_hunkIndex++;
            m_subHunkIndex = 0;
        }
        break;
    }
    case BlockScope:
        m_fileIndex++;
        m_hunkIndex = 0;
        m_subHunkIndex = 0;
        if (m_fileIndex > last.file)
            skipAnswered = false; // there is nothing left;
        break;
    case FullRange:
        m_fileIndex = last.file + 1;
        m_hunkIndex = 0;
        m_subHunkIndex = 0;
        break;
    }

    if (! isValid())
        return currentIndex();

    File file = m_changeSet.file(m_fileIndex);
    const bool renamed = file.fileName() != file.oldFileName();
    const bool protectionChanged = !renamed && file.protection() != file.oldProtection();
    if (m_hunkIndex == 0 && !renamed)
        m_hunkIndex++;
    if (m_hunkIndex == 1 && !protectionChanged)
        return forward(ItemScope, skipAnswered);

    if (skipAnswered) {
        if ((m_hunkIndex == 0 && file.renameAcceptance() != Vng::Undecided)
                || (m_hunkIndex == 1 && file.protectionAcceptance() != Vng::Undecided))
            return forward(ItemScope, skipAnswered);

        if (m_hunkIndex == 2 && file.isBinary())
            ;
        else if (m_hunkIndex >= 2) { // a hunk;
            Hunk hunk = file.hunks()[m_hunkIndex - 2];
            Vng::Acceptance a = hunk.acceptance();
            if (a == Vng::Accepted || a == Vng::Rejected || hunk.acceptance(m_subHunkIndex) != Vng::Undecided) {
                if (hunk.subHunkCount() == m_subHunkIndex + 1)
                    return forward(BlockScope, skipAnswered);
                return forward(ItemScope, skipAnswered);
            }
        }
    }
    return currentIndex();
}

int HunksCursor::back(Scope scope)
{
    if (m_changeSet.count() == 0
        || (m_fileIndex == 0 && m_hunkIndex == 0 && m_subHunkIndex == 0))
        return currentIndex();
    switch(scope) {
    case ItemScope:
        if (--m_subHunkIndex >= 0)
            break;
        ++m_subHunkIndex;
        if (--m_hunkIndex < 1) {
            m_hunkIndex++;
            return back(BlockScope);
        }
        m_subHunkIndex = currentSubHunkCount() - 1;
        break;
    case BlockScope:
        if (--m_fileIndex < 0)
            m_fileIndex = 0;
        m_hunkIndex = m_changeSet.file(m_fileIndex).count() -1+2;
        Q_ASSERT(m_hunkIndex >= 0);
        m_subHunkIndex = currentSubHunkCount() - 1;
        break;
    case FullRange:
        m_hunkIndex = 0;
        m_fileIndex = 0;
        m_subHunkIndex = 0;
        break;
    }

    File file = m_changeSet.file(m_fileIndex);
    const bool renamed = file.fileName() != file.oldFileName();
    const bool protectionChanged = !renamed && file.protection() != file.oldProtection();
    if (m_hunkIndex == 1 && !protectionChanged)
        m_hunkIndex--;
    if (m_hunkIndex == 0 && !renamed)
        return back(BlockScope);

    return currentIndex();
}

void HunksCursor::setResponse(bool response, Scope scope)
{
    if (m_changeSet.count() == 0)
        return;
    const Vng::Acceptance accept = response ? Vng::Accepted : Vng::Rejected;

    if (scope == ItemScope) {
        File file = m_changeSet.file(m_fileIndex);
        if (m_hunkIndex == 0) {
            if ((!response && file.oldFileName().isEmpty())
                    || (response && file.fileName().isEmpty())) { // new file that is not added, removed file that we want added.
                setResponse(response, BlockScope);
                return;
            }
            file.setRenameAcceptance(accept);
        }
        else if (m_hunkIndex == 1)
            file.setProtectionAcceptance(accept);
        else if (m_hunkIndex == 2 && file.isBinary())
            file.setBinaryChangeAcceptance(accept);
        else {
            Hunk hunk = file.hunks()[m_hunkIndex - 2];
            hunk.setAcceptance(m_subHunkIndex, accept);
        }
        return;
    }

    int fileIndex = m_fileIndex;
    int hunkIndex = m_hunkIndex;

    bool firstSubHunk = m_subHunkIndex > 0;
    do {
        File file = m_changeSet.file(fileIndex);
        QList<Hunk> hunks = file.hunks();
        do {
            if (hunkIndex == 0)
                file.setRenameAcceptance(accept);
            else if (hunkIndex == 1)
                file.setProtectionAcceptance(accept);
            else if (firstSubHunk) { // if this is the first hunk, we have to accept only the appropriate subhunks.
                Hunk hunk = hunks[hunkIndex-2];
                for (int i = m_subHunkIndex; i < hunk.subHunkCount(); ++i)
                    hunk.setAcceptance(i, accept);
            }
            else
                hunks[hunkIndex - 2].setAcceptance(accept);
            hunkIndex++;
            firstSubHunk = false;
        } while(scope >= BlockScope && hunkIndex -2 < hunks.count());
        hunkIndex = 0;
        fileIndex++;
    } while(scope == FullRange && fileIndex < m_changeSet.count());
}

int HunksCursor::count()
{
    if (m_totalHunks == -1 && m_changeSet.hasAllHunks())
        forceCount();
    return m_totalHunks;
}

void HunksCursor::forceCount()
{
    if (m_totalHunks < 0) {
        int total = 0;
        for (int i=0; i < m_changeSet.count(); ++i) {
            File file = m_changeSet.file(i);
            if (file.fileName() != file.oldFileName()
                    || file.protection() != file.oldProtection()
                    || file.isBinary())
                total++;
            foreach(Hunk hunk, file.hunks())
                total += hunk.subHunkCount();
        }
        m_totalHunks = total;
    }
}

int HunksCursor::currentIndex() const
{
    int answer = 1;
    const int end = qMin(m_changeSet.count(), m_fileIndex+1);
    int i=0;
    while(i < end) {
        File file = m_changeSet.file(i);
        const bool renamed = file.fileName() != file.oldFileName();
        const bool protectionChanged = !renamed && file.protection() != file.oldProtection();
        QList<Hunk> hunks = file.hunks();
        ++i;
        if (i == end && m_fileIndex < m_changeSet.count()) {
            if (m_hunkIndex > 0 && renamed)
                answer++;
            if (m_hunkIndex > 1 && protectionChanged)
                answer++;
            hunks = hunks.mid(0, m_hunkIndex - 2);
        }
        else {
            if (renamed)
                answer++;
            if (protectionChanged)
                answer++;
        }
        QList<Hunk>::Iterator iter2 = hunks.begin();
        int index = 2;
        while(iter2 != hunks.end()) {
            if (isValid() && i == end && m_hunkIndex < index++)
                break;
            answer += (*iter2).subHunkCount();
            ++iter2;
        }
    }
    return answer + m_subHunkIndex;
}

QString HunksCursor::currentText() const
{
    QByteArray bytes;
    if (m_changeSet.count() <= m_fileIndex)
        return QString();
    File file = m_changeSet.file(m_fileIndex);

    QString output;
    QTextStream ts(&output);
    if (m_config)
        m_config->colorize(ts);
    if (m_hunkIndex == 0) { // file rename
        if (file.fileName().isEmpty())
            ts << "remove ";
        else if (file.oldFileName().isEmpty())
            ts << "add ";
        else
            ts << "move ";
    }
    else if (m_hunkIndex == 1) // protections change
        ts << "mode change ";
    else
        ts << "hunk ";
    if (m_config)
        m_config->normalColor(ts);
    if (m_hunkIndex == 0) {
        if (!file.oldFileName().isEmpty())
            ts << "`" << file.oldFileName() << "' ";
        if (! file.fileName().isEmpty())
            ts << "`" << file.fileName() << "'";
    }
    else if (m_hunkIndex == 1)
        ts << file.fileName() << "  " << file.oldProtection() << " => " << file.protection() << endl;

    if (m_hunkIndex < 2) {
        ts << endl;
        ts.flush();
        return output;
    }

    if (file.count() <= m_hunkIndex - 2) {
        if (file.isBinary())
            return "binary data\n";
        else
            return "no change\n";
    }
    Hunk hunk = file.hunks()[m_hunkIndex - 2];
    bytes.append(file.fileName());
    bytes.append(" ");
    bytes.append(QString::number(hunk.lineNumber(m_subHunkIndex)));
//bytes.append("[subhunk "+ QString::number(m_subHunkIndex + 1) +"/"+ QString::number(hunk.subHunkCount()) +"]");
    bytes.append("\n");

    QByteArray patch = hunk.subHunk(m_subHunkIndex);
    //m_config.normalColor(out);
    if (patch.contains((char) 0)) { // binary
        //m_config.colorize(out);
        bytes.append(QString("binary data\n"));
        //m_config.normalColor(out);
    }
    else
        bytes.append(patch);

    ts.flush();
    return output + QString::fromLocal8Bit(bytes);
}

QString HunksCursor::helpMessage() const
{
    return QString(
   "How to use revert...\n"
   "y: revert this patch\n"
   "n: don't revert it\n\n"

   "s: don't revert the rest of the changes to this file\n"
   "f: revert the rest of the changes to this file\n\n"

   "d: revert selected patches, skipping all the remaining patches\n"
   "a: revert all the remaining patches\n"
   "q: cancel revert\n\n"

   "j: skip to next patch\n"
   "k: back up to previous patch\n"
   "c: calculate number of patches\n"
   "h or ?: show this help\n");
}

int HunksCursor::currentSubHunkCount()
{
    File file = m_changeSet.file(m_fileIndex);
    if (m_hunkIndex < 2 || file.isBinary())
        return 1;
    Hunk hunk = file.hunks()[m_hunkIndex - 2];
    return hunk.subHunkCount();
}

bool HunksCursor::isValid() const
{
    return m_changeSet.count() > 0 && m_changeSet.count() > m_fileIndex;
}

QString HunksCursor::allowedOptions() const
{
    QString allowed = "ynsfqadjk";
    if (m_totalHunks == -1)
        allowed += "c";
    return allowed;
}

