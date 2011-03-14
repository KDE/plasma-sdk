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

#include "kconfigxtparser.h"

class KConfigXtParserPrivate
{
public:
    QString filename;
    QStringList groups;
    QHash<QString, QVariant> keysAndValues;
};

KConfigXtParser::KConfigXtParser()
        : d(new KConfigXtParserPrivate())
{

}

KConfigXtParser::~KConfigXtParser()
{
    delete d;
}

void KConfigXtParser::setConfigXmlFile(const QString& filename)
{
    d->filename = filename;
}

bool KConfigXtParser::parse()
{
    return true;
}

QStringList KConfigXtParser::groups() const
{
    return d->groups;
}

QHash<QString, QVariant> KConfigXtParser::keysAndValues() const
{
    return d->keysAndValues;
}
