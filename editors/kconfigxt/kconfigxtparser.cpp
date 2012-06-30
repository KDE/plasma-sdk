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

#include <KMessageBox>
#include <KLocalizedString>

#include <QFile>
#include <QXmlStreamReader>

#include <QDebug>
KConfigXtParserItem::KConfigXtParserItem(QObject* parent)
{
}

QString KConfigXtParserItem::groupName() const
{
    return m_groupName;
}

QString KConfigXtParserItem::entryName() const
{
    return m_entryName;
}

QString KConfigXtParserItem::entryType() const
{
    return m_entryType;
}

QString KConfigXtParserItem::entryValue() const
{
    return m_entryValue;
}

void KConfigXtParserItem::setGroupName(const QString& groupName)
{
    m_groupName = groupName;
    qDebug() << "groupName:" + groupName;
}

void KConfigXtParserItem::setEntryName(const QString& entryName)
{
    m_entryName = entryName;
    qDebug() << "entryName:" + entryName;
}

void KConfigXtParserItem::setEntryType(const QString& entryType)
{
    //those are the possible types
    QStringList types;
    types << "string"
    <<"String"
    <<"StringList"
    <<"Font"
    <<"Rect"
    <<"Size"
    <<"Color"
    <<"Point"
    <<"Int"
    <<"UInt"
    <<"Bool"
    <<"Double"
    <<"DateTime"
    <<"LongLong"
    <<"ULongLong"
    <<"IntList"
    <<"Enum"
    <<"Path"
    <<"PathList"
    <<"Password"
    <<"Url"
    <<"UrlList";

    //check if the entryType is valid
    foreach(const QString& type, types) {
        if (type == entryType) {
            //its a type
            m_entryType = type;
        }
    }
    qDebug() << "entryType:" + entryType;
}

void KConfigXtParserItem::setEntryValue(const QString& entryValue)
{
    m_entryValue = entryValue;
    qDebug() << "m_entryValue:" + m_entryValue;
}

KConfigXtParser::KConfigXtParser(QObject *parent)
        : QObject(parent),
        m_parseResult(false)
{

}

void KConfigXtParser::setConfigXmlFile(const QString& filename)
{
    m_filename = filename;
}

bool KConfigXtParser::parse()
{
    QFile xmlFile(m_filename);

    if (!xmlFile.open(QIODevice::ReadWrite)) {
        KMessageBox::error(0, i18n("The xml file isn't writable"));
        return false;
    }

    QXmlStreamReader reader;
    reader.setDevice(&xmlFile);

   //we will parse the file unti its end or until an error occurs
    while (!reader.atEnd() && !reader.hasError()) {
        //we need the token in order to check what is the element
        QXmlStreamReader::TokenType token = reader.readNext();

        //we have an element
        if(token == QXmlStreamReader::StartElement) {
            //we have a new group
            if(reader.name() == "group") {
                parseGroup(reader);
            }

        }
    }

    if(reader.hasError()) {
        //an error has occured
        KMessageBox::error(0, i18n("The xml parsing has failed"));

        //clear the reader
        reader.clear();
        //the parse has failed
        m_parseResult = false;
    }

    m_dataList.append(m_data);
    //the parse was successfull
    m_parseResult = true;

    return m_parseResult;
}

void KConfigXtParser::parseGroup(QXmlStreamReader& reader)
{
    //verify if we really has a group
    if(reader.tokenType() != QXmlStreamReader::StartElement &&
        reader.name() == "person") {
        //fail!
        return;
    }

    //check if <group name="something"> exists
    if(reader.attributes().hasAttribute("name")) {
    // We'll add it to the hash
    m_data.setGroupName(reader.attributes().value("name").toString());
    qDebug() << "auto einai to name tou group:" + reader.attributes().value("name").toString();
    }


    //we are still in the group element so let's go the next element
    reader.readNext();
    //we will loop over the group element until its end
    while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
        reader.name() == "group")) {
        if(reader.tokenType() == QXmlStreamReader::StartElement) {
            //check if this is an entry element
            if(reader.name() == "entry") {
                parseEntry(reader);
            }
        }
        reader.readNext();
    }
}

void KConfigXtParser::parseEntry(QXmlStreamReader& reader)
{
    // Check if we are inside an element like <entry name="interval" type="Int">
    if(reader.tokenType() != QXmlStreamReader::StartElement) {
        return;
    }

    //check if there is a type attribute
    //if there isn't fail!
    if (reader.attributes().hasAttribute("type")) {
        qDebug() << "auto einai to attribute  type tou entry:" + reader.attributes().value("type").toString();
        //now we can take the entry's name and type
        m_data.setEntryName(reader.attributes().value("name").toString());
        m_data.setEntryType(reader.attributes().value("type").toString());

        m_parseResult = true;

        qDebug() << "entry name:" + reader.name().toString();
        qDebug() << "entry type:" + reader.attributes().value("type").toString();
    } else {
        //there is no type, fail
        m_parseResult = false;
        return;
    }

    //go ahead!
    reader.readNext();

    //for as long as we are inside the entry element
    //we need to search for its value
    int counter = 0;
    while(!(reader.tokenType() == QXmlStreamReader::EndElement &&
        reader.name() == "entry")) {

        //we have a default element
        if (reader.name().toString() == "default") {
            qDebug() << "mpike!!!";

            //go ahead one more!
            reader.readNext();

            //we need the text of default
            QString defaultText = reader.text().toString();

            //check if we have characters
            if(reader.tokenType() != QXmlStreamReader::Characters) {
                return;
            }
            m_data.setEntryValue(reader.text().toString());

        }
        reader.readNext();
    }
}

QList<KConfigXtParserItem> KConfigXtParser::dataList() const
{
    return m_dataList;
}

