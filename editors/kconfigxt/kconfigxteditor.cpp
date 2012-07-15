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

#include "kconfigxteditor.h"

#include <KDebug>
#include <KIcon>
#include <KMessageBox>
#include <KLocalizedString>

#include <QFile>
#include <QHeaderView>
#include <QTreeWidgetItem>

KConfigXtEditor::KConfigXtEditor(QWidget *parent)
        : QWidget(parent)
{
    m_ui.setupUi(this);

    //don't move the columns!!!
    m_ui.twEntries->header()->setMovable(false);

    m_ui.twEntries->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_ui.twGroups->header()->setResizeMode(QHeaderView::ResizeToContents);

    m_ui.lblHintIcon->setPixmap(KIcon("dialog-information").pixmap(16, 16));

    connect(m_ui.pbAddGroup, SIGNAL(clicked()), this, SLOT(createNewGroup()));

    connect(m_ui.twGroups, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(setupWidgetsForEntries(QTreeWidgetItem*)));

    connect(m_ui.twGroups, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(setLastGroupItem(QTreeWidgetItem*, int)));
    connect(m_ui.twEntries, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(setLastEntryItem(QTreeWidgetItem*)));

    connect(m_ui.twGroups, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(modifyGroup(QTreeWidgetItem*, int)));
    connect(m_ui.twEntries, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(modifyEntry(QTreeWidgetItem*, int)));

    connect(m_ui.pbDeleteGroup, SIGNAL(clicked()), this, SLOT(removeGroup()));
    connect(m_ui.pbDeleteEntry, SIGNAL(clicked()), this, SLOT(removeEntry()));

    //hide the source related ui stuff
    m_ui.srcLabel1->setVisible(false);
    m_ui.srcLabel2->setVisible(false);
    m_ui.srcRequester->setVisible(false);
    m_ui.srcSeparator->setVisible(false);
}

void KConfigXtEditor::setFilename(const KUrl& filename)
{
    m_filename = filename;
}

void KConfigXtEditor::readFile()
{
    if (m_filename.isEmpty()) {
        kDebug() << "Empty filename given!";
        return;
    }

    //check if the xml exists
    if (!QFile::exists(m_filename.pathOrUrl())) {
        setupWidgetsForNewFile();
        return;
    } else {
        m_parser.setConfigXmlFile(m_filename.pathOrUrl());
        takeDataFromParser();
        setupWidgetsForOldFile();
    }
}

void KConfigXtEditor::writeFile()
{
    // TODO: writing goes here
}

void KConfigXtEditor::setupWidgetsForNewFile()
{
    // Add default group
    createNewGroup();
}

void KConfigXtEditor::setupWidgetsForOldFile()
{
    setupWidgetsForGroups();
}

void KConfigXtEditor::createNewGroup()
{
    m_ui.twEntries->clear();
    QString newGroupName;
    if (m_groups.isEmpty()) {
        newGroupName = "General";
    } else {
        int counter = 1;
        newGroupName = QString("Group %1").arg(counter);
        while (m_groups.contains(newGroupName)) {
            counter++;
            newGroupName = QString("Group %1").arg(counter);
        }
    }

    m_groups.append(newGroupName);

    addGroupToUi(newGroupName);
}


void KConfigXtEditor::setupWidgetsForGroups()
{
    foreach(const QString& group, m_groups) {
        addGroupToUi(group);
    }
}

void KConfigXtEditor::addGroupToUi(const QString& group)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(m_ui.twGroups);
    item->setText(0, group);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    //TODO mem leak?
}

void KConfigXtEditor::setupWidgetsForEntries(QTreeWidgetItem *item)
{
    if (!item) {
        return;
    }

    //the currectIndex of m_ui.twGroups has changed.
    //this means that we need to load the data for the new
    //group so remove the old items
    m_ui.twEntries->clear();

    //also we will take new keys values and types so clear our list
    m_keysValuesTypes.clear();

    //take keys,values and types for the specified group
    takeDataFromParser(item->text(0));

   foreach(const KConfigXtParserItem& item, m_keysValuesTypes) {
       addEntryToUi(item.entryName(), item.entryType(), item.entryValue());
    }

}

void KConfigXtEditor::addEntryToUi(const QString& key, const QString& type, const QString& value)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(m_ui.twEntries);
    item->setText(0, key);
    item->setText(1, type);
    item->setText(2, value);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void KConfigXtEditor::takeDataFromParser(const QString& group)
{

    //we need to update the data of our parser first
    m_parser.parse();

    foreach(const KConfigXtParserItem& item, m_parser.dataList()) {

        //take the name of the groups also due to the fact
        //that we take data from a parser we have to be careful so
        //check if the group exists in the list
        if (!item.groupName().isEmpty() && !m_groups.contains(item.groupName())) {
            m_groups << item.groupName();
        }

        if (!group.isEmpty() && item.groupName() == group
            && !m_keysValuesTypes.contains(item)) {
            //we have specified a group
            //so probably we want to populate the
            //m_ui.twEntries.
            m_keysValuesTypes.append(item);
        }
    }
}

void KConfigXtEditor::modifyGroup(QTreeWidgetItem* item, int column)
{
    //check if ptr is evil
    //and if the name of the group has been changed
    if (!item || item->text(column) == m_lastGroupItem) {
        return;
    }

    //take the groups
    const QString oldGroupEntry = stringToGroupEntry(m_lastGroupItem);
    const QString newGroupEntry = stringToGroupEntry(item->text(column));

    //replace the groups in the xml
    replaceItemsInXml(oldGroupEntry, newGroupEntry);
}

void KConfigXtEditor::setLastGroupItem(QTreeWidgetItem* item, int column)
{
    //check if ptr is evil
    if (!item) {
        return;
    }

    m_lastGroupItem = item->text(column);
}

QString KConfigXtEditor::stringToGroupEntry(const QString& groupName) const
{
    return QString("<group name=\"%1\">").arg(groupName);
}

void KConfigXtEditor::modifyEntry(QTreeWidgetItem* item, int column)
{
    //check if ptr is evil
    if (!item ) {
        return;
    }

    //create the entry
    const QString oldEntry = stringToEntryAndValue(m_lastEntryItem["name"],
                                                   m_lastEntryItem["type"]);

    if (column == 0 || column == 1) {
        //the user has modified either the
        //key or its type.
        //TODO maybe it should check if the type is valid

        const QString newEntry = stringToEntryAndValue(item->text(0), item->text(1));

        //replace the entries in the xml
        replaceItemsInXml(oldEntry, newEntry);

    } else if (column == 2) {
        QFile xmlFile(m_filename.pathOrUrl());

        if(!xmlFile.open(QIODevice::ReadWrite)) {
            return;
        }

       QByteArray text;

       while (!xmlFile.atEnd()) {
            QString line = xmlFile.readLine();
            if (line.contains(oldEntry)) {
                while (!line.contains("</entry>")) {
                    //we took this from the xml spec
                    QString startsWith = "<default>";
                    QString endsWith = "</default>";
                    if (line.startsWith(startsWith)
                        && line.endsWith(endsWith)) {
                            line.replace(oldEntry, startsWith
                            + item->text(2) + endsWith);
                    }
               }
            }
            text.append(line);
       }

       //clear the xml fle
       xmlFile.resize(0);

       //write the data
       xmlFile.write(text);

       //close the file
       xmlFile.close();
    }
}

void KConfigXtEditor::setLastEntryItem(QTreeWidgetItem* item)
{
    //check if ptr is evil
    if (!item) {
        return;
    }

    //our tree has 3 columns and those columns doesn't move
    m_lastEntryItem["name"] = item->text(0);
    m_lastEntryItem["type"] = item->text(1);
    m_lastEntryItem["value"] = item->text(2);
}

QString KConfigXtEditor::stringToEntryAndValue(const QString& entryName, const QString entryType)
{
    return QString("<entry name=\"%1\" type=\"%2\">").arg(entryName).arg(entryType);
}

void KConfigXtEditor::replaceItemsInXml(const QString& oldItem, const QString& newItem)
{
    //take the xml file
    QFile xmlFile(m_filename.pathOrUrl());

    if(!xmlFile.open(QIODevice::ReadWrite)) {
        //the xml file has failed to open
        return;
    }

    QByteArray rawData = xmlFile.readAll();

    //replace the data
    rawData.replace(oldItem, newItem.toAscii());

    //clear the xml fle
    xmlFile.resize(0);

    //write the data
    xmlFile.write(rawData);

    //close the file
    xmlFile.close();

}

void KConfigXtEditor::removeGroup()
{
    //take the current item of the tree
    QTreeWidgetItem *item = m_ui.twGroups->currentItem();
    if (removeElement(item->text(0), KConfigXtEditor::Group)) {
        //remote the group from the ui,
        //we have already deleted it from the xml file
        delete m_ui.twGroups->currentItem();
    } else {
        removeError();
    }
}

void KConfigXtEditor::removeEntry()
{
    //take the current item of the tree
    QTreeWidgetItem *item = m_ui.twEntries->currentItem();
    if (removeElement(item->text(0), KConfigXtEditor::Entry)) {
        //remote the entry from the ui,
        //we have already deleted it from the xml file
        delete m_ui.twEntries->currentItem();
    } else {
        removeError();
    }
}

void KConfigXtEditor::removeError()
{
    QString error = i18n("An error has occured during the removal of the item");
    KMessageBox::error(this, error);
}


bool KConfigXtEditor::removeElement(const QString& elementName, ElementType elementType)
{
    if (elementName.isEmpty()) {
        return false;
    }

    //take the element that we want
    QString startElement;
    if (elementType == KConfigXtEditor::Group) {
        startElement  = "<group name=\"" + elementName + "\">";
    } else if (elementType == KConfigXtEditor::Entry) {
        startElement  = "<entry name=\"" + elementName + "\">";
    }

    QFile xmlFile(m_filename.pathOrUrl());
    if(!xmlFile.open(QIODevice::ReadWrite)) {
        return false;
    }

    //Let me exaplain what is going on in this method,
    //there is no way in Qt to remove the contents of
    //a file if we don't use QTextEdit. So we can only keep
    //the content which we don't want to delete then to empty
    //our file and finally to add the content which we don't
    //want to delete. So, its like we delete something!
    QByteArray text;

    while (!xmlFile.atEnd()) {
        QString line = xmlFile.readLine();
        //while the element has started and the element
        //hasn't ended skip those lines.
        if (line.contains(startElement)) {
            //choose our flag according to the give type;
            QString endElement;
            if (elementType == KConfigXtEditor::Group) {
                endElement = "</group>";
            } else if (elementType == KConfigXtEditor::Entry) {
                endElement = "</entry>";
            }

            while (!line.contains(endElement)) {
                line = xmlFile.readLine();
            }

            //if you are the last element which
            //you have escaped from the while, skip!
            if (line.contains(endElement)) {
                line.clear();
            }
        }
        text.append(line);
    }

    //empty the xml file
    xmlFile.resize(0);

    //write our text and close the file
    xmlFile.write(text);
    xmlFile.close();

    return true;
}
