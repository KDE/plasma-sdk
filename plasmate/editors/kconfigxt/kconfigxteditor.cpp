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

#include "kconfigxteditor.h"
#include <KComboBox>
#include <QDebug>
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
    connect(m_ui.pbAddEntry, SIGNAL(clicked()), this, SLOT(createNewEntry()));

    connect(m_ui.twGroups, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(setupWidgetsForEntries(QTreeWidgetItem*)));

    connect(m_ui.twEntries, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(entryItemWidgetState(QTreeWidgetItem*, QTreeWidgetItem*)));

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

void KConfigXtEditor::setFilename(const QUrl& filename)
{
    m_filename = filename;
    //load the xml file
}

QUrl KConfigXtEditor::filename()
{
    return m_filename;

}

void KConfigXtEditor::readFile()
{
    m_writer.setConfigXmlFile(filename().pathOrUrl());

    if (m_filename.isEmpty()) {
        qDebug() << "Empty filename given!";
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

void KConfigXtEditor::clear()
{
    m_ui.twEntries->clear();
    m_ui.twGroups->clear();
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

    takeDataFromParser();

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

    KConfigXtReaderItem::GroupNode groupNode;
    groupNode.groupName = newGroupName;

    KConfigXtReaderItem::EntryNode newEntryItem;
    newEntryItem.groupName = newGroupName;
    newEntryItem.entryName = "TODO";
    newEntryItem.entryType = "string";
    newEntryItem.entryDescriptionType = KConfigXtReaderItem::Label;
    newEntryItem.entryDescriptionValue = "TODO";
    newEntryItem.entryValue = "TODO";

    m_writer.addNewGroup(newGroupName);
    m_writer.addNewEntry(newEntryItem);

    //add our new entry into the tree
    addEntryToUi(newEntryItem.entryName,
                 newEntryItem.entryType, newEntryItem.entryValue,
                 newEntryItem.entryDescriptionValue, newEntryItem.entryDescriptionType);
}

void KConfigXtEditor::createNewEntry()
{
    QTreeWidgetItem *currentGroupItem = m_ui.twGroups->currentItem();

    //check if the ptr is evil!!
    if (!currentGroupItem) {
        return;
    }

    takeDataFromParser();

    QString groupName = currentGroupItem->text(0);
    QStringList entryNameList;


    for(int i = 0; i < m_parser.data().groupNodes().length(); i++) {
        KConfigXtReaderItem::GroupNode it = m_parser.data().groupNodes().at(i);
        if (groupName == it.groupName) {
            for (int j = 0; j < it.entryNodeList.length(); j++) {
                KConfigXtReaderItem::EntryNode it2 = it.entryNodeList.at(j);
                entryNameList << it2.entryName;
            }
        }
    }

    if (entryNameList.contains("TODO")) {
        QString text = i18n("There is already a TODO entry,"
            " fix it before you continue");
        KMessageBox::information(this, text);
        return;
    }

    KConfigXtReaderItem::EntryNode newEntryItem;
    newEntryItem.groupName = groupName;
    newEntryItem.entryName = "TODO";
    newEntryItem.entryValue = "TODO";
    newEntryItem.entryType = "string";
    newEntryItem.entryDescriptionType = KConfigXtReaderItem::Label;
    newEntryItem.entryDescriptionValue = "TODO";

    //add our new entry into the tree
    addEntryToUi(newEntryItem.entryName, newEntryItem.entryType, newEntryItem.entryValue,
                 newEntryItem.entryDescriptionValue, newEntryItem.entryDescriptionType);

    m_writer.addNewEntry(newEntryItem);
}

void KConfigXtEditor::setupWidgetsForGroups()
{
    takeDataFromParser();

    foreach(const QString& group, m_groups) {
        addGroupToUi(group);
    }
}

void KConfigXtEditor::addGroupToUi(const QString& group)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(m_ui.twGroups);
    item->setText(0, group);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    m_ui.twGroups->setCurrentItem(item);

    m_lastGroupItem = group;
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

    takeDataFromParser();
    for (int i = 0; i < m_parser.data().groupNodes().length(); i++) {
        KConfigXtReaderItem::GroupNode it = m_parser.data().groupNodes().at(i);
        //check if this group has an entry
        if (item->text(0) == it.groupName) {
            for(int j = 0; j < it.entryNodeList.length(); j++) {
                KConfigXtReaderItem::EntryNode entryNode = it.entryNodeList.at(j);
                addEntryToUi(entryNode.entryName, entryNode.entryType,
                entryNode.entryValue, entryNode.entryDescriptionValue, entryNode.entryDescriptionType);
            }
        }
    }
}

void KConfigXtEditor::addEntryToUi(const QString& key, const QString& type,
                                   const QString& value, const QString& descriptionValue,
                                   KConfigXtReaderItem::DescriptionType descriptionType)
{
    KComboBox *descriptionButton = new KComboBox();
    descriptionButton->addItem("Label");
    descriptionButton->addItem("ToolTip");
    descriptionButton->addItem("WhatsThis");

    //we need a valid QTreeWidgetItem in order to enable the button,
    //so disable it for now, and leave the signal to do the work!
    descriptionButton->setEnabled(false);

    connect(descriptionButton, SIGNAL(currentIndexChanged(int)), this, SLOT(modifyTypeDescription()));

    //find the correct index
    if (descriptionType == KConfigXtReaderItem::Label) {
        descriptionButton->setCurrentIndex(0);
        m_lastEntryItem["descriptionType"] = "label";
    } else if (descriptionType == KConfigXtReaderItem::ToolTip) {
        descriptionButton->setCurrentIndex(1);
        m_lastEntryItem["descriptionType"] = "ToolTip";
    } else if (descriptionType == KConfigXtReaderItem::WhatsThis) {
        descriptionButton->setCurrentIndex(2);
        m_lastEntryItem["descriptionType"] = "WhatsThis";
    }

    QTreeWidgetItem *item = new QTreeWidgetItem(m_ui.twEntries);
    item->setText(0, key);
    item->setText(1, type);
    item->setText(2, value);
    item->setText(4, descriptionValue);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    m_ui.twEntries->setItemWidget(item, 3, descriptionButton);

}

void KConfigXtEditor::modifyGroup(QTreeWidgetItem* item, int column)
{
    //check if ptr is evil
    //and if the name of the group has been changed
    if (!item ||
        !item->isSelected()) {
        return;
    }

    //take the groups
    const QString oldGroupName = m_lastGroupItem;
    const QString newGroupName = item->text(column);

    m_writer.editGroupName(oldGroupName, newGroupName);
}

void KConfigXtEditor::setLastGroupItem(QTreeWidgetItem* item, int column)
{
    //check if ptr is evil
    if (!item) {
        return;
    }
    m_lastGroupItem = item->text(column);
}

void KConfigXtEditor::modifyTypeDescription()
{
    modifyEntry(m_ui.twEntries->currentItem(), m_ui.twEntries->currentColumn());
}

void KConfigXtEditor::modifyEntry(QTreeWidgetItem* item, int column)
{
    //* check if ptr is evil
    //* make sure that this method will be called
    //only when we modify an item. When do we modify an item?
    //When we click on it. The itemChanged signal is also called
    //on the creation of a new entry.
    if (!item || !item->isSelected()) {
        return;
    }

    takeDataFromParser();

    //create the entry
    const QString oldEntryName = m_lastEntryItem["name"];
    if (column == 0) {
        const QString newEntryName = item->text(0);
        m_writer.editEntry(m_lastGroupItem, oldEntryName, "name", newEntryName);
    } else if (column == 1) {
        const QString oldEntryType = m_lastEntryItem["type"];
        const QString newEntryType = item->text(1);
        m_writer.editEntry(m_lastGroupItem, oldEntryName, "type", newEntryType);
    } else if (column == 2) {
        const QString oldEntryValue = m_lastEntryItem["value"];
        const QString newEntryValue = item->text(2);
        m_writer.editEntry(m_lastGroupItem, oldEntryName, "default", newEntryValue);
    } else if (column == 3) {
        KComboBox *bt = qobject_cast<KComboBox*>(m_ui.twEntries->itemWidget(item, column));

        //check if the ptr is evil
        if (!bt) {
            return;
        }

        const QString oldDescriptionType = m_lastEntryItem["descriptionType"];
        if (bt->currentIndex() == 0) {
            //it's a label
            m_writer.editEntry(m_lastGroupItem, oldEntryName, "label", "label");
            //our descriptionType is about to change, so update its value
            m_lastEntryItem["descriptionType"] = "label";
        } else if (bt->currentIndex() == 1) {
            //it's a tooltip
            m_writer.editEntry(m_lastGroupItem, oldEntryName, "tooltip", "tooltip");
            //our descriptionType is about to change, so update its value
            m_lastEntryItem["descriptionType"] = "tooltip";
        } else if (bt->currentIndex() == 2) {
            //it's a whatsthis
            m_writer.editEntry(m_lastGroupItem, oldEntryName, "whatsthis", "whatsthis");
            //our descriptionType is about to change, so update its value
            m_lastEntryItem["descriptionType"] = "whatsthis";
        }
    } else if (column == 4) {
        const QString newDescriptionValue = item->text(4);

        //m_writer.editEntry(m_lastGroupItem, oldEntryName, m_lastEntryItem["descriptionType"], newDescriptionValue);

        m_writer.editEntry(m_lastGroupItem, oldEntryName, QString(), newDescriptionValue);
    }
}

void KConfigXtEditor::entryItemWidgetState(QTreeWidgetItem *currentItem, QTreeWidgetItem *previousItem)
{

    //we need to check every button/item individually because,
    //the first time that the signal will be emitted the previousItem
    //will be 0, because there is no previousItem at that time, only
    //the currentItem is valid

    KComboBox *previousBt = 0;
    KComboBox *currentBt = 0;

    if (previousItem) {
        previousBt = qobject_cast<KComboBox*>(m_ui.twEntries->itemWidget(previousItem, 3));

        if (previousBt) {
            if (previousBt->isEnabled()) {
                previousBt->setEnabled(false);
            }
        }
    }

    if (currentItem) {
        currentBt = qobject_cast<KComboBox*>(m_ui.twEntries->itemWidget(currentItem, 3));

        if (currentBt) {
            if (!currentBt->isEnabled()) {
                currentBt->setEnabled(true);
            }
        }
    }

}

void KConfigXtEditor::setLastEntryItem(QTreeWidgetItem* item)
{
    //check if ptr is evil
    if (!item) {
        return;
    }
    //our tree has 4 columns and those columns doesn't move
    m_lastEntryItem["name"] = item->text(0);
    m_lastEntryItem["type"] = item->text(1);
    m_lastEntryItem["value"] = item->text(2);
    m_lastEntryItem["descriptionValue"] = item->text(4);
}

void KConfigXtEditor::removeGroup()
{
    //take the current item of the tree
    QTreeWidgetItem *item = m_ui.twGroups->currentItem();

    if (!item) {
        return;
    }

    if (m_writer.removeGroup(item->text(0))) {
        //remote the group from the ui,
        //we have already deleted it from the xml file
        delete m_ui.twGroups->currentItem();

        //clear the tree of the entries
        m_ui.twEntries->clear();
    } else {
        removeError();
    }
}

void KConfigXtEditor::removeEntry()
{
    //take the current item of the tree
    QTreeWidgetItem *item = m_ui.twEntries->currentItem();

    if (!item) {
        return;
    }

    if (m_writer.removeEntry(m_lastGroupItem, item->text(0))) {
        //remote the entry from the ui,
        //we have already deleted it from the xml file
        delete m_ui.twEntries->currentItem();
    } else {
        removeError();
    }
}

void KConfigXtEditor::removeError()
{
    QString error = i18n("An error has occurred during the removal of the item");
    KMessageBox::error(this, error);
}


void KConfigXtEditor::takeDataFromParser()
{
    m_parser.parse();
    m_writer.setData(m_parser.data().groupNodes());

    QList<KConfigXtReaderItem::GroupNode> dataList = m_parser.data().groupNodes();

    m_groups.clear();

    for (int i = 0; i < dataList.length(); i++) {
        m_groups << dataList.at(i).groupName;
    }
}
