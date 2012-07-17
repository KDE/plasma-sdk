/*
   This file is part of the KDE project
   Copyright 2012 by Giorgos Tsiapaliwkas <terietor@gmail.com>

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

#include <QMultiHash>
#include <QVariant>
#include <QList>
#include <QXmlStreamReader>

#include "kconfigxtreader.h"

class KConfigXtWriterItem
{

public:
    KConfigXtWriterItem(QObject* parent = 0);

    QString group() const;
    void setGroup(const QString& groupName);

    QList<KConfigXtReaderItem> entries() const;
    void setEntries(QList<KConfigXtReaderItem> entries);

private:
    QString m_group;
    QList<KConfigXtReaderItem> m_entries;
};


class KConfigXtWriter : public QObject
{

    Q_OBJECT
public:
    KConfigXtWriter(QObject *parent = 0);
    KConfigXtWriter( const QString& xmlFilePath, QObject *parent = 0);

    /**
     * This is the xml in which the data will be
     * writen
     **/
    void setConfigXmlFile(const QString& filename);

    /**
     * Returns the data that will be writen in the xml
     **/
    QList<KConfigXtWriterItem> data();

    /**
     * Sets the data that will be writen in the xml
     **/
    void setData(QList<KConfigXtWriterItem> dataList);

    /**
     * writes the data in the xml
     **/
    void writeXML();

    QList<KConfigXtWriterItem> readerItemsToWriterIems(QStringList& groupList,
                                                       QList<KConfigXtReaderItem> entriesList) const;

private:
    QList<KConfigXtWriterItem> m_dataList;
    QFile m_xmlFile;
    QXmlStreamWriter *m_writer;
};

#endif
