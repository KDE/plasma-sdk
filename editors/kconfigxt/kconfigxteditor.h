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
#ifndef KCONFIGXT_EDITOR_H
#define KCONFIGXT_EDITOR_H

#include "ui_kconfigxteditor.h"

#include "kconfigxtparser.h"

#include <QWidget>
#include <KUrl>

class QTreeWidgetItem;

class KConfigXtEditor : public QWidget
{
    Q_OBJECT

public:
    KConfigXtEditor(QWidget *parent = 0);

    enum ElementType {
        Group = 0,
        Entry
    };

    /**
     * Sets filename to edit
     */
    void setFilename(const KUrl& filename);

public slots:
    /**
     * Sets up editor widgets according to contents of config file
     * specified with setFilename
     */
    void readFile();

    /**
     * Writes config values to config file specified with setFilename according
     * to current editor widgets state
     */
    void writeFile();

private slots:
    /**
     * Creates new kconfig group
     */
    void createNewGroup();

    /**
     * Sets up editor widgets for
     * the groups. This method should be called every time that the
     * a group is modified/deleted/etc.
     **/
    void setupWidgetsForGroups();

    /**
     * Sets up editor widgets for
     * the entries. This method should be called every time that the
     * an entry is modified/deleted/etc.
     **/
    void setupWidgetsForEntries(QTreeWidgetItem *item);

    /**
     * Removes a group from the xml file
     **/
    void removeGroup();

    /**
     * Removes an entry from the xml file
     **/
    void removeEntry();

    /**
     * Modifies a group from the xml file
     **/
    void modifyGroup(QTreeWidgetItem *item, int column);

    /**
     * Modifies an entry from the xml file
     **/
    void modifyEntry(QTreeWidgetItem *item, int column);

    /**
     * sets the last item of the group treewidget
     **/
    void setLastGroupItem(QTreeWidgetItem* item, int column);

    /**
     * sets the last item of the entry treewidget
     **/
    void setLastEntryItem(QTreeWidgetItem* item);

protected:
    Ui::KConfigXtEditor m_ui;

private:
    /**
     * Sets up editor widgets for an existing file
     * (e.g. creates a default group etc)
     */
    void setupWidgetsForOldFile();

    /**
     * Sets up editor widgets for a new file
     * (e.g. creates a default group etc)
     */
    void setupWidgetsForNewFile();

    /**
     * This method takes the groups from the parser
     * If group is specified it will also take the
     * keys,values and types from the parser for the specified group
     *
     * NOTE: after the call of this method use can use m_groups
     * and m_keysValuesTypes
     */

    void takeDataFromParser(const QString& group = QString());

    //with this method we avoid duplication
    void addGroupToUi(const QString& group);

    //with this method we can avoid duplication
    void addEntryToUi(const QString& entryName,
                                const QString& entryType, const QString& entryValue);

    //with this method we can avoid duplication,
    //this method will delete everything that is between the startsWith
    //and endsWith
    bool removeElement(const QString& elementName, ElementType elementType);

    //reduces duplication
    void removeError();

    KUrl m_filename;
    QStringList m_groups;
    QList<KConfigXtParserItem> m_keysValuesTypes;

    KConfigXtParser m_parser;

    QString m_lastGroupItem;
    //avoid duplication
    QString stringToGroupEntry(const QString& groupName) const;

    QHash<QString, QString> m_lastEntryItem;
    //avoid duplication
    QString stringToEntryAndValue(const QString& entryName, const QString entryType);

    //avoid duplication
    void replaceItemsInXml(const QString& oldItem, const QString& newItem);
};

#endif
