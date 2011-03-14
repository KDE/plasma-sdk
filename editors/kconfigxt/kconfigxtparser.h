/*
   This file is part of the KDE project
   Copyright 2009 by Dmitry Suzdalev <dimsuz@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KCONFIGXTPARSER_H
#define KCONFIGXTPARSER_H

#include <QStringList>
#include <QHash>
#include <QVariant>

class KConfigXtParserPrivate;

class KConfigXtParser
{
public:
    KConfigXtParser();
    ~KConfigXtParser();

    void setConfigXmlFile(const QString& filename);

    /**
     * Parses a kcfg file.
     * Should be called after setConfigXmlFile()
     */
    bool parse();

    /*
     * Returns a list of group names from config file.
     * Valid only after a successful call to parse()
     */
    QStringList groups() const;

    /*
     * Returns a map of keys and values from config file.
     * Valid only after a successful call to parse()
     */
    QHash<QString, QVariant> keysAndValues() const;

private:
    KConfigXtParserPrivate *const d;
};

#endif
