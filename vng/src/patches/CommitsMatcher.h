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

class Commit;

#include <QRegExp>
#include <QStringMatcher>

class CommitsMatcher
{
public:
    enum ExpectedAction {
        SkipPatch,
        ShowPatch,
        Exit
    };

    CommitsMatcher();
    ExpectedAction match(const Commit &commit);

private:
    void findNormalMatchers();

    enum State {
        AllClear,       // no matching to be done. All pass per definition.
        BeforeFrom,     // searching for the 'from' All fail until we find that.
        MatchingPerItem,// Skipping / matching per item. As appropriate.
        SearchingForTo, // Pass all until we find the 'to' match.
        AfterTo         // We are done, fail all.
    };
    bool useRegExp, useRegExpTo, useMatcher, useMatcherTo;
    QRegExp m_regExp, m_regExpTo;
    QStringMatcher m_matcher, m_matcherTo;
    State m_state;
};
