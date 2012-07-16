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

#include "kconfigxtwriter.h"

#include <KMessageBox>
#include <KLocalizedString>

#include <QFile>
#include <QXmlStreamWriter>

#include <KDebug>

KConfigXtWriterItem::KConfigXtWriterItem(QObject* parent)
{
    Q_UNUSED(parent)
}

QString KConfigXtWriterItem::group() const
{
    return m_group;
}


void KConfigXtWriterItem::setGroup(const QString& group)
{
    m_group = group;
}

QList< KConfigXtReaderItem > KConfigXtWriterItem::entries() const
{
    return m_entries;
}

void KConfigXtWriterItem::setEntries(QList<KConfigXtReaderItem> entries)
{
    m_entries = entries;
}

KConfigXtWriter::KConfigXtWriter(QObject *parent)
        : QObject(parent)
{
}

KConfigXtWriter::KConfigXtWriter(const QString& xmlFilePath, QObject *parent)
        : QObject(parent)
{
    setConfigXmlFile(xmlFilePath);
}

void KConfigXtWriter::setConfigXmlFile(const QString& filename)
{
    QFile xmlFile(filename);

    //clear the file
    xmlFile.resize(0);

    if (!xmlFile.open(QIODevice::ReadWrite)) {
        return;
    }

    m_writer.setDevice(&xmlFile);

}

QList< KConfigXtWriterItem > KConfigXtWriter::data()
{
    return m_dataList;
}

void KConfigXtWriter::setData(QList< KConfigXtWriterItem > dataList)
{
    m_dataList = dataList;
}

void KConfigXtWriter::writeXML()
{
    if (m_dataList.isEmpty()) {
        return;
    }

    m_writer.setAutoFormatting(true);
    m_writer.writeStartDocument();

    foreach(const KConfigXtWriterItem& writerItem, m_dataList) {
        //start group
        m_writer.writeStartElement("group");
        //start kcfg element
        m_writer.writeStartElement("kcfg");

        //TODO add the above into the xml
        /*every kconfigxt xml file contains the below data
         * 
         *   <?xml version="1.0" encoding="UTF-8"?>
         *   <kcfg xmlns="http://www.kde.org/standards/kcfg/1.0"
         *   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         *   xsi:schemaLocation="http://www.kde.org/standards/kcfg/1.0
         *   http://www.kde.org/standards/kcfg/1.0/kcfg.xsd" >
         *   <kcfgfile name=""/>
         *   </kcfg>
         *
         */

        m_writer.writeAttribute("name", writerItem.group());

        foreach(const KConfigXtReaderItem& readerItem, writerItem.entries()) {
            //start entry
            m_writer.writeStartElement("entry");
            m_writer.writeAttribute("name", readerItem.entryName());
            m_writer.writeAttribute("type", readerItem.entryType());
            //start default
            m_writer.writeStartElement("default");
            m_writer.writeCharacters(readerItem.entryValue());
            //end default
            m_writer.writeEndElement();
            //end entry
            m_writer.writeEndElement();
        }
        //end group
        m_writer.writeEndElement();
    }
    //end kcfg
    m_writer.writeEndElement();
    //end the document
    m_writer.writeEndDocument();
}

QList<KConfigXtWriterItem> KConfigXtWriter::readerItemsToWriterIems(QStringList& groupList,
                                            QList<KConfigXtReaderItem> entriesList) const
{

    QList<KConfigXtWriterItem> list;

    foreach(const QString& group, groupList) {
        KConfigXtWriterItem tmpItem;
        tmpItem.setGroup(group);
        QList<KConfigXtReaderItem> tmpEntryList;
        foreach (const KConfigXtReaderItem& entry, entriesList) {
            if (group == entry.groupName()) {
                KConfigXtReaderItem tmpEntry;
                tmpEntry.setEntryName(entry.entryName());
                tmpEntry.setEntryType(entry.entryType());
                tmpEntry.setEntryValue(entry.entryValue());

                tmpEntryList.append(tmpEntry);
            }
        }
        tmpItem.setEntries(tmpEntryList);
        list.append(tmpItem);
    }

    return list;
}

