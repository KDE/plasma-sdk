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

#ifndef Logger_H
#define Logger_H

#include <QTextStream>

class LoggerPrivate;

class Logger
{
public:
    enum Verbosity {
        Quiet,
        Chatty,
        Verbose,
        Debug
    };

    Logger();

    /// return an outputstream logger irregardless of the loglevel
    static QTextStream &standardOut();
    /// return an outputstream or a dummy logger based on verbosity set on the logger and passed as argument.
    static QTextStream &log(Verbosity verbosity);
    /// equals log(Debug)
    static QTextStream &debug();
    /// equals log(Chatty)
    static QTextStream &info();
    /// equals log(Verbose)
    static QTextStream &warn();
    /// equals log(Quiet)
    static QTextStream &error();
    /**
     * in case a pager is used calling this will explicitly flush the output to the screen.
     * You should not have to call this because the different Logger methods which return a QTextStream call it automatically.
     */
    static void flushPager();

    /**
     * by calling this method all the logger operations will be routed via a pager like less (if available).
     * @see stopPager()
     */
    static void startPager();
    /**
     * by calling this method all the logger operations will be routed via a pager like less (if available).
     * @see stopPager()
     */
    static void stopPager();
    /// return true when there is a pager in use that doesn't support throughput of color information.
    static bool hasNonColorPager();
    /// Set the verbosity of the logger.  Already in use logger streams will not be affected.
    static void setVerbosity(Verbosity verbosity);
    /// return the verbosity set on the logger.
    static Verbosity verbosity();

private:
    LoggerPrivate * const d;
};

#endif
