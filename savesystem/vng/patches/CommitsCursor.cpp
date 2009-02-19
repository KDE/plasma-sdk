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

#include "CommitsCursor.h"
#include "CommitsMatcher.h"

#include <QDebug>

CommitsCursor::CommitsCursor(SelectionType selectionType)
    : m_currentIndex(1),
    m_oldestIndex(0),
    m_head("HEAD"),
    m_current(m_head),
    m_done(! m_head.isValid()),
    m_matcher(0),
    m_selectionType(selectionType)
{
    if (selectionType == SelectRange)
        m_firstInBranch = m_head.firstCommitInBranch();
}

CommitsCursor::~CommitsCursor()
{
    delete m_matcher;
}

int CommitsCursor::forward(Scope scope, bool skipAnswered)
{
    Q_UNUSED(scope);
    Q_UNUSED(skipAnswered);
    // go to a parent commit
    int newIndex = m_currentIndex;
    if (m_current.previousCommitsCount() > 0) {
        Commit parent = m_current.previous()[0];
        while(true) {
            if (! parent.isValid())
                return m_currentIndex;
            ++newIndex;
            CommitsMatcher::ExpectedAction action = CommitsMatcher::ShowPatch;
            if (m_matcher)
                action = m_matcher->match(parent);
            if (action == CommitsMatcher::ShowPatch) {
                m_current = parent;
                m_currentIndex = newIndex;
                return newIndex;
            }
            if (action == CommitsMatcher::Exit) {
                m_done = true;
                return m_currentIndex;
            }
            if (parent.previousCommitsCount() == 0) {// no more commits;
                m_done = true;
                return m_currentIndex;
            }
            parent = parent.previous()[0];
        }
    }
    return m_currentIndex;
}

int CommitsCursor::back(Scope scope)
{
    Q_UNUSED(scope);
    Commit c = m_current.next();
    int newIndex = m_currentIndex;
    while(true) {
        if (! c.isValid())
            return m_currentIndex;
        --newIndex;
        CommitsMatcher::ExpectedAction action = CommitsMatcher::ShowPatch;
        if (m_matcher)
            action = m_matcher->match(c);
        if (action == CommitsMatcher::ShowPatch) {
            m_current = c;
            if (m_current == m_firstInBranch)
                m_done = true;
            m_currentIndex = newIndex;
            return m_currentIndex;
        }
        if (action == CommitsMatcher::Exit) {
            m_done = true;
            return m_currentIndex;
        }
        c = c.next();
    }
}

void CommitsCursor::setResponse(bool response, Scope scope)
{
    Q_UNUSED(scope);
    m_current.setAcceptance(response ? Vng::Accepted : Vng::Rejected);
    if ((m_selectionType == SelectRange && !response)
            || (m_selectionType == SelectOnePatch && response)) // exit when user says 'no'
        m_done = true;
    m_oldestIndex = qMax(m_oldestIndex, m_currentIndex);
}

int CommitsCursor::count()
{
    return -1; // TODO we _can_ count.. Its just not quite useful in most cases.
}

void CommitsCursor::forceCount()
{
    // TODO sure you want to do that??
}

QString CommitsCursor::currentText() const
{
    return QString("\n") +
        m_current.commitTime().toString() + QString("  ") + m_current.author() + QString("\n ") +
        m_current.logMessage();
}

QString CommitsCursor::helpMessage() const
{
    return QString(
        "How to use unrecord...\n"
        "y: unrecord this patch\n"
        "n: don't unrecord it\n\n"
        // "v: view this patch in full with pager\n"
        // "x: view a summary of this patch\n"
        "d: unrecord selected patches\n"
        "q: cancel unrecord\n\n"
        "j: skip to next patch\n"
        "k: back up to previous patch\n"
        "c: calculate number of patches\n"
        "h or ?: show this help\n");
}

bool CommitsCursor::isValid() const
{
    return !m_done && m_current.previousCommitsCount() > 0;
}

QString CommitsCursor::allowedOptions() const
{
    QString allowed = "ynq";
    if (m_selectionType == SelectRange)
        allowed += "djk";
//   if (m_total == -1)
//       allowed += "c";
    return allowed;
}

Commit CommitsCursor::head()
{
    return m_head;
}

void CommitsCursor::setUseMatcher(bool use)
{
    if (! use) {
        delete m_matcher;
        m_matcher = 0;
        return;
    }
    if (m_matcher)
        return;
    m_matcher = new CommitsMatcher();
}

