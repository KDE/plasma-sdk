/*
   This file is part of the KDE project
   Copyright 2009 by Dmitry Suzdalev <dimsuz@gmail.com>
   Copyright 2012 by Giorgos Tsiapaliokas <terietor@gmail.com>

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

#ifndef KCONFIGXTREADER_H
#define KCONFIGXTREADER_H

#include <QDomDocument>
#include <QList>
#include <QObject>

class KConfigXtReaderItem
{

public:
    KConfigXtReaderItem();

    enum DescriptionType {
        Label = 0,
        ToolTip,
        WhatsThis
    };

    struct EntryNode {
        QString groupName;
        QString entryName;
        QString entryType;
        QString entryValue;
        QString entryDescriptionValue;
        KConfigXtReaderItem::DescriptionType entryDescriptionType;
        inline bool operator==(const KConfigXtReaderItem::EntryNode& e)
        {
            return (e.groupName == this->groupName &&
            e.entryName == this->entryName &&
            e.entryType == this->entryType &&
            e.entryValue == this->entryValue &&
            e.entryDescriptionValue == this->entryDescriptionValue &&
            e.entryDescriptionType == this->entryDescriptionType);
        };
    };

    struct GroupNode {
        QString groupName;
        QList<KConfigXtReaderItem::EntryNode> entryNodeList;
        inline bool operator==(const KConfigXtReaderItem::GroupNode& g)
        {
            return g.groupName == this->groupName;
        };
    };

    QList<KConfigXtReaderItem::GroupNode> groupNodes() const;

    void appendGroupNode(const KConfigXtReaderItem::GroupNode& groupNode);

    void clear();
    bool isEmpty();

private:
    QList<GroupNode> m_groupNodeList;
};

class KConfigXtReader : public QObject
{

    Q_OBJECT
public:
    KConfigXtReader(QObject *parent = 0);

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
    KConfigXtReaderItem data() const;

private:
    QString attributeValue(const QDomNode element, const QString& attributeName) const;
    QString m_filename;
    KConfigXtReaderItem m_data;
};

#endif
