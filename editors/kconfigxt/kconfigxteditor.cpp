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

    m_ui.twKeyValues->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_ui.twGroups->header()->setResizeMode(QHeaderView::ResizeToContents);

    m_ui.lblHintIcon->setPixmap(KIcon("dialog-information").pixmap(16, 16));

    connect(m_ui.pbAddGroup, SIGNAL(clicked()), SLOT(createNewGroup()));

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

    if (!QFile::exists(m_filename.pathOrUrl())) {
        setupWidgetsForNewFile();
        return;
    } else {
        m_parser.setConfigXmlFile(m_filename.pathOrUrl());
        m_parser.parse();
    }

    // TODO: reading goes here
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

void KConfigXtEditor::createNewGroup()
{
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

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, newGroupName);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    m_ui.twGroups->addTopLevelItem(item);

    m_ui.twGroups->setCurrentItem(item);
    m_ui.twGroups->editItem(item);
}
