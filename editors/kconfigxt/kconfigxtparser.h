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

#include <QMultiHash>
#include <QVariant>
#include <QList>
#include <QXmlStreamReader>

class KConfigXtParserItem
{

public:
    KConfigXtParserItem(QObject* parent = 0);

    QString groupName() const;
    void setGroupName(const QString& groupName);

    QString entryName() const;
    void setEntryName(const QString& entryName);

    QString entryType() const;
    void setEntryType(const QString& entryType);

    QString entryValue() const;
    void setEntryValue(const QString& entryValue);

    bool operator==(const KConfigXtParserItem& item);

private:
    QString m_groupName;
    QString m_entryName;
    QString m_entryType;
    QString m_entryValue;
};


class KConfigXtParser : public QObject
{

    Q_OBJECT
public:
    KConfigXtParser(QObject *parent = 0);

    void setConfigXmlFile(const QString& filename);

    /**
     * Parses a kcfg file.
     * Should be called after setConfigXmlFile()
     */
    bool parse();

    /**
     * Returns the data from the xml file.
     * Valid only after a successful call to parse()
     **/
    QList<KConfigXtParserItem> dataList() const;

private:
    void parseGroup(QXmlStreamReader& reader);
    void parseEntry(QXmlStreamReader& reader);
    bool m_parseResult;

    QString m_filename;
    QList<KConfigXtParserItem> m_dataList;
    KConfigXtParserItem m_data;
};

#endif
