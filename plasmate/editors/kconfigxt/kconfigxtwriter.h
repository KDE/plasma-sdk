/*
   This file is part of the KDE project
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

#ifndef KCONFIGXTWRITER_H
#define KCONFIGXTWRITER_H

#include "kconfigxtreader.h"

#include <QObject>

class KConfigXtWriter : public QObject
{
    Q_OBJECT
public:
    KConfigXtWriter(QObject *parent = 0);
    KConfigXtWriter( const QString& xmlFilePath, QObject *parent = 0);
    ~KConfigXtWriter();

    /**
     * This is the xml in which the data will be
     * writen
     **/
    void setConfigXmlFile(const QString& filename);

    void setData(const QList<KConfigXtReaderItem::GroupNode>& data);
    QList<KConfigXtReaderItem::GroupNode> data() const;

    void addNewEntry(const KConfigXtReaderItem::EntryNode newEntry);

    void addNewGroup(const QString& groupName);

    void editGroupName(const QString& groupName, const QString& newValue);

    void editEntry(const QString& groupName, const QString& entryName, const QString elementName, const QString& newValue);

    bool removeEntry(const QString& groupName, const QString& entryName);

    bool removeGroup(const QString& groupName);

private:
    /**
     * writes the data in the xml
     **/
    void writeXML(const QList<KConfigXtReaderItem::GroupNode> &data);

    void writeXML();

    QList<KConfigXtReaderItem::GroupNode> m_data;
    QString m_xmlFile;
    KConfigXtReaderItem::EntryNode entryNode(const QString& groupName, const QString& entryName) const;
    KConfigXtReaderItem::GroupNode groupNode(const QString& groupName) const;
};

#endif
