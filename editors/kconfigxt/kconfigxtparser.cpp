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
/*#include <QDomElement>
#include <QDomNode>*/

KConfigXtParser::KConfigXtParser(QObject *parent)
        : QObject(parent)
{

}

void KConfigXtParser::setConfigXmlFile(const QString& filename)
{
    m_filename = filename;
}
#include <QDebug>
void KConfigXtParser::parse()
{
    QFile xmlFile(m_filename);

    if (!xmlFile.open(QIODevice::ReadWrite)) {
        KMessageBox::error(0, i18n("The xml file isn't writable"));
        return;
    }

    //we need a temporary hash
    QHash<QString, QVariant> tmpHash;

    QXmlStreamReader reader;
    reader.setDevice(&xmlFile);
QStringList groups;
QStringList entries;
QStringList elses;

    QStringList groupNameList;
    QStringList entriesList;

    while (!reader.atEnd()) {
        //for as long as we are still in the same element take the data
        if (reader.readNextStartElement()) {
        //ignore all the elements that are
        //named "kcfg" and "kcgfile" or they are ""
        //we don't want those to appear in the ui
            if (reader.name() != "kcfg" && reader.name() != "kcfgfile" && !reader.name().isEmpty()) {
                //qDebug() << "element text" <<reader.attributes().data()->value().toString();
              //  qDebug() << "element name" << reader.name().toString()
                if (reader.name() == "group") {
                    //we have a new group
                  //  tmpHash["group"] = reader.attributes().data()->value().toString();
                    groupNameList << reader.attributes().data()->value().toString();
                } else if(reader.name() == "entry") {
                    //we have a new entry
                    entriesList << reader.attributes().data()->value().toString();
                } else {
                  //  reader.readNext();
                    elses <<  reader.name().toString() + ":" + reader.attributes().data()->value().toString();
                    //QVector<QXmlStreamAttribute> vector = reader.attributes().data();
                    qDebug() << "qualifiedName:" + reader.qualifiedName().toString();
                    qDebug() << "qualifiedValue:" << reader.attributes().value(reader.qualifiedName().toString());
                }
            }
        }
    }


    foreach(const QString& groupName, groupNameList) {
        foreach(const QString& entry, entriesList) {
            m_groups[groupName] << entry;
        }
    }
    
    
    foreach(QString g, groupNameList) {
        qDebug() << "groups:" <<g;
    }
    foreach(QString e, entriesList) {
        qDebug() << "entries:"<< e;
    }
    
    foreach(QString e, elses) {
        qDebug() << "elses:"<< e;
    }
}

QHash<QString, QStringList> KConfigXtParser::groups() const
{
    return m_groups;
}

QHash<QString, QVariant> KConfigXtParser::keysAndValues() const
{
    return m_keysAndValues;
}
