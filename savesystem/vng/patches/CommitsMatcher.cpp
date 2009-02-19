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
#include "CommitsMatcher.h"
#include "Commit.h"
#include "../CommandLineParser.h"

CommitsMatcher::CommitsMatcher()
    : useRegExp(false), useRegExpTo(false), useMatcher(false), useMatcherTo(false)
{
    CommandLineParser *args = CommandLineParser::instance();

    if (args->contains("to-match")) {
        m_matcherTo = QStringMatcher(args->optionArgument("to-match"), Qt::CaseInsensitive);
        useMatcherTo = true;
    }
    else if (args->contains("to-patch")) {
        m_regExpTo = QRegExp(args->optionArgument("to-patch"),  Qt::CaseInsensitive, QRegExp::RegExp2);
        useRegExpTo = true;
    }

    m_state = BeforeFrom;
    if (args->contains("from-match")) {
        m_matcher = QStringMatcher(args->optionArgument("from-match"), Qt::CaseInsensitive);
        useMatcher = true;
    }
    else if (args->contains("from-patch")) {
        m_regExp = QRegExp(args->optionArgument("from-patch"),  Qt::CaseInsensitive, QRegExp::RegExp2);
        useRegExp = true;
    }
    else {
        m_state = MatchingPerItem;
        findNormalMatchers();
    }
}

CommitsMatcher::ExpectedAction CommitsMatcher::match(const Commit &commit)
{
    switch (m_state) {
    case AllClear: return ShowPatch;
    case BeforeFrom:
        if ((useMatcher && m_matcher.indexIn(commit.author()) == -1
                && m_matcher.indexIn(commit.logMessage()) == -1)
              || (useRegExp && m_regExp.indexIn(commit.author()) == -1
                && m_regExp.indexIn(commit.logMessage()) == -1))
            return SkipPatch;
        m_state = MatchingPerItem;
        findNormalMatchers();
        return match(commit);
    case MatchingPerItem:
        if ((useMatcher && m_matcher.indexIn(commit.author()) == -1
                && m_matcher.indexIn(commit.logMessage()) == -1)
              || (useRegExp && m_regExp.indexIn(commit.author()) == -1
                && m_regExp.indexIn(commit.logMessage()) == -1))
            return SkipPatch;
        if (!useMatcherTo && !useRegExpTo)
            return ShowPatch;
    case SearchingForTo:
        if ((useMatcherTo && m_matcherTo.indexIn(commit.author()) == -1
                && m_matcherTo.indexIn(commit.logMessage()) == -1)
              || (useRegExpTo && m_regExpTo.indexIn(commit.author()) == -1
                && m_regExpTo.indexIn(commit.logMessage()) == -1))
            return ShowPatch;
        m_state = AfterTo;
        return ShowPatch;
    default:
    case AfterTo:
        return Exit;
    }
}

void CommitsMatcher::findNormalMatchers()
{
    CommandLineParser *args = CommandLineParser::instance();
    if (args->contains("match")) {
        m_matcher = QStringMatcher(args->optionArgument("match"), Qt::CaseInsensitive);
        useMatcher = true;
    }
    else if (args->contains("patches")) {
        m_regExp = QRegExp(args->optionArgument("patches"),  Qt::CaseInsensitive, QRegExp::RegExp2);
        useRegExp = true;
    }
    else if (useMatcherTo || useRegExpTo)
        m_state = SearchingForTo;
    else
        m_state = AllClear;
}
