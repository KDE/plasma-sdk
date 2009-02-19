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

#ifndef INTERVIEWCURSOR_H
#define INTERVIEWCURSOR_H

#include <QString>

class Configuration;

class InterviewCursor
{
public:
    enum Scope {
        ItemScope,
        BlockScope,
        FullRange
    };

    InterviewCursor();
    virtual ~InterviewCursor();

    void setConfiguration(Configuration &config) {
        m_config = &config;
    }

    /**
     * Moves the cursor forward in the list.
     * returns the new index.
     * Moving the cursor past the last item will make isValid() return false.
     */
    virtual int forward(Scope scope = ItemScope, bool skipAnswered = true) = 0;

    /**
     * Moves the cursor backwards in the list.
     * returns the new index.
     * This method can not move the cursor to be before the first item.
     */
    virtual int back(Scope scope = ItemScope) = 0;
    virtual void setResponse(bool response, Scope scope = ItemScope) = 0;
    virtual QString currentText() const = 0;

    /**
     * Returns true if the item the cursor is on is a valid one.  The cursor can point to an invalid item when there
     * are no items in the document, or when the cursor moved past the end of the list.
     */
    virtual bool isValid() const = 0;

    /**
     * Returns the amount of items in the list, or -1 if the amount is not fully determined yet.
     * @see forceCount()
     */
    virtual int count() = 0;

    /**
     * Will force a count of all items, potentially blocking for extended times while counting.
     * Afterwards the count() method will return a valid count.
     */
    virtual void forceCount() = 0;

    virtual QString helpMessage() const = 0;

    /**
     * Returns all the interview options that the user can use on this cursor as a concatanated string.
     * Each option is encoded using one character, as follows;
     *  y  allow the user to accept the current item.
     *  n  allow the user to decline the current item.
     *  s  allow the user to skip over a whole block.
     *  f  allow the user to accept all items in the block and skip over it.
     *  q  allow the user to cancel the interaction.
     *  a  allow the user to accept all remainting items.
     *  d  allow the user to indicate the interaction is succesfully completed.
     *  j  allow the user to forward to the next item without indicating either yes or no.
     *  k  allow the user to go back to the previos item without indicating either yes or no.
     *  c  allow the user to calculate the total number of items.
     */
    virtual QString allowedOptions() const = 0;

protected:
    Configuration *m_config;
};

#endif
