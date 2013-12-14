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
#include <QDebug>

KConfigXtWriter::KConfigXtWriter(QObject *parent)
        : QObject(parent)
{
}

KConfigXtWriter::KConfigXtWriter(const QString& xmlFilePath, QObject *parent)
        : QObject(parent)
{
    setConfigXmlFile(xmlFilePath);
}

KConfigXtWriter::~KConfigXtWriter()
{
}

void KConfigXtWriter::setConfigXmlFile(const QString& filename)
{
    m_xmlFile = filename;
}

void KConfigXtWriter::editEntry(const QString& groupName, const QString& entryName,
                                         const QString elementName, const QString& newValue)
{
    KConfigXtReaderItem::EntryNode n = entryNode(groupName, entryName);
    if (elementName == "name") {
        n.entryName = newValue;
    } else if (elementName == "type") {
        n.entryType = newValue;
    } else if (elementName == "default") {
        n.entryValue = newValue;
    } else if (elementName == "label") {
        n.entryDescriptionType = KConfigXtReaderItem::Label;
    } else if (elementName == "whatthis") {
        n.entryDescriptionType = KConfigXtReaderItem::WhatsThis;
    } else if (elementName == "tooltip") {
        n.entryDescriptionType = KConfigXtReaderItem::ToolTip;
    } else {
        n.entryDescriptionValue = newValue;
    }

    KConfigXtReaderItem::GroupNode n2 = groupNode(groupName);
    int index = n2.entryNodeList.indexOf(entryNode(groupName, entryName));
    n2.entryNodeList.replace(index, n);

    int index2 = m_data.indexOf(groupNode(groupName));
    m_data.replace(index2, n2);
    writeXML(m_data);
}

void KConfigXtWriter::editGroupName(const QString& groupName, const QString& newValue)
{
    KConfigXtReaderItem::GroupNode n = groupNode(groupName);
    n.groupName = newValue;
    int index = m_data.indexOf(groupNode(groupName));
    //TODO show kmessagebox

    if (index == -1) {
        return;
    }

    m_data.replace(index, n);
    writeXML(m_data);
}

bool KConfigXtWriter::removeGroup(const QString& groupName)
{
    int count = m_data.removeAll(groupNode(groupName));

    bool ok = false;
    if (count > 0) {
        ok = true;
        writeXML();
    }

    return ok;
}

bool KConfigXtWriter::removeEntry(const QString& groupName, const QString& entryName)
{
    KConfigXtReaderItem::GroupNode n = groupNode(groupName);
    int count = n.entryNodeList.removeAll(entryNode(groupName, entryName));

    int index = m_data.indexOf(groupNode(groupName));
    m_data.replace(index, n);

    bool ok = false;
    if (count > 0) {
        ok = true;
        writeXML();
    }

    return ok;
}

void KConfigXtWriter::addNewEntry(const KConfigXtReaderItem::EntryNode newEntry)
{
    KConfigXtReaderItem::GroupNode n = groupNode(newEntry.groupName);
    n.entryNodeList.append(newEntry);
    int index = m_data.indexOf(n);

    m_data.replace(index, n);
    writeXML(m_data);
}

void KConfigXtWriter::addNewGroup(const QString& groupName)
{
    KConfigXtReaderItem::GroupNode n;
    n.groupName = groupName;

    m_data.append(n);

    writeXML();
}

KConfigXtReaderItem::EntryNode KConfigXtWriter::entryNode(const QString& groupName, const QString& entryName) const
{
    KConfigXtReaderItem::GroupNode n = groupNode(groupName);
    for (int i = 0; i < n.entryNodeList.length(); i++) {
        KConfigXtReaderItem::EntryNode n2 = n.entryNodeList.at(i);
        if (n2.groupName == groupName && n2.entryName == entryName) {
            return n2;
        }
    }

    KConfigXtReaderItem::EntryNode e;
    return e;
}

KConfigXtReaderItem::GroupNode KConfigXtWriter::groupNode(const QString& groupName) const
{
    for (int i = 0; i < m_data.length(); i++) {
        KConfigXtReaderItem::GroupNode n = m_data.at(i);
        if (n.groupName == groupName) {
            return n;
        }
    }

    KConfigXtReaderItem::GroupNode g;
    return g;
}

void KConfigXtWriter::setData(const QList<KConfigXtReaderItem::GroupNode>& data)
{
    m_data = data;
}

QList<KConfigXtReaderItem::GroupNode> KConfigXtWriter::data() const
{
    return m_data;
}

void KConfigXtWriter::writeXML()
{
    writeXML(m_data);
}

void KConfigXtWriter::writeXML(const QList<KConfigXtReaderItem::GroupNode>& data)
{
    QFile f (m_xmlFile);
    if (!f.open(QFile::WriteOnly)) {
        KMessageBox::error(0, i18n("This xml file isn't writable"));
    }

    QXmlStreamWriter writer(&f);

    if (data.isEmpty()) {
        return;
    }

    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(4);
    //start the document
  writer.writeStartDocument();


    //start kcfg element
    writer.writeStartElement("kcfg");
    QString ns;
    ns.append("http://www.kde.org/standards/kcfg/1.0\"");
    ns.append("\n");
    ns.append("     xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ");
    ns.append("\n");
    ns.append("     xsi:schemaLocation=\"http://www.kde.org/standards/kcfg/1.0");
    ns.append("\n");
    ns.append("     http://www.kde.org/standards/kcfg/1.0/kcfg.xsd");

    writer.writeDefaultNamespace(ns);

    //start kcfgfile element
    writer.writeStartElement("kcfgfile");
    writer.writeAttribute("name", "");
    writer.writeEndElement();

    for (int i = 0; i < data.length(); i++) {
        KConfigXtReaderItem::GroupNode groupIt = data.at(i);

        //start group
        writer.writeStartElement("group");

        writer.writeAttribute("name", groupIt.groupName);

        for (int j = 0; j < groupIt.entryNodeList.length(); j++) {
            KConfigXtReaderItem::EntryNode entryIt = groupIt.entryNodeList.at(j);

            //start entry
            writer.writeStartElement("entry");
            writer.writeAttribute("name", entryIt.entryName);
            writer.writeAttribute("type", entryIt.entryType);

            //start label/tooltip/whatthis
            //check what if we have label or tooltip or whatthis
            if (entryIt.entryDescriptionType == KConfigXtReaderItem::Label) {
                writer.writeStartElement("label");
            } else if (entryIt.entryDescriptionType == KConfigXtReaderItem::ToolTip) {
                writer.writeStartElement("tooltip");
            } else if (entryIt.entryDescriptionType == KConfigXtReaderItem::WhatsThis) {
                writer.writeStartElement("whatsthis");
            } else {
                qDebug() << "There is no descriptionType, probably something is wrong";
            }

            writer.writeCharacters(entryIt.entryDescriptionValue);
            //end label/tooltip/whatthis
            writer.writeEndElement();

            //start default
            writer.writeStartElement("default");
            writer.writeCharacters(entryIt.entryValue);
            //end default
            writer.writeEndElement();

            //end entry
            writer.writeEndElement();
        }
        //end group
        writer.writeEndElement();
    }
    //end kcfg element
    writer.writeEndElement();

    //end the document
    writer.writeEndDocument();

    //finally close the device and write our data
    writer.device()->close();
}

