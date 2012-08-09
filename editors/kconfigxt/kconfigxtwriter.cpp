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
        : QObject(parent),
        m_writer(0),
        m_xmlFile(0)
{
}

KConfigXtWriter::KConfigXtWriter(const QString& xmlFilePath, QObject *parent)
        : QObject(parent),
        m_writer(0),
        m_xmlFile(0)
{
    setConfigXmlFile(xmlFilePath);
}

void KConfigXtWriter::setConfigXmlFile(const QString& filename)
{
    m_xmlFile = new QFile(filename, this);

    if (!m_xmlFile->open(QIODevice::WriteOnly)) {
        return;
    }

    m_writer = new QXmlStreamWriter(m_xmlFile);
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

    m_writer->setAutoFormatting(true);
    m_writer->setAutoFormattingIndent(4);

    //start the document
    m_writer->writeStartDocument();

    //start kcfg element
    startKcfgElement();

    //start kcfgfile element
    m_writer->writeStartElement("kcfgfile");
    m_writer->writeAttribute("name", "");
    m_writer->writeEndElement();

    foreach(const KConfigXtWriterItem& writerItem, m_dataList) {
        //start group
        m_writer->writeStartElement("group");

        m_writer->writeAttribute("name", writerItem.group());

        foreach(const KConfigXtReaderItem& readerItem, writerItem.entries()) {
            //start entry
            m_writer->writeStartElement("entry");
            m_writer->writeAttribute("name", readerItem.entryName());
            m_writer->writeAttribute("type", readerItem.entryType());

            //start label/tooltip/whatthis
            //check what if we have label or tooltip or whatthis
            if (readerItem.descriptionType() == KConfigXtReaderItem::Label) {
                m_writer->writeStartElement("label");
            } else if (readerItem.descriptionType() == KConfigXtReaderItem::ToolTip) {
                m_writer->writeStartElement("tooltip");
            } else if (readerItem.descriptionType() == KConfigXtReaderItem::WhatsThis) {
                m_writer->writeStartElement("whatsthis");
            } else {
                kDebug() << "There is no descriptionType, probably something is wrong";
            }

            m_writer->writeCharacters(readerItem.descriptionValue());
            //end label/tooltip/whatthis
            m_writer->writeEndElement();

            //start default
            m_writer->writeStartElement("default");
            m_writer->writeCharacters(readerItem.entryValue());
            //end default
            m_writer->writeEndElement();

            //end entry
            m_writer->writeEndElement();
        }
        //end group
        m_writer->writeEndElement();
    }
    //end kcfg element
    endKcfgElement();

    //end the document
    m_writer->writeEndDocument();

    //finally close the device and write our data
    m_writer->device()->close();
}

void KConfigXtWriter::startKcfgElement()
{
    //write the kcfg manually
    QByteArray startKcfg;
    startKcfg.append("\n");
    startKcfg.append("<kcfg xmlns=\"http://www.kde.org/standards/kcfg/1.0\"");
    startKcfg.append("\n");
    startKcfg.append("     xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
    startKcfg.append("\n");
    startKcfg.append("     xsi:schemaLocation=\"http://www.kde.org/standards/kcfg/1.0\"");
    startKcfg.append("\n");
    startKcfg.append("     http://www.kde.org/standards/kcfg/1.0/kcfg.xsd\" >");
    startKcfg.append("\n");
    m_writer->device()->write(startKcfg);
}

void KConfigXtWriter::endKcfgElement()
{
    QByteArray endKcfg;
    endKcfg.append("\n");
    endKcfg.append("</kcfg>");
    m_writer->device()->write(endKcfg);
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

                if (!tmpEntryList.contains(entry)) {
                    tmpEntryList.append(entry);
                }
            }
        }
        tmpItem.setEntries(tmpEntryList);
        list.append(tmpItem);
    }

    return list;
}

