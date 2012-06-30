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
#include <QFile>
#include <QHeaderView>
#include <QTreeWidgetItem>

KConfigXtEditor::KConfigXtEditor(QWidget *parent)
        : QWidget(parent)
{
    m_ui.setupUi(this);

    m_ui.twEntries->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_ui.twGroups->header()->setResizeMode(QHeaderView::ResizeToContents);

    m_ui.lblHintIcon->setPixmap(KIcon("dialog-information").pixmap(16, 16));

    connect(m_ui.pbAddGroup, SIGNAL(clicked()), this, SLOT(createNewGroup()));
    connect(m_ui.twGroups, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(setupWidgetsForEntries(QTreeWidgetItem*)));
    connect(m_ui.pbDeleteGroup, SIGNAL(clicked()), this, SLOT(removeGroup()));

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
        //parse the xml
        m_parser.parse();
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

void KConfigXtEditor::setupWidgetsForEntries(QTreeWidgetItem *item)
{
    //the currectIndex of m_ui.twGroups has changed.
    //this means that we need to load the data for the new
    //group so remove the old items
    m_ui.twEntries->clear();

    if (!item) {
        return;
    }

    //take keys,values and types for the specified group
    takeDataFromParser(item->text(0));

    addEntryToUi(m_keysValuesTypes.entryName(), m_keysValuesTypes.entryType(), m_keysValuesTypes.entryValue());

}

void KConfigXtEditor::takeDataFromParser(const QString& group)
{
    foreach(const KConfigXtParserItem& item, m_parser.dataList()) {

        //take the name of the groups
        if (!item.groupName().isEmpty()) {
            m_groups << item.groupName();
        }

        if (!group.isEmpty()) {
            if (item.groupName() == group) {
                //we have specified a group
                //so probably we want to populate the
                //m_ui.twEntries.
                m_keysValuesTypes = item;
            } else {
                //we haven't specified a group.
                //So we don't want populate the m_ui.twEntries.
                //clear the item.
                m_keysValuesTypes = KConfigXtParserItem();
            }
        }
    }

}

void KConfigXtEditor::addGroupToUi(const QString& group)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, group);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    m_ui.twGroups->addTopLevelItem(item);

    m_ui.twGroups->setCurrentItem(item);
    m_ui.twGroups->editItem(item);
    //TODO mem leak?
}

void KConfigXtEditor::addEntryToUi(const QString& key, const QString& type, const QString& value)
{
    qDebug() << "key:" + key;
    qDebug() << "type:" + type;
    qDebug() << "value:" + value;
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, key);
    item->setText(1, type);
    item->setText(2, value);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    m_ui.twEntries->addTopLevelItem(item);
}

void KConfigXtEditor::removeGroup()
{
    QByteArray array;
    
    QTreeWidgetItem *item = m_ui.twGroups->currentItem();

    QString group = "<group name=\"" + item->text(0) + "\">";

    QFile xmlFile(m_filename.pathOrUrl());
    if(!xmlFile.open(QIODevice::ReadWrite)) {
        return;
    }

    QTextStream text(&xmlFile);

    while (!text.atEnd()) {
        QString line = text.readLine();
        if (line == group) {
            while (line != "</group>") {
                array.replace(line,"");
            }
        }
    }
}
