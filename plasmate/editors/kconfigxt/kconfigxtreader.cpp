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

#include "kconfigxtreader.h"

#include <KMessageBox>
#include <KLocalizedString>

#include <QFile>

#include <KDebug>

KConfigXtReaderItem::KConfigXtReaderItem()
{
}

void KConfigXtReaderItem::appendGroupNode(const KConfigXtReaderItem::GroupNode& groupNode)
{
    m_groupNodeList.append(groupNode);
}

void KConfigXtReaderItem::clear()
{
    m_groupNodeList.clear();
}

QList<KConfigXtReaderItem::GroupNode> KConfigXtReaderItem::groupNodes() const
{
    return m_groupNodeList;
}

bool KConfigXtReaderItem::isEmpty()
{
    return m_groupNodeList.isEmpty();
}

KConfigXtReader::KConfigXtReader(QObject *parent)
        : QObject(parent)
{
}

void KConfigXtReader::setConfigXmlFile(const QString& filename)
{
    m_filename = filename;
}

bool KConfigXtReader::parse()
{
    m_data.clear();

    bool parseResult = true;
    QString parseError;

    QFile f(m_filename);
    if (!f.open(QIODevice::ReadOnly)) {
        parseResult = false;
        parseError = i18n("The xml file isn't writable");
    }

    if (parseResult) {
        QDomDocument document;
        document.setContent(&f);

        //iterate inside it

        //this is the root element
        QDomElement docElem = document.documentElement();

        //this is the first child
        QDomNode it = docElem.firstChild();

        while(!it.isNull() && parseResult) {
            if (it.nodeName() == "group") {
                //continue, its a group
                const QString groupName = attributeValue(it, "name");
                if (groupName.isEmpty()) {
                    parseError = i18n("The group element doesn't have a name");
                    break;
                }

                KConfigXtReaderItem::GroupNode groupNode;
                groupNode.groupName = groupName;

                QDomNodeList childList = it.childNodes();
                //check if the node has children
                if (!childList.isEmpty()) {
                    for(uint i = 0; i < childList.length(); i++) {

                        const QDomNode child = childList.at(i);
                        if (child.nodeName() == "entry") {
                            KConfigXtReaderItem::EntryNode entryNode;
                            entryNode.groupName = groupName;

                            //we have an entry element
                            const QString entryName = attributeValue(child, "name");
                            const QString entryType = attributeValue(child, "type");
                            if (entryName.isEmpty()) {
                                parseError = i18n("The entry element doesn't have a name");
                                break;
                            }

                            entryNode.entryName = entryName;

                            if (entryType.isEmpty()) {
                                parseError = i18n("The entry element doesn't have a type");
                                break;
                            }

                            entryNode.entryType = entryType;

                            //until now we have a valid group and a valid entry
                            //so lets take those data, if we have more we will
                            //take them later

                           //now check if the child has more children
                            QDomNodeList childList2 = child.childNodes();
                            if (!childList2.isEmpty()) {
                                for(uint j = 0; j < childList2.length(); j++) {
                                    const QDomNode child2 = childList2.at(j);

                                    bool descriptionExists = true;

                                    if (child2.nodeName() == "label") {
                                        entryNode.entryDescriptionType = KConfigXtReaderItem::Label;
                                    } else if (child2.nodeName() == "tooltip") {
                                        entryNode.entryDescriptionType = KConfigXtReaderItem::ToolTip;
                                    } else if (child2.nodeName() == "whatsthis") {
                                        entryNode.entryDescriptionType = KConfigXtReaderItem::WhatsThis;
                                    } else if (child2.nodeName() == "default") {
                                        entryNode.entryValue = child2.toElement().text();
                                        descriptionExists = false;
                                    } else {
                                        descriptionExists = false;
                                    }
                                    if (descriptionExists) {
                                        entryNode.entryDescriptionValue = child2.toElement().text();
                                    }
                                }
                            }
                            groupNode.entryNodeList.append(entryNode);
                        }
                    }

                }
                m_data.appendGroupNode(groupNode);
                //we have parsed all the element so go to the next one
                it = it.nextSibling();
            } else {
                //its not a group element, so go to the next one
                it = it.nextSibling();
            }
        }
    }

    if (!parseResult) {
        KMessageBox::error(0, parseError);
    }

    return parseResult;
}


QString KConfigXtReader::attributeValue(const QDomNode element, const QString& attributeName) const
{
    QDomElement e = element.toElement();
    if (e.hasAttribute(attributeName)) {
        return e.attribute(attributeName);
    }
    return QString();
}

KConfigXtReaderItem KConfigXtReader::data() const
{
    return m_data;
}
